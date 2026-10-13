// Copyright srSerge. All Rights Reserved.

#include "MaceGameplayTags.h"

namespace MaceGameplayTags
{
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_ActivateFail_IsDead, "Mace.Ability.ActivateFail.IsDead", "Ability failed to activate because its owner is dead.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_ActivateFail_Cooldown, "Mace.Ability.ActivateFail.Cooldown", "Ability failed to activate because it is on cooldown.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_ActivateFail_Cost, "Mace.Ability.ActivateFail.Cost", "Ability failed to activate because its cost could not be paid.");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_Move, "Mace.InputTag.Move", "Movement input.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_Look, "Mace.InputTag.Look", "Camera look input.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_Fire, "Mace.InputTag.Fire", "Fire the equipped ranged weapon.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_Melee, "Mace.InputTag.Melee", "Melee attack.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_Dodge, "Mace.InputTag.Dodge", "Dodge or evade.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_Block, "Mace.InputTag.Block", "Block incoming attacks.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_Reload, "Mace.InputTag.Reload", "Reload the equipped weapon.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_SwapWeapon, "Mace.InputTag.SwapWeapon", "Cycle to another equipment slot.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_Interact, "Mace.InputTag.Interact", "Interact with the world, such as picking up a weapon.");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(HitZone_Head, "Mace.HitZone.Head", "Critical zone. Usually lethal.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(HitZone_Torso, "Mace.HitZone.Torso", "Default body zone.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(HitZone_Limb, "Mace.HitZone.Limb", "Marked zone. Usually incapacitating rather than lethal.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(HitZone_Limb_Arm_Left, "Mace.HitZone.Limb.Arm.Left", "Left arm.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(HitZone_Limb_Arm_Right, "Mace.HitZone.Limb.Arm.Right", "Right arm.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(HitZone_Limb_Leg_Left, "Mace.HitZone.Limb.Leg.Left", "Left leg.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(HitZone_Limb_Leg_Right, "Mace.HitZone.Limb.Leg.Right", "Right leg.");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Damage_Class_Normal, "Mace.Damage.Class.Normal", "Standard hit. Neutral hitmarker.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Damage_Class_NonLethal, "Mace.Damage.Class.NonLethal", "Incapacitating hit. Yellow hitmarker.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Damage_Class_Lethal, "Mace.Damage.Class.Lethal", "Lethal hit. Red hitmarker.");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Stance_Aiming, "Mace.Stance.Aiming", "Aiming down sights. Routes the fire input to the ranged attack.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Stance_Blocking, "Mace.Stance.Blocking", "Guarding against incoming attacks.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Stance_Sprinting, "Mace.Stance.Sprinting", "Sprinting. Blocks most combat abilities.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Stance_Equipping, "Mace.Stance.Equipping", "Drawing or holstering. Blocks attacks until the weapon is in hand.");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_Dead, "Mace.State.Dead", "The combatant is dead.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_Incapacitated, "Mace.State.Incapacitated", "The combatant is down but not dead.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_Invulnerable, "Mace.State.Invulnerable", "Immune to incoming damage, such as during a dodge.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_Blocking, "Mace.State.Blocking", "Actively blocking incoming attacks.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_MovementLocked, "Mace.State.MovementLocked", "Movement input is ignored, such as during a melee attack without root motion.");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(SetByCaller_Damage, "Mace.SetByCaller.Damage", "Damage magnitude passed to a gameplay effect by caller.");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Event_Hit_Confirmed, "Mace.Event.Hit.Confirmed", "A hit landed on a combatant. Carries the damage class.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Event_Death, "Mace.Event.Death", "A combatant died.");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Event_ComboWindow_Open, "Mace.Event.ComboWindow.Open", "The attack can now be continued into the next link of the combo.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Event_ComboWindow_Close, "Mace.Event.ComboWindow.Close", "The chance to continue the combo has passed.");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Event_HitWindow_Open, "Mace.Event.HitWindow.Open", "The attack can now connect. Collision detection starts here.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Event_HitWindow_Close, "Mace.Event.HitWindow.Close", "The attack can no longer connect. Collision detection stops here.");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(UI_Widget_HealthBar, "Mace.UI.Widget.HealthBar", "Shows how much health a character has left.");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attack, "Mace.Attack", "Identifies one blow. Each project adds its attacks beneath it.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Combo, "Mace.Combo", "Identifies one sequence of blows. Each project adds its combos beneath it.");
}
