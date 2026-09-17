// Copyright srSerge. All Rights Reserved.

#pragma once

#include "GameplayTagContainer.h"

#include "MaceAttackContext.generated.h"

/**
 * FMaceAttackContext
 *
 * What is known about the fight at the instant a blow has to be chosen. The
 * ability fills it, the selection table reads it.
 *
 * It lives in the core although the table that consumes it lives in the sample,
 * and that is deliberate: Chooser is forbidden in MaceCore, so the dependency
 * has to run from the sample towards the core and never back. The core declares
 * the vocabulary; the sample decides with it.
 *
 * Declared in C++ rather than as a Blueprint structure asset because a
 * Blueprint struct loses the data of every instance when a field is added,
 * removed or reordered, and this shape is going to grow.
 */
USTRUCT(BlueprintType)
struct MACECORE_API FMaceAttackContext
{
	GENERATED_BODY()

	/** The input that arrived. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mace", Meta = (Categories = "Mace.InputTag"))
	FGameplayTag InputTag;

	/** The blow just thrown, invalid on the first of a chain. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mace", Meta = (Categories = "Mace.Attack"))
	FGameplayTag PreviousAttackTag;

	/** How many blows deep the chain is, counting from zero. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mace")
	int32 ChainIndex = 0;

	/** Centimetres to the target, or negative when there is no target. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mace")
	float DistanceToTarget = -1.0f;

	/** Standing still, walking or running. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mace")
	FGameplayTag MovementTag;

	/** On its feet, blocking, staggered or downed. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mace")
	FGameplayTag TargetStateTag;
};
