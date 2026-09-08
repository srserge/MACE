// Copyright srSerge. All Rights Reserved.

#pragma once

#include "GameplayTagContainer.h"

#include "MaceComboLink.generated.h"

class UAnimMontage;

/**
 * Where a combo link can go next, and what input takes it there.
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

	/** The LinkId this input leads to. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mace", Meta = (Categories = "Mace.Combo"))
	FGameplayTag NextLinkId;
};

/**
 * FMaceComboLink
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
struct FMaceComboLink
{
	GENERATED_BODY()

	/** Identifies this link so other links can point at it. Mace.Combo.Light1 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mace", Meta = (Categories = "Mace.Combo"))
	FGameplayTag LinkId;

	/** The attack animation. Carries the combo and hit window notify states. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mace")
	TObjectPtr<UAnimMontage> Montage;

	/** Scales the base damage of the attack. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mace", Meta = (ClampMin = "0.0"))
	float DamageMultiplier = 1.0f;

	/** How a hit from this link is classified, before the zone rule adjusts it. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mace", Meta = (Categories = "Mace.Damage.Class"))
	FGameplayTag DamageClass;

	/** Where the chain can go from here. Empty means this link ends the combo. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mace", Meta = (TitleProperty = "InputTag"))
	TArray<FMaceComboTransition> Transitions;

	/** The LinkId this input leads to, or an invalid tag if it does not. */
	FGameplayTag FindNextLink(const FGameplayTag& InputTag) const
	{
		for (const FMaceComboTransition& Transition : Transitions)
		{
			if (Transition.InputTag == InputTag)
			{
				return Transition.NextLinkId;
			}
		}

		return FGameplayTag();
	}
};
