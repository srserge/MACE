// Copyright srSerge. All Rights Reserved.

#pragma once

#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"

#include "MaceComboSet.generated.h"

class UMaceAttackData;

/**
 * UMaceComboSet
 *
 * An ordered run of blows: jab, hook, straight. What a combo is, readable at a
 * glance, and duplicated to make a variant.
 *
 * A flat list rather than a graph. The branching this system needs does not
 * happen inside a sequence: it happens when the sequence is picked, which is
 * the selection table's job. Variety within a branch is several sets the table
 * chooses between, which keeps this asset a list of attacks and nothing else.
 *
 * The conditions for entering a sequence do not live here. They live in the
 * rows of the selection table; holding them in both places would give the same
 * decision two sources of truth.
 */
UCLASS(BlueprintType, Const, Meta = (DisplayName = "Mace Combo Set"))
class MACECORE_API UMaceComboSet : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UMaceComboSet(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** What this sequence is. Selection results and logs name it by this. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mace|Combo", Meta = (Categories = "Mace.Combo"))
	FGameplayTag ComboTag;

	/** The blows, in the order they are thrown. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mace|Combo")
	TArray<TObjectPtr<UMaceAttackData>> Steps;

	/**
	 * Whether chaining out of this sequence asks the selection table again
	 * instead of simply advancing.
	 *
	 * On is what turns a punch chain into a kick when the target backs away.
	 * Off is for a sequence authored as one piece, where the third blow only
	 * reads correctly after the second.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mace|Combo")
	bool bReevaluateEachStep = true;

	/**
	 * The attack at that position, or null past the end of the sequence.
	 *
	 * Null is the answer, not a failure: it is how the ability learns the
	 * sequence is spent and ends, which is what stops button mashing from
	 * extending a chain forever.
	 */
	UFUNCTION(BlueprintPure, Category = "Mace|Combo")
	UMaceAttackData* GetStep(int32 StepIndex) const;
};
