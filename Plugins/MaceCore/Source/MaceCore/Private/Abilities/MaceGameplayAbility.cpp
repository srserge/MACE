// Copyright srSerge. All Rights Reserved.

#include "Abilities/MaceGameplayAbility.h"

#include "AbilitySystemComponent.h"
#include "GameFramework/Pawn.h"

#include "Components/MacePawnExtensionComponent.h"
#include "Data/MacePawnData.h"
#include "MaceGameplayTags.h"

UMaceGameplayAbility::UMaceGameplayAbility(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Instanced per actor by default: an ability that holds combo state or a
	// montage handle cannot be shared between activations.
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	// Abilities run where the player pressed the button and are corrected by
	// the server, rather than waiting for a round trip.
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

APawn* UMaceGameplayAbility::GetMacePawn() const
{
	return CurrentActorInfo ? Cast<APawn>(CurrentActorInfo->AvatarActor.Get()) : nullptr;
}

UMacePawnExtensionComponent* UMaceGameplayAbility::GetPawnExtensionComponent() const
{
	return CurrentActorInfo ? UMacePawnExtensionComponent::FindPawnExtensionComponent(CurrentActorInfo->AvatarActor.Get()) : nullptr;
}

const UMacePawnData* UMaceGameplayAbility::GetPawnData() const
{
	const UMacePawnExtensionComponent* Extension = GetPawnExtensionComponent();

	return Extension ? Extension->GetPawnData() : nullptr;
}

bool UMaceGameplayAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	if (!ActorInfo || !ActorInfo->AbilitySystemComponent.IsValid())
	{
		return false;
	}

	// Nothing activates once the combatant is dead. Checked here rather than in
	// every ability's Blueprint, which is exactly what this base class is for.
	if (ActorInfo->AbilitySystemComponent->HasMatchingGameplayTag(MaceGameplayTags::State_Dead))
	{
		if (OptionalRelevantTags)
		{
			OptionalRelevantTags->AddTag(MaceGameplayTags::Ability_ActivateFail_IsDead);
		}

		return false;
	}

	return true;
}
