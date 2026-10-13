// Copyright srSerge. All Rights Reserved.

#pragma once

#include "GameplayEffectExecutionCalculation.h"

#include "MaceDamageExecution.generated.h"

/**
 * UMaceDamageExecution
 *
 * Works out how much damage a hit really does, from two things that come from
 * opposite sides of the fight:
 *
 *     damage = the attack's base damage      x  the multiplier of the zone hit
 *              (Set By Caller, the attacker)    (UMaceDamageProfile, the target)
 *
 * The attack says how hard it hits; the body says how much that hurts where it
 * landed. Keeping the zone rules on the target is what lets the same jab hurt a
 * bare head more than a helmeted one without either attack knowing.
 *
 * The zone is read from the hit result carried in the effect context, so the
 * ability applying the effect has to put it there. Without one the hit has no
 * zone and the profile's default rule applies. A target with no profile takes
 * the base damage unchanged.
 *
 * The result is written to the Damage meta attribute, never to Health:
 * UMaceHealthSet stays the one place that turns damage into lost health.
 */
UCLASS(Meta = (DisplayName = "Mace Damage Execution"))
class MACECORE_API UMaceDamageExecution : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:

	UMaceDamageExecution();

protected:

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
