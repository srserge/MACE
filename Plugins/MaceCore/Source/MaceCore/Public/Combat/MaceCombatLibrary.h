// Copyright srSerge. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"

#include "MaceCombatLibrary.generated.h"

/** Which side of a combatant a hit came from, relative to where it is facing. */
UENUM(BlueprintType)
enum class EMaceHitDirection : uint8
{
	Front,
	Back,
	Left,
	Right
};

/**
 * UMaceCombatLibrary
 *
 * Combat questions any Blueprint can ask without owning an ability. These
 * functions read the world and return answers; they never act on them. Which
 * reaction to play, or whether to play one at all, belongs to whoever calls.
 *
 * That line is what keeps this in the core. Playing a reaction knows about
 * montages, and what knows about montages lives outside MaceCore.
 *
 * Directions are an enum rather than tags on purpose. Impact classes are tags
 * so a template can add one without recompiling; the four directions here are
 * defined by the arithmetic below, so adding a fifth means changing this code
 * whatever type it returns.
 */
UCLASS()
class MACECORE_API UMaceCombatLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	/**
	 * Which side of Target a hit came from, judged on the ground plane against
	 * the way Target is facing.
	 *
	 * HitOrigin is where the blow came from. The attacker's location is the
	 * robust choice: it answers "where was I attacked from".
	 *
	 * OutAngle is the signed angle in degrees, from -180 to 180, positive to the
	 * right. The four directions are its quadrants; use the angle itself for a
	 * finer split or a blend.
	 *
	 * A null target, or an origin standing exactly on it, reads as Front.
	 */
	UFUNCTION(BlueprintPure, Category = "Mace|Combat")
	static EMaceHitDirection GetHitDirection(const AActor* Target, FVector HitOrigin, float& OutAngle);
};
