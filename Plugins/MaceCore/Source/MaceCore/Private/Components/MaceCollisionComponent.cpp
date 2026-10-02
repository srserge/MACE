// Copyright srSerge. All Rights Reserved.

#include "Components/MaceCollisionComponent.h"

#include "AbilitySystemGlobals.h"
#include "Components/ShapeComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "HAL/IConsoleManager.h"
#include "UObject/UObjectIterator.h"

#if ENABLE_DRAW_DEBUG
#include "DrawDebugHelpers.h"
#endif

#include "Components/MaceHurtboxBoxComponent.h"
#include "Components/MaceHurtboxCapsuleComponent.h"
#include "Interfaces/MaceCombatant.h"
#include "MaceLogChannels.h"

namespace
{
	/** How long the hurtbox that was hit stays red in the debug drawing. */
	constexpr double HitFlashSeconds = 0.5;

#if ENABLE_DRAW_DEBUG
	void OnDrawHurtboxesChanged(IConsoleVariable* Variable)
	{
		// Characters only tick to draw while the drawing is on, so switching it
		// has to wake them up or put them back to sleep.
		for (TObjectIterator<UMaceCollisionComponent> It; It; ++It)
		{
			if (!It->IsTemplate() && It->HasBegunPlay())
			{
				It->RefreshTickEnabled();
			}
		}
	}

	TAutoConsoleVariable<bool> CVarDrawHurtboxes(
		TEXT("mace.Debug.DrawHurtboxes"),
		false,
		TEXT("Draws every character's hurtboxes in green. The one hit last flashes red."),
		FConsoleVariableDelegate::CreateStatic(&OnDrawHurtboxesChanged),
		ECVF_Cheat);
#endif

	/** Where a hurtbox is right now, scale included, following its bone. */
	FTransform GetHurtboxWorldTransform(const FMaceHurtbox& Hurtbox)
	{
		const USkeletalMeshComponent* Mesh = Hurtbox.Mesh.Get();
		return Mesh ? Hurtbox.RelativeTransform * Mesh->GetSocketTransform(Hurtbox.AttachName, RTS_World) : FTransform::Identity;
	}

	/**
	 * Where along a segment it first enters an axis-aligned box centred on the
	 * origin, as a fraction from 0 to 1. A slab test: the segment is inside the
	 * box where it is inside all three pairs of planes at once.
	 */
	bool SegmentEntersBox(const FVector& Start, const FVector& End, const FVector& Extent, double& OutFraction)
	{
		const FVector Delta = End - Start;
		double Enter = 0.0;
		double Exit = 1.0;

		for (int32 Axis = 0; Axis < 3; ++Axis)
		{
			if (FMath::Abs(Delta[Axis]) < UE_KINDA_SMALL_NUMBER)
			{
				// Parallel to this pair of planes: inside them or never.
				if (FMath::Abs(Start[Axis]) > Extent[Axis])
				{
					return false;
				}
				continue;
			}

			double Near = (-Extent[Axis] - Start[Axis]) / Delta[Axis];
			double Far = (Extent[Axis] - Start[Axis]) / Delta[Axis];
			if (Near > Far)
			{
				Swap(Near, Far);
			}

			Enter = FMath::Max(Enter, Near);
			Exit = FMath::Min(Exit, Far);
			if (Enter > Exit)
			{
				return false;
			}
		}

		OutFraction = Enter;
		return true;
	}

	/**
	 * Whether a sphere sweeping from Start to End touches a hurtbox. On a hit,
	 * OutDistance is how far along the sweep the contact happens, so several
	 * hurtboxes can be compared to find the one touched first, and OutPoint is
	 * where on the hurtbox's surface it happens.
	 */
	bool SweepAgainstHurtbox(const FMaceHurtbox& Hurtbox, const FVector& Start, const FVector& End, float Radius, double& OutDistance, FVector& OutPoint)
	{
		if (!Hurtbox.Mesh.IsValid())
		{
			return false;
		}

		const FTransform WorldTransform = GetHurtboxWorldTransform(Hurtbox);
		const double SweepLength = FVector::Dist(Start, End);

		if (Hurtbox.bIsBox)
		{
			// Work in the box's own space, where it is axis-aligned. Growing it by
			// the sphere's radius turns a sphere sweep into a segment test; the
			// corners come out slightly generous, which a hit check can afford.
			const FVector Extent = Hurtbox.Extent * WorldTransform.GetScale3D().GetAbs();
			FTransform Unscaled = WorldTransform;
			Unscaled.SetScale3D(FVector::OneVector);

			const FVector LocalStart = Unscaled.InverseTransformPosition(Start);
			const FVector LocalEnd = Unscaled.InverseTransformPosition(End);

			double Fraction = 0.0;
			if (!SegmentEntersBox(LocalStart, LocalEnd, Extent + FVector(Radius), Fraction))
			{
				return false;
			}

			// The contact is where the sweep enters the grown box, pulled onto the
			// surface of the real one.
			const FVector LocalContact = FMath::Lerp(LocalStart, LocalEnd, Fraction);
			const FVector OnSurface = LocalContact.BoundToBox(-Extent, Extent);

			OutDistance = Fraction * SweepLength;
			OutPoint = Unscaled.TransformPosition(OnSurface);
			return true;
		}

		// A capsule is a segment with a radius, so a sphere sweep against it is two
		// segments closer together than the two radii added up. The engine scales
		// a capsule by its smallest axis, and this does the same.
		const double Scale = WorldTransform.GetMinimumAxisScale();
		const double CapsuleRadius = Hurtbox.Radius * Scale;
		const double SpineHalfLength = FMath::Max(0.0, Hurtbox.HalfHeight * Scale - CapsuleRadius);

		const FVector Centre = WorldTransform.GetLocation();
		const FVector Axis = WorldTransform.GetRotation().GetUpVector();
		const FVector SpineTop = Centre + Axis * SpineHalfLength;
		const FVector SpineBottom = Centre - Axis * SpineHalfLength;

		FVector OnSweep;
		FVector OnSpine;
		FMath::SegmentDistToSegmentSafe(Start, End, SpineTop, SpineBottom, OnSweep, OnSpine);

		const double Gap = FVector::Dist(OnSweep, OnSpine);
		const double Reach = Radius + CapsuleRadius;
		if (Gap > Reach)
		{
			return false;
		}

		// The closest approach comes after the first contact. Backing off along
		// the sweep by the overlap estimates when the sphere first touched, which
		// is what decides between two hurtboxes the same sweep passes through.
		const double Overlap = FMath::Sqrt(FMath::Max(0.0, Reach * Reach - Gap * Gap));
		OutDistance = FMath::Max(0.0, FVector::Dist(Start, OnSweep) - Overlap);

		const FVector Outward = (OnSweep - OnSpine).GetSafeNormal();
		OutPoint = OnSpine + Outward * CapsuleRadius;
		return true;
	}
}

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

void UMaceCollisionComponent::BeginPlay()
{
	Super::BeginPlay();

	GatherHurtboxes();
	RefreshTickEnabled();
}

void UMaceCollisionComponent::GatherHurtboxes()
{
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}

	TInlineComponentArray<UShapeComponent*> Shapes(Owner);

	for (UShapeComponent* Shape : Shapes)
	{
		FMaceHurtbox Hurtbox;

		if (const UMaceHurtboxCapsuleComponent* Capsule = Cast<UMaceHurtboxCapsuleComponent>(Shape))
		{
			Hurtbox.ZoneTag = Capsule->ZoneTag;
			Hurtbox.Radius = Capsule->GetUnscaledCapsuleRadius();
			Hurtbox.HalfHeight = Capsule->GetUnscaledCapsuleHalfHeight();
		}
		else if (const UMaceHurtboxBoxComponent* Box = Cast<UMaceHurtboxBoxComponent>(Shape))
		{
			Hurtbox.ZoneTag = Box->ZoneTag;
			Hurtbox.bIsBox = true;
			Hurtbox.Extent = Box->GetUnscaledBoxExtent();
		}
		else
		{
			continue;
		}

		USkeletalMeshComponent* Mesh = Cast<USkeletalMeshComponent>(Shape->GetAttachParent());
		if (!Mesh)
		{
			UE_LOG(LogMaceDamage, Warning, TEXT("[%s] hurtbox [%s] is not attached to a skeletal mesh, so it cannot follow a bone. It is ignored."),
				*GetNameSafe(Owner), *GetNameSafe(Shape));
			Shape->DestroyComponent();
			continue;
		}

		// Hurtboxes are usually authored once on a base class and inherited. A
		// child with a different skeleton keeps the bone names but not the bones,
		// and a missing bone resolves to the mesh's origin without a word, which
		// would leave every hurtbox piled up at the character's feet.
		const FName AttachName = Shape->GetAttachSocketName();
		if (!AttachName.IsNone() && !Mesh->DoesSocketExist(AttachName))
		{
			UE_LOG(LogMaceDamage, Warning, TEXT("[%s] hurtbox [%s] hangs from [%s], which [%s] does not have. It is ignored."),
				*GetNameSafe(Owner), *GetNameSafe(Shape), *AttachName.ToString(), *GetNameSafe(Mesh->GetSkeletalMeshAsset()));
			Shape->DestroyComponent();
			continue;
		}

		if (!Hurtbox.ZoneTag.IsValid())
		{
			UE_LOG(LogMaceDamage, Warning, TEXT("[%s] hurtbox [%s] has no zone tag. It can still be hit, but no rule will know where."),
				*GetNameSafe(Owner), *GetNameSafe(Shape));
		}

		Hurtbox.Mesh = Mesh;
		Hurtbox.AttachName = AttachName;
		Hurtbox.BoneName = Hurtbox.AttachName.IsNone() ? NAME_None : Mesh->GetSocketBoneName(Hurtbox.AttachName);
		Hurtbox.RelativeTransform = Shape->GetRelativeTransform();
		Hurtboxes.Add(Hurtbox);

		// From here on the hurtbox is data. A live shape would follow its bone
		// every frame for as long as the character exists, which is the cost
		// this whole approach is built to avoid.
		Shape->DestroyComponent();
	}

	if (HasHurtboxes())
	{
		UE_LOG(LogMaceDamage, Verbose, TEXT("[%s] registered %d hurtbox(es)."), *GetNameSafe(Owner), Hurtboxes.Num());
	}
}

void UMaceCollisionComponent::RefreshTickEnabled()
{
	bool bWantsTick = bWindowOpen;

#if ENABLE_DRAW_DEBUG
	bWantsTick |= HasHurtboxes() && CVarDrawHurtboxes.GetValueOnGameThread();
#endif

	SetComponentTickEnabled(bWantsTick);
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
	RefreshTickEnabled();

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
	RefreshTickEnabled();

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

bool UMaceCollisionComponent::ResolveHurtboxHit(const FVector& Start, const FVector& End, float Radius, FHitResult& InOutHit)
{
	int32 FirstTouched = INDEX_NONE;
	double FirstDistance = TNumericLimits<double>::Max();
	FVector FirstPoint = FVector::ZeroVector;

	for (int32 Index = 0; Index < Hurtboxes.Num(); ++Index)
	{
		double Distance = 0.0;
		FVector Point = FVector::ZeroVector;

		if (SweepAgainstHurtbox(Hurtboxes[Index], Start, End, Radius, Distance, Point) && Distance < FirstDistance)
		{
			FirstTouched = Index;
			FirstDistance = Distance;
			FirstPoint = Point;
		}
	}

	if (FirstTouched == INDEX_NONE)
	{
		return false;
	}

	// Rewritten from the hurtbox rather than the broad volume the sweep found,
	// so whoever reads the hit learns the bone, and the point on the body
	// rather than on an invisible capsule around it.
	const FVector SweepDirection = (End - Start).GetSafeNormal();
	const FVector SphereAtContact = Start + SweepDirection * FirstDistance;
	const FVector Normal = (SphereAtContact - FirstPoint).GetSafeNormal();

	InOutHit.BoneName = Hurtboxes[FirstTouched].BoneName;
	InOutHit.ImpactPoint = FirstPoint;
	InOutHit.Location = SphereAtContact;
	InOutHit.ImpactNormal = Normal;
	InOutHit.Normal = Normal;

	LastHitHurtbox = FirstTouched;
	LastHitTime = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0;

	return true;
}

FGameplayTag UMaceCollisionComponent::GetZoneForBone(FName BoneName) const
{
	if (BoneName.IsNone())
	{
		return FGameplayTag();
	}

	for (const FMaceHurtbox& Hurtbox : Hurtboxes)
	{
		if (Hurtbox.BoneName == BoneName)
		{
			return Hurtbox.ZoneTag;
		}
	}

	return FGameplayTag();
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

			// The sweep has found the target's broad volume. With hurtboxes, it
			// only counts once it reaches one of them: the fist is near the body
			// but has not touched it, and a later frame may still connect.
			FHitResult ResolvedHit = Hit;
			UMaceCollisionComponent* TargetCollision = FindCollisionComponent(HitActor);

			if (TargetCollision && TargetCollision->HasHurtboxes()
				&& !TargetCollision->ResolveHurtboxHit(Start, End, TraceRadius, ResolvedHit))
			{
				continue;
			}

			HitActors.Add(HitActor);

			const FGameplayTag Zone = TargetCollision ? TargetCollision->GetZoneForBone(ResolvedHit.BoneName) : FGameplayTag();
			UE_LOG(LogMaceDamage, Verbose, TEXT("[%s] hit [%s] on bone [%s], zone [%s]."),
				*GetNameSafe(GetOwner()), *GetNameSafe(HitActor), *ResolvedHit.BoneName.ToString(), *Zone.ToString());

			OnHitDetected.Broadcast(HitActor, ResolvedHit);
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

void UMaceCollisionComponent::DrawHurtboxes() const
{
#if ENABLE_DRAW_DEBUG
	const UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	const double Now = World->GetTimeSeconds();

	for (int32 Index = 0; Index < Hurtboxes.Num(); ++Index)
	{
		const FMaceHurtbox& Hurtbox = Hurtboxes[Index];
		if (!Hurtbox.Mesh.IsValid())
		{
			continue;
		}

		const FTransform WorldTransform = GetHurtboxWorldTransform(Hurtbox);
		const bool bJustHit = (Index == LastHitHurtbox) && (Now - LastHitTime < HitFlashSeconds);
		const FColor Colour = bJustHit ? FColor::Red : FColor::Green;

		if (Hurtbox.bIsBox)
		{
			DrawDebugBox(World, WorldTransform.GetLocation(), Hurtbox.Extent * WorldTransform.GetScale3D().GetAbs(),
				WorldTransform.GetRotation(), Colour, false, -1.0f, 0, 0.5f);
		}
		else
		{
			const double Scale = WorldTransform.GetMinimumAxisScale();
			DrawDebugCapsule(World, WorldTransform.GetLocation(), Hurtbox.HalfHeight * Scale, Hurtbox.Radius * Scale,
				WorldTransform.GetRotation(), Colour, false, -1.0f, 0, 0.5f);
		}
	}
#endif
}

void UMaceCollisionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bWindowOpen && TracedMesh)
	{
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

#if ENABLE_DRAW_DEBUG
	if (CVarDrawHurtboxes.GetValueOnGameThread())
	{
		DrawHurtboxes();
	}
#endif
}
