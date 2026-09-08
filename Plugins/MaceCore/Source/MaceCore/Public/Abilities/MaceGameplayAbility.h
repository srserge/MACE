// Copyright srSerge. All Rights Reserved.

#pragma once

#include "Abilities/GameplayAbility.h"

#include "MaceGameplayAbility.generated.h"

class APawn;
class UMacePawnData;
class UMacePawnExtensionComponent;

/** When an ability tries to activate itself. */
UENUM(BlueprintType)
enum class EMaceAbilityActivationPolicy : uint8
{
	/** Once per input press. Attacks, dodges, reloads. */
	OnInputTriggered,

	/** Repeatedly while the input is held. Automatic fire, blocking. */
	WhileInputActive,

	/** As soon as the ability is granted. Passives. */
	OnSpawn
};

/**
 * UMaceGameplayAbility
 *
 * Base class for every gameplay ability in Mace. Abilities themselves are
 * authored in Blueprint; this class exists so that anything shared by all of
 * them lives in one place instead of forty graphs.
 *
 * Adding a property here gives it to every ability in the project at once.
 * That is the point of it: it is the seam where project-wide ability behaviour
 * is changed without touching content.
 */
UCLASS(Abstract, Meta = (ShortTooltip = "Base class for Mace gameplay abilities."))
class MACECORE_API UMaceGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:

	UMaceGameplayAbility(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** The pawn running this ability, or null if the avatar is not a pawn. */
	UFUNCTION(BlueprintPure, Category = "Mace|Ability")
	APawn* GetMacePawn() const;

	/** Shortcut to the extension component that granted this ability's set. */
	UFUNCTION(BlueprintPure, Category = "Mace|Ability")
	UMacePawnExtensionComponent* GetPawnExtensionComponent() const;

	/** The data asset describing this character. Null if none was assigned. */
	UFUNCTION(BlueprintPure, Category = "Mace|Ability")
	const UMacePawnData* GetPawnData() const;

	EMaceAbilityActivationPolicy GetActivationPolicy() const { return ActivationPolicy; }

protected:

	/**
	 * Adds the project-wide activation rules on top of the ones GAS already
	 * checks. Failure reasons are reported as Mace.Ability.ActivateFail.* tags
	 * so the UI can say why an input did nothing.
	 */
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mace|Activation")
	EMaceAbilityActivationPolicy ActivationPolicy = EMaceAbilityActivationPolicy::OnInputTriggered;
};
