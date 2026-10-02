// Copyright srSerge. All Rights Reserved.

#pragma once

#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"

#include "MaceDamageProfile.generated.h"

class UGameplayEffect;

/**
 * What happens when a specific body zone is hit.
 *
 * Lethality is a property of the zone, not of the ability, so the same profile
 * serves a punch and a bullet: the system never knows which one arrived.
 */
USTRUCT(BlueprintType)
struct FMaceDamageZoneRule
{
	GENERATED_BODY()

	/**
	 * Which zone. Matched hierarchically, so a rule for Mace.HitZone.Limb also
	 * covers a hit on Mace.HitZone.Limb.Arm.Left.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mace", Meta = (Categories = "Mace.HitZone"))
	FGameplayTag ZoneTag;

	/** Multiplier applied to the incoming base damage. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mace", Meta = (ClampMin = "0.0"))
	float DamageMultiplier = 1.0f;

	/**
	 * How the hit is classified. A tag rather than an enum, so a template can
	 * add a class of its own without recompiling the core.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mace", Meta = (Categories = "Mace.Damage.Class"))
	FGameplayTag DamageClass;

	/** Status effects applied on hit. Stun and incapacitation go here. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mace")
	TArray<TSubclassOf<UGameplayEffect>> StatusEffects;

	/**
	 * Lowest health this zone can leave the target at.
	 * 0 means the zone can kill. 1 means it never does.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mace", Meta = (ClampMin = "0.0"))
	float MinHealthFloor = 0.0f;
};

/**
 * UMaceDamageProfile
 *
 * The full set of zone rules for a character. A designer tags a hurtbox on the
 * character with a zone and adds a row here; no code changes.
 */
UCLASS(BlueprintType, Const, Meta = (DisplayName = "Mace Damage Profile"))
class MACECORE_API UMaceDamageProfile : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UMaceDamageProfile(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/**
	 * Returns the rule matching the first zone tag found in ZoneTags.
	 * Returns nullptr when no rule matches, in which case the caller should
	 * apply damage unmodified.
	 */
	const FMaceDamageZoneRule* FindRuleForZone(const FGameplayTagContainer& ZoneTags) const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mace|Damage", Meta = (TitleProperty = "ZoneTag"))
	TArray<FMaceDamageZoneRule> ZoneRules;

	/** Used when an impact carries no zone tag at all. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mace|Damage")
	FMaceDamageZoneRule DefaultRule;
};
