// Copyright srSerge. All Rights Reserved.

#include "Components/MaceCollisionComponent.h"

#include "AbilitySystemGlobals.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"

#if ENABLE_DRAW_DEBUG
#include "DrawDebugHelpers.h"
#endif

#include "Interfaces/MaceCombatant.h"
#include "MaceLogChannels.h"

UMaceCollisionComponent::UMaceCollisionComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Ticking is the expensive part, and an attack is a fraction of a second.
	// The component only ticks between BeginWindow and EndWindow, so a
	// character standing still costs nothing.
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

UMaceCollisionComponent* UMaceCollisionComponent::FindCollisionComponent(const AActor* Actor)
{
	return Actor ? Actor->FindComponentByClass<UMaceCollisionComponent>() : nullptr;
}

void UMaceCollisionComponent::BeginWindow(USkeletalMeshComponent* Mesh, const TArray<FName>& Sockets, float Radius)
{
	const AActor* Owner = GetOwner();

	if (bAuthorityOnly && Owner && !Owner->HasAuthority())
	{
		return;
	}

	// Null means the unarmed case: the character's own mesh. A weapon passes
	// its own, which is the only difference between a punch and a sword.
	TracedMesh = Mesh ? Mesh : (Owner ? Owner->FindComponentByClass<USkeletalMeshComponent>() : nullptr);

	if (!TracedMesh)
	{
		UE_LOG(LogMaceDamage, Warning, TEXT("[%s] opened a collision window with no skeletal mesh to trace."), *GetNameSafe(Owner));
		return;
	}

	if (Radius <= 0.0f)
	{
		UE_LOG(LogMaceDamage, Warning, TEXT("[%s] opened a collision window with a radius of %.2f, which can never touch anything."), *GetNameSafe(Owner), Radius);
		return;
	}

	// Checked once here rather than every frame. A misspelled socket otherwise
	// resolves to the component's own origin and traces from the character's
	// feet without ever reporting a problem.
	TracedSockets.Reset();
	for (const FName& Socket : Sockets)
	{
		if (TracedMesh->DoesSocketExist(Socket))
		{
			TracedSockets.Add(Socket);
		}
		else
		{
			UE_LOG(LogMaceDamage, Warning, TEXT("[%s] has no socket or bone named [%s]; it will not be traced."), *GetNameSafe(TracedMesh.Get()), *Socket.ToString());
		}
	}

	if (TracedSockets.IsEmpty())
	{
		UE_LOG(LogMaceDamage, Warning, TEXT("[%s] opened a collision window with no usable sockets."), *GetNameSafe(Owner));
		return;
	}

	TraceRadius = Radius;

	if (bClearHitsOnWindowBegin)
	{
		ResetHitActors();
	}

	// The first frame has no previous position to sweep from, so it starts from
	// where the sockets are now and the sweeping begins on the next tick.
	BuildSamplePoints(PreviousPoints);

	bWindowOpen = true;
	SetComponentTickEnabled(true);

	UE_LOG(LogMaceDamage, Verbose, TEXT("[%s] opened a collision window on [%s] with %d socket(s), radius %.1f."),
		*GetNameSafe(Owner), *GetNameSafe(TracedMesh.Get()), TracedSockets.Num(), TraceRadius);
}

void UMaceCollisionComponent::EndWindow()
{
	if (!bWindowOpen)
	{
		return;
	}

	bWindowOpen = false;
	SetComponentTickEnabled(false);

	PreviousPoints.Reset();
	TracedSockets.Reset();
	TracedMesh = nullptr;

	UE_LOG(LogMaceDamage, Verbose, TEXT("[%s] closed its collision window."), *GetNameSafe(GetOwner()));
}

void UMaceCollisionComponent::ResetHitActors()
{
	HitActors.Reset();
}

bool UMaceCollisionComponent::IsValidTarget(AActor* Target) const
{
	if (!Target || Target == GetOwner())
	{
		return false;
	}

	// An ability system is what makes a hit mean something: without one there
	// is nothing to apply an effect to.
	if (!UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Target))
	{
		return false;
	}

	// Dead combatants are scenery. Anything that is not a combatant at all is
	// let through, so a destructible prop can still be hit.
	if (Target->Implements<UMaceCombatant>())
	{
		return IMaceCombatant::Execute_IsAlive(Target);
	}

	return true;
}

void UMaceCollisionComponent::BuildSamplePoints(TArray<FVector>& OutPoints) const
{
	OutPoints.Reset();

	if (!TracedMesh)
	{
		return;
	}

	const int32 SocketCount = TracedSockets.Num();

	for (int32 Index = 0; Index < SocketCount; ++Index)
	{
		const FVector SocketLocation = TracedMesh->GetSocketLocation(TracedSockets[Index]);
		OutPoints.Add(SocketLocation);

		if (SegmentSubdivisions <= 0 || Index + 1 >= SocketCount)
		{
			continue;
		}

		// Fill the edge between this socket and the next one. Without this a
		// blade is two points with a gap where the sharp part is.
		const FVector NextLocation = TracedMesh->GetSocketLocation(TracedSockets[Index + 1]);
		for (int32 Step = 1; Step <= SegmentSubdivisions; ++Step)
		{
			const float Alpha = static_cast<float>(Step) / static_cast<float>(SegmentSubdivisions + 1);
			OutPoints.Add(FMath::Lerp(SocketLocation, NextLocation, Alpha));
		}
	}
}

void UMaceCollisionComponent::SweepBetweenFrames(const TArray<FVector>& CurrentPoints)
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	FCollisionQueryParams Params(SCENE_QUERY_STAT(MaceCollisionSweep), /*bTraceComplex=*/false, GetOwner());
	Params.bReturnPhysicalMaterial = true;

	const FCollisionShape Sphere = FCollisionShape::MakeSphere(TraceRadius);

	for (int32 Index = 0; Index < CurrentPoints.Num(); ++Index)
	{
		const FVector Start = PreviousPoints[Index];
		const FVector End = CurrentPoints[Index];

		TArray<FHitResult> Hits;
		World->SweepMultiByChannel(Hits, Start, End, FQuat::Identity, TraceChannel, Sphere, Params);

		for (const FHitResult& Hit : Hits)
		{
			AActor* HitActor = Hit.GetActor();

			if (!IsValidTarget(HitActor))
			{
				continue;
			}

			// One report per actor per swing. Without this a slow swing bills
			// the same target once a frame for as long as it overlaps.
			if (HitActors.Contains(HitActor))
			{
				continue;
			}

			HitActors.Add(HitActor);

			UE_LOG(LogMaceDamage, Verbose, TEXT("[%s] hit [%s] on bone [%s]."),
				*GetNameSafe(GetOwner()), *GetNameSafe(HitActor), *Hit.BoneName.ToString());

			OnHitDetected.Broadcast(HitActor, Hit);
		}

#if ENABLE_DRAW_DEBUG
		if (bDrawDebug)
		{
			DrawDebugLine(World, Start, End, FColor::Yellow, false, DebugDrawDuration);
			DrawDebugSphere(World, End, TraceRadius, 12, Hits.IsEmpty() ? FColor::Green : FColor::Red, false, DebugDrawDuration);
		}
#endif
	}
}

void UMaceCollisionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bWindowOpen || !TracedMesh)
	{
		return;
	}

	TArray<FVector> CurrentPoints;
	BuildSamplePoints(CurrentPoints);

	// Only sweep when both frames describe the same points. They can disagree
	// for one frame if the traced mesh changes underneath, and pairing point 3
	// of one shape with point 3 of another would trace a line across the level.
	if (CurrentPoints.Num() == PreviousPoints.Num())
	{
		SweepBetweenFrames(CurrentPoints);
	}

	PreviousPoints = MoveTemp(CurrentPoints);
}
