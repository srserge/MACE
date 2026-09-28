// Copyright srSerge. All Rights Reserved.

#pragma once

#include "NativeGameplayTags.h"

/**
 * Native gameplay tags used by MaceCore.
 *
 * A tag declared here is a symbol the compiler checks. A tag written as text in
 * an .ini fails silently on a typo, so the rule is: if code mentions it, it is
 * declared here. The .ini is only for tags that content alone refers to.
 *
 * Every string is prefixed with "Mace." so the tag tree stays separate from the
 * host project's own tags when the plugin is dropped into someone else's game.
 */
namespace MaceGameplayTags
{
	// Why an ability failed to activate.
	MACECORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_IsDead);
	MACECORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_Cooldown);
	MACECORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_Cost);

	// Input. Bound to InputActions through UMaceInputConfig.
	MACECORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Move);
	MACECORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Look);
	MACECORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Fire);
	MACECORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Melee);
	MACECORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Dodge);
	MACECORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Block);
	MACECORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Reload);
	MACECORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_SwapWeapon);
	MACECORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Interact);

	// Hit zones. Carried by a UPhysicalMaterialWithTags on the physics asset.
	MACECORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(HitZone_Head);
	MACECORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(HitZone_Torso);
	MACECORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(HitZone_Limb);

	// How a hit is classified. Drives the hitmarker in the sample.
	MACECORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Damage_Class_Normal);
	MACECORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Damage_Class_NonLethal);
	MACECORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Damage_Class_Lethal);

	// Stance: what the character is doing right now. Changes several times a
	// second, so it is only ever a tag - never a combat style swap. Abilities
	// sharing an input tag let GAS pick between them by stance, which is how
	// the same trigger fires a gun while aiming and pistol-whips while not.
	MACECORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Stance_Aiming);
	MACECORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Stance_Blocking);
	MACECORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Stance_Sprinting);
	MACECORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Stance_Equipping);

	// Combatant state.
	MACECORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Dead);
	MACECORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Incapacitated);
	MACECORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Invulnerable);
	MACECORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Blocking);

	// A tag rather than a call into the movement component, because MaceCore
	// cannot depend on any movement plugin. Whatever produces the pawn's movement
	// input reads it and holds the character in place.
	MACECORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_MovementLocked);

	// SetByCaller magnitudes.
	MACECORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Damage);

	// Events the core broadcasts. The sample turns them into game feel.
	MACECORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Hit_Confirmed);
	MACECORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Death);

	// Sent by a notify state on an attack montage to mark the stretch in which
	// the attack can be continued. An event rather than a loose tag because the
	// ability system exposes no way to add a loose tag from Blueprint, and the
	// notify state that sends these is Blueprint.
	MACECORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_ComboWindow_Open);
	MACECORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_ComboWindow_Close);

	// Sent by a notify state on an attack montage to mark the stretch in which
	// the attack can actually hit. Separate from the combo window because they
	// answer different questions: this one is where the blow lands, that one is
	// when the next blow can be asked for.
	MACECORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_HitWindow_Open);
	MACECORE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_HitWindow_Close);
}
