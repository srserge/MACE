// Copyright srSerge. All Rights Reserved.

#pragma once

#include "Engine/DataAsset.h"

#include "MacePawnData.generated.h"

class APawn;
class UAnimInstance;
class UMaceAbilitySet;
class UMaceDamageProfile;

/**
 * UMacePawnData
 *
 * Everything needed to build a combatant, as data. One asset describes a whole
 * character: swapping character is swapping a pointer.
 *
 * Shared by players and enemies. UMacePlayerData extends it with the pieces
 * only a player-controlled pawn needs.
 */
UCLASS(BlueprintType, Const, Meta = (DisplayName = "Mace Pawn Data"))
class MACECORE_API UMacePawnData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UMacePawnData(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** Pawn class to spawn for this character. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mace|Pawn")
	TSubclassOf<APawn> PawnClass;

	/** What this character knows how to do, and which attributes it owns. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mace|Abilities")
	TArray<TObjectPtr<UMaceAbilitySet>> AbilitySets;

	/**
	 * Animation blueprint driving this character.
	 *
	 * Deliberately a soft reference: a soft pointer is a path, not a pointer, so
	 * it creates no module dependency and MaceCore stays independent of
	 * MaceAnimation. It is also the mechanism async loading needs.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mace|Animation")
	TSoftClassPtr<UAnimInstance> AnimInstanceClass;

	/** How this character reacts to hits, per body zone. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mace|Damage")
	TObjectPtr<UMaceDamageProfile> DamageProfile;
};
