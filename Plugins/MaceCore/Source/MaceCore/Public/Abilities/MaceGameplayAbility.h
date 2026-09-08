// Copyright srSerge. All Rights Reserved.

#pragma once

#include "Abilities/GameplayAbility.h"

#include "MaceGameplayAbility.generated.h"

class ACharacter;
class AController;
class APawn;
class APlayerController;
class UAbilitySystemComponent;
class UMaceCombatStyle;
class UMaceInputBufferComponent;
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

	//~ Helpers for the Blueprint abilities that derive from this class. All of
	//~ them resolve from the ability's actor info, so none needs a target pin.

	//~ GetAbilitySystemComponentFromActorInfo is not redeclared here: UGameplayAbility
	//~ already exposes it to Blueprint and returns the same type, since Mace does
	//~ not subclass the ability system component.

	/** The pawn acting, or null if the avatar is not a pawn. */
	UFUNCTION(BlueprintPure, Category = "Mace|Ability")
	APawn* GetPawnFromActorInfo() const;

	/** The avatar as a character, or null if it is a plain pawn. */
	UFUNCTION(BlueprintPure, Category = "Mace|Ability")
	ACharacter* GetCharacterFromActorInfo() const;

	/** The controller driving the avatar, AI or player. */
	UFUNCTION(BlueprintPure, Category = "Mace|Ability")
	AController* GetControllerFromActorInfo() const;

	/** The player controller, or null when an AI is acting. */
	UFUNCTION(BlueprintPure, Category = "Mace|Ability")
	APlayerController* GetPlayerControllerFromActorInfo() const;

	/** The extension component that granted this ability's set. */
	UFUNCTION(BlueprintPure, Category = "Mace|Ability")
	UMacePawnExtensionComponent* GetPawnExtensionComponentFromActorInfo() const;

	/** The data asset describing this character. Null if none was assigned. */
	UFUNCTION(BlueprintPure, Category = "Mace|Ability")
	const UMacePawnData* GetPawnDataFromActorInfo() const;

	/** The combat style in effect right now. Derived, never stored. */
	UFUNCTION(BlueprintPure, Category = "Mace|Ability")
	const UMaceCombatStyle* GetCombatStyleFromActorInfo() const;

	/** The input buffer, for consuming a press that arrived slightly early. */
	UFUNCTION(BlueprintPure, Category = "Mace|Ability")
	UMaceInputBufferComponent* GetInputBufferComponentFromActorInfo() const;

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
