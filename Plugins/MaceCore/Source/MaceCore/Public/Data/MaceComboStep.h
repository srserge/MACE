// Copyright srSerge. All Rights Reserved.

#pragma once

#include "GameplayTagContainer.h"

#include "MaceComboStep.generated.h"

class UAnimMontage;

/**
 * A pool of interchangeable montages for one step of a combo, split by hand.
 *
 * The step is fixed; the animation that fills it is not. That separation is the
 * whole point. An array of montages indexed by chain position makes one index
 * mean two things at once - where the player is in the combo, and what is
 * playing - so the only way to add variety to it is to make the combo longer.
 * Here the position stays fixed and the variety lives inside it.
 *
 * Split by hand rather than pooled together because alternation has to be
 * deterministic, not random. If the previous step threw a left, this one draws
 * from Right. Two left jabs in a row read wrong even when each animation is
 * correct on its own, and that is most of what separates a chain that looks
 * authored from one that looks sampled.
 *
 * Every montage in the same array must share a timing envelope: the same
 * wind-up, active window and recovery. The variety is meant to be visual only.
 * The moment two candidates take different lengths the player cannot learn the
 * rhythm, and an enemy reacting to the wind-up cannot behave consistently
 * either, because its reaction delay is measured against a length that keeps
 * changing.
 *
 * Left unprefixed deliberately: this is a small helper of one system rather
 * than part of the framework vocabulary, which is the same line Lyra draws
 * when it writes FInteractionOption instead of FLyraInteractionOption.
 */
USTRUCT(BlueprintType)
struct FHandedMontageSet
{
	GENERATED_BODY()

	/** Candidates thrown with the left hand or leg. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mace")
	TArray<TObjectPtr<UAnimMontage>> Left;

	/** Candidates thrown with the right hand or leg. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mace")
	TArray<TObjectPtr<UAnimMontage>> Right;
};

/**
 * Where a combo step can go next, and what input takes it there.
 *
 * A list of these rather than named NextOnLight / NextOnHeavy fields, so that
 * adding a third kind of attack later is a new row instead of a new field on
 * the struct. Changing the struct is what this whole type exists to avoid.
 */
USTRUCT(BlueprintType)
struct FMaceComboTransition
{
	GENERATED_BODY()

	/** Which input continues the chain from here. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mace", Meta = (Categories = "Mace.InputTag"))
	FGameplayTag InputTag;

	/** The StepId this input leads to. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mace", Meta = (Categories = "Mace.Combo"))
	FGameplayTag NextStepId;
};

/**
 * FMaceComboStep
 *
 * One attack in a combo chain: the animation it plays, what it does on hit,
 * and where the chain can go from it.
 *
 * Defined in C++ rather than as a Blueprint structure asset for one reason:
 * Blueprint structs lose the data of every instance when a field is added,
 * removed or reordered, and this shape will grow. Defining the type here costs
 * nothing to the rest of the project - it adds no property to any ability
 * class. The array of links lives as a variable on the Blueprint ability that
 * uses it, so an ability with no combo never sees this type.
 *
 * The chain is a graph, not a sequence: each link names its successors by tag,
 * which is what lets light-light-heavy and light-heavy branch from the same
 * opening move.
 */
USTRUCT(BlueprintType)
struct FMaceComboStep
{
	GENERATED_BODY()

	/** Identifies this step so other steps can point at it. Mace.Combo.Light.1 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mace", Meta = (Categories = "Mace.Combo"))
	FGameplayTag StepId;

	/** The attack animation. Carries the combo and hit window notify states. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mace")
	TObjectPtr<UAnimMontage> Montage;

	/** Scales the base damage of the attack. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mace", Meta = (ClampMin = "0.0"))
	float DamageMultiplier = 1.0f;

	/** How a hit from this link is classified, before the zone rule adjusts it. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mace", Meta = (Categories = "Mace.Damage.Class"))
	FGameplayTag DamageClass;

	/** Where the chain can go from here. Empty means this step ends the combo. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mace", Meta = (TitleProperty = "InputTag"))
	TArray<FMaceComboTransition> Transitions;

	/** The StepId this input leads to, or an invalid tag if it does not. */
	FGameplayTag FindNextStep(const FGameplayTag& InputTag) const
	{
		for (const FMaceComboTransition& Transition : Transitions)
		{
			if (Transition.InputTag == InputTag)
			{
				return Transition.NextStepId;
			}
		}

		return FGameplayTag();
	}
};
