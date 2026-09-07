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

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Damage_Class_Normal, "Mace.Damage.Class.Normal", "Standard hit. Neutral hitmarker.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Damage_Class_NonLethal, "Mace.Damage.Class.NonLethal", "Incapacitating hit. Yellow hitmarker.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Damage_Class_Lethal, "Mace.Damage.Class.Lethal", "Lethal hit. Red hitmarker.");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_Dead, "Mace.State.Dead", "The combatant is dead.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_Incapacitated, "Mace.State.Incapacitated", "The combatant is down but not dead.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_Invulnerable, "Mace.State.Invulnerable", "Immune to incoming damage, such as during a dodge.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_Blocking, "Mace.State.Blocking", "Actively blocking incoming attacks.");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(SetByCaller_Damage, "Mace.SetByCaller.Damage", "Damage magnitude passed to a gameplay effect by caller.");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Event_Hit_Confirmed, "Mace.Event.Hit.Confirmed", "A hit landed on a combatant. Carries the damage class.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Event_Death, "Mace.Event.Death", "A combatant died.");
}
