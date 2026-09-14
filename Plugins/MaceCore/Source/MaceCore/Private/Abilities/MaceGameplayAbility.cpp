// Copyright srSerge. All Rights Reserved.

#include "Abilities/MaceGameplayAbility.h"

#include "AbilitySystemComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/Controller.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"

#include "Components/MaceAbilitySystemComponent.h"
#include "Components/MacePawnExtensionComponent.h"
#include "Data/MaceCombatStyle.h"
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

APawn* UMaceGameplayAbility::GetPawnFromActorInfo() const
{
	return CurrentActorInfo ? Cast<APawn>(CurrentActorInfo->AvatarActor.Get()) : nullptr;
}

ACharacter* UMaceGameplayAbility::GetCharacterFromActorInfo() const
{
	return CurrentActorInfo ? Cast<ACharacter>(CurrentActorInfo->AvatarActor.Get()) : nullptr;
}

AController* UMaceGameplayAbility::GetControllerFromActorInfo() const
{
	if (!CurrentActorInfo)
	{
		return nullptr;
	}

	if (AController* Controller = CurrentActorInfo->PlayerController.Get())
	{
		return Controller;
	}

	// The ability system's owner is not always the thing holding the
	// controller: walk up from the avatar, and keep walking when a pawn is
	// possessed through an intermediate actor.
	AActor* Owner = CurrentActorInfo->OwnerActor.Get();
	while (Owner)
	{
		if (AController* Controller = Cast<AController>(Owner))
		{
			return Controller;
		}

		if (const APawn* Pawn = Cast<APawn>(Owner))
		{
			return Pawn->GetController();
		}

		Owner = Owner->GetOwner();
	}

	return nullptr;
}

APlayerController* UMaceGameplayAbility::GetPlayerControllerFromActorInfo() const
{
	return CurrentActorInfo ? CurrentActorInfo->PlayerController.Get() : nullptr;
}

UMacePawnExtensionComponent* UMaceGameplayAbility::GetPawnExtensionComponentFromActorInfo() const
{
	return CurrentActorInfo ? UMacePawnExtensionComponent::FindPawnExtensionComponent(CurrentActorInfo->AvatarActor.Get()) : nullptr;
}

const UMacePawnData* UMaceGameplayAbility::GetPawnDataFromActorInfo() const
{
	const UMacePawnExtensionComponent* Extension = GetPawnExtensionComponentFromActorInfo();

	return Extension ? Extension->GetPawnData() : nullptr;
}

const UMaceCombatStyle* UMaceGameplayAbility::GetCombatStyleFromActorInfo() const
{
	const UMacePawnExtensionComponent* Extension = GetPawnExtensionComponentFromActorInfo();

	return Extension ? Extension->GetActiveCombatStyle() : nullptr;
}

UMaceAbilitySystemComponent* UMaceGameplayAbility::GetMaceAbilitySystemComponentFromActorInfo() const
{
	return CurrentActorInfo ? UMaceAbilitySystemComponent::FindAbilitySystemComponent(CurrentActorInfo->AvatarActor.Get()) : nullptr;
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
