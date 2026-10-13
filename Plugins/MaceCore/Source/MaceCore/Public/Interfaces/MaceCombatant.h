// Copyright srSerge. All Rights Reserved.

#pragma once

#include "UObject/Interface.h"

#include "MaceCombatant.generated.h"

UINTERFACE(MinimalAPI, BlueprintType)
class UMaceCombatant : public UInterface
{
	GENERATED_BODY()
};

/**
 * IMaceCombatant
 *
 * The minimum the combat system needs to know about an actor. Any pawn can
 * implement it: there is no mandatory base class, which is what lets MaceCore
 * drop into a project that already has its own character hierarchy.
 */
class MACECORE_API IMaceCombatant
{
	GENERATED_BODY()

public:

	/** False once the combatant is dead or otherwise out of the fight. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Mace|Combat")
	bool IsAlive() const;

	/**
	 * The actor that receives animation and effects. Usually the pawn itself,
	 * but it can be another actor such as a vehicle or a mount.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Mace|Combat")
	AActor* GetCombatAvatar() const;
};
