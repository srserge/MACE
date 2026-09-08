// Copyright srSerge. All Rights Reserved.

#include "Components/MaceInputBufferComponent.h"

#include "Engine/World.h"
#include "GameFramework/Actor.h"

#include "MaceLogChannels.h"

namespace
{
	/** Returned when a tag has never been pressed. Larger than any window. */
	constexpr float NeverPressed = 1000.0f;
}

UMaceInputBufferComponent::UMaceInputBufferComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(false);
}

UMaceInputBufferComponent* UMaceInputBufferComponent::FindInputBufferComponent(const AActor* Actor)
{
	return Actor ? Actor->FindComponentByClass<UMaceInputBufferComponent>() : nullptr;
}

void UMaceInputBufferComponent::RecordInput(FGameplayTag InputTag)
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

bool UMaceInputBufferComponent::ConsumeBufferedInput(FGameplayTag InputTag, float WindowSeconds)
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

bool UMaceInputBufferComponent::HasBufferedInput(FGameplayTag InputTag, float WindowSeconds) const
{
	return GetTimeSinceInput(InputTag) <= WindowSeconds;
}

float UMaceInputBufferComponent::GetTimeSinceInput(FGameplayTag InputTag) const
{
	const UWorld* World = GetWorld();
	const double* LastPress = LastPressTimes.Find(InputTag);

	if (!World || !LastPress)
	{
		return NeverPressed;
	}

	return static_cast<float>(World->GetTimeSeconds() - *LastPress);
}

void UMaceInputBufferComponent::ClearBufferedInput(FGameplayTag InputTag)
{
	LastPressTimes.Remove(InputTag);
}

void UMaceInputBufferComponent::ClearAll()
{
	LastPressTimes.Reset();
}
