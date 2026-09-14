// Copyright srSerge. All Rights Reserved.

#include "Components/MaceAbilitySystemComponent.h"

#include "AbilitySystemGlobals.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"

#include "MaceLogChannels.h"

namespace
{
	/** Returned when a tag has never been pressed. Larger than any window. */
	constexpr float NeverPressed = 1000.0f;
}

UMaceAbilitySystemComponent::UMaceAbilitySystemComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;
}

UMaceAbilitySystemComponent* UMaceAbilitySystemComponent::FindAbilitySystemComponent(const AActor* Actor)
{
	// Through globals rather than a direct component search: this finds the
	// component whether it lives on the pawn or on the player state. The cast
	// fails cleanly on a project that kept the engine's plain component.
	return Cast<UMaceAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor));
}

void UMaceAbilitySystemComponent::RecordInput(FGameplayTag InputTag)
{
	const UWorld* World = GetWorld();
	if (!World || !InputTag.IsValid())
	{
		return;
	}

	LastPressTimes.Add(InputTag, World->GetTimeSeconds());

	UE_LOG(LogMace, VeryVerbose, TEXT("Buffered input [%s] on [%s]."),
		*InputTag.ToString(), *GetNameSafe(GetOwner()));
}

bool UMaceAbilitySystemComponent::ConsumeBufferedInput(FGameplayTag InputTag, float WindowSeconds)
{
	if (!HasBufferedInput(InputTag, WindowSeconds))
	{
		return false;
	}

	// Removed rather than zeroed: a consumed press must not satisfy a second
	// consumer in the same frame, which is how one button press turns into two
	// attacks.
	LastPressTimes.Remove(InputTag);

	return true;
}

bool UMaceAbilitySystemComponent::HasBufferedInput(FGameplayTag InputTag, float WindowSeconds) const
{
	return GetTimeSinceInput(InputTag) <= WindowSeconds;
}

float UMaceAbilitySystemComponent::GetTimeSinceInput(FGameplayTag InputTag) const
{
	const UWorld* World = GetWorld();
	const double* LastPress = LastPressTimes.Find(InputTag);

	if (!World || !LastPress)
	{
		return NeverPressed;
	}

	return static_cast<float>(World->GetTimeSeconds() - *LastPress);
}

void UMaceAbilitySystemComponent::ClearBufferedInput(FGameplayTag InputTag)
{
	LastPressTimes.Remove(InputTag);
}

void UMaceAbilitySystemComponent::ClearAllBufferedInput()
{
	LastPressTimes.Reset();
}

void UMaceAbilitySystemComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// A press left in the map would otherwise outlive the pawn in an editor
	// session, since the component object can be reused.
	ClearAllBufferedInput();

	Super::EndPlay(EndPlayReason);
}
