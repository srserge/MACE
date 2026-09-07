// Copyright srSerge. All Rights Reserved.

#pragma once

#include "ActiveGameplayEffectHandle.h"
#include "AttributeSet.h"
#include "Engine/DataAsset.h"
#include "GameplayAbilitySpecHandle.h"
#include "GameplayTagContainer.h"

#include "MaceAbilitySet.generated.h"

class UAbilitySystemComponent;
class UGameplayAbility;
class UGameplayEffect;

/** A gameplay ability granted by an ability set, and the input that triggers it. */
USTRUCT(BlueprintType)
struct FMaceAbilitySet_GameplayAbility
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "Mace")
	TSubclassOf<UGameplayAbility> Ability = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Mace")
	int32 AbilityLevel = 1;

	/** Input tag that activates this ability. Leave empty for passive abilities. */
	UPROPERTY(EditDefaultsOnly, Category = "Mace", Meta = (Categories = "Mace.InputTag"))
	FGameplayTag InputTag;
};

/** A gameplay effect applied when the set is granted. Attribute initialisation goes here. */
USTRUCT(BlueprintType)
struct FMaceAbilitySet_GameplayEffect
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "Mace")
	TSubclassOf<UGameplayEffect> GameplayEffect = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Mace")
	float EffectLevel = 1.0f;
};

/** An attribute set instantiated on the ability system component when the set is granted. */
USTRUCT(BlueprintType)
struct FMaceAbilitySet_AttributeSet
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "Mace")
	TSubclassOf<UAttributeSet> AttributeSet = nullptr;
};

/**
 * Handles for everything an ability set granted, so all of it can be taken back.
 *
 * Revocation is not optional: equipment grants ability sets, so dropping a rifle
 * has to remove the abilities that rifle gave. Without these handles those
 * abilities stay on the character forever.
 */
USTRUCT(BlueprintType)
struct MACECORE_API FMaceAbilitySet_GrantedHandles
{
	GENERATED_BODY()

	void AddAbilitySpecHandle(const FGameplayAbilitySpecHandle& Handle);
	void AddGameplayEffectHandle(const FActiveGameplayEffectHandle& Handle);
	void AddAttributeSet(UAttributeSet* Set);

	/** Removes every ability, effect and attribute set this handle recorded. */
	void TakeFromAbilitySystem(UAbilitySystemComponent* ASC);

protected:

	UPROPERTY()
	TArray<FGameplayAbilitySpecHandle> AbilitySpecHandles;

	UPROPERTY()
	TArray<FActiveGameplayEffectHandle> GameplayEffectHandles;

	UPROPERTY()
	TArray<TObjectPtr<UAttributeSet>> GrantedAttributeSets;
};

/**
 * UMaceAbilitySet
 *
 * A bundle of abilities, effects and attribute sets granted together. This is
 * what a pawn data asset points at: it is how a designer says "this character
 * knows how to do these things and has these attributes", with no code.
 */
UCLASS(BlueprintType, Const, Meta = (DisplayName = "Mace Ability Set"))
class MACECORE_API UMaceAbilitySet : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UMaceAbilitySet(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/**
	 * Grants everything in this set to the ability system.
	 * Server only. Fills OutGrantedHandles so the grant can be undone later.
	 */
	void GiveToAbilitySystem(UAbilitySystemComponent* ASC, FMaceAbilitySet_GrantedHandles* OutGrantedHandles, UObject* SourceObject = nullptr) const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gameplay Abilities", Meta = (TitleProperty = Ability))
	TArray<FMaceAbilitySet_GameplayAbility> GrantedGameplayAbilities;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gameplay Effects", Meta = (TitleProperty = GameplayEffect))
	TArray<FMaceAbilitySet_GameplayEffect> GrantedGameplayEffects;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attribute Sets", Meta = (TitleProperty = AttributeSet))
	TArray<FMaceAbilitySet_AttributeSet> GrantedAttributes;
};
