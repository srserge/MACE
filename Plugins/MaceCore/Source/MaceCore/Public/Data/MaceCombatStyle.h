// Copyright srSerge. All Rights Reserved.

#pragma once

#include "Engine/DataAsset.h"

#include "MaceCombatStyle.generated.h"

class UAnimInstance;
class UMaceAbilitySet;
class UMaceComboSet;

/**
 * UMaceCombatStyle
 *
 * How a character fights right now: what it can do, and how it stands.
 *
 * Exactly one style is active at a time. It is not stored anywhere as truth:
 * it is derived from what the character is holding, falling back to the base
 * style on the pawn data when the hands are empty.
 *
 *     ActiveStyle = ActiveWeapon ? ActiveWeapon->CombatStyle
 *                                : PawnData->BaseCombatStyle;
 *
 * Switching style revokes the previous style's ability sets and grants the new
 * ones, which is why FMaceAbilitySet_GrantedHandles records what it handed out.
 *
 * A style is not a stance. Aiming, blocking and sprinting are gameplay tags on
 * the ability system, not styles: they change several times a second and must
 * not cause abilities to be granted and revoked. Two abilities can share an
 * input tag and let GAS pick between them by stance.
 */
UCLASS(BlueprintType, Const, Meta = (DisplayName = "Mace Combat Style"))
class MACECORE_API UMaceCombatStyle : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UMaceCombatStyle(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** Everything the character can do while this style is active. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mace|Abilities")
	TArray<TObjectPtr<UMaceAbilitySet>> AbilitySets;

	/**
	 * The sequence of blows this style throws when nothing else is chosen.
	 *
	 * The entry point to the whole melee chain: the style is derived from the
	 * equipped weapon, so changing weapon changes the combos without the
	 * ability knowing. It is also the fallback for when the selection table
	 * matches no row, which is what lets the system work before the table
	 * exists at all.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mace|Combat")
	TObjectPtr<UMaceComboSet> DefaultComboSet;

	/**
	 * Animation layer for this style: an unarmed posture, a pistol aim pose.
	 *
	 * Soft on purpose. A soft class pointer is a path, not a pointer, so it
	 * creates no module dependency and MaceCore stays independent of
	 * MaceAnimation, which is what actually links the layer onto the mesh.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mace|Animation")
	TSoftClassPtr<UAnimInstance> AnimLayerClass;
};
