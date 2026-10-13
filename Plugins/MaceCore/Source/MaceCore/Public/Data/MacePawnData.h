// Copyright srSerge. All Rights Reserved.

#pragma once

#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"

#include "MacePawnData.generated.h"

class APawn;
class UAnimInstance;
class UMaceAbilitySet;
class UMaceCombatStyle;
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

	/**
	 * What this character knows how to do regardless of what it is holding,
	 * and which attributes it owns. Health, dodging, interacting.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mace|Abilities")
	TArray<TObjectPtr<UMaceAbilitySet>> AbilitySets;

	/**
	 * How this character fights with empty hands.
	 *
	 * A permanent trait, not a slot that gets overwritten: a boxer has an
	 * unarmed style, a character who cannot fight bare-handed has none. The
	 * style actually in effect is derived at runtime from the equipped weapon,
	 * falling back to this one, so nothing ever writes here.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mace|Abilities")
	TObjectPtr<UMaceCombatStyle> BaseCombatStyle;

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

	/**
	 * Widgets this character offers, keyed by the role each one plays.
	 *
	 * The tag names the role, such as Mace.UI.Widget.HealthBar, and the value is
	 * the widget that plays it for this character. Leaving a role out is how a
	 * character goes without it: a street pedestrian has no health bar, a knight
	 * does, and a boss can point the same role at a widget of its own.
	 *
	 * Nothing here creates a widget. Whoever owns the role - the widget component
	 * above an enemy, the HUD of a player - looks its tag up and builds the widget
	 * itself, which is what keeps UMG out of the core.
	 *
	 * Typed as UObject with a MetaClass filter rather than TSoftClassPtr<UUserWidget>
	 * for the same reason: a widget-only picker in the editor without linking UMG,
	 * which a dedicated server has no use for.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mace|UI", Meta = (Categories = "Mace.UI.Widget", MetaClass = "/Script/UMG.UserWidget", ForceInlineRow))
	TMap<FGameplayTag, TSoftClassPtr<UObject>> Widgets;
};
