// Copyright srSerge. All Rights Reserved.

#pragma once

#include "Data/MaceComboStep.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "MaceComboLibrary.generated.h"

/**
 * UMaceComboLibrary
 *
 * Blueprint access to the combo link data.
 *
 * A USTRUCT cannot carry UFUNCTION members at all, so a struct's operations
 * only reach Blueprint through a function library. That is the whole reason
 * this class exists: FMaceComboStep::FindNextStep is plain C++ and invisible
 * to a Blueprint graph without it.
 */
UCLASS()
class MACECORE_API UMaceComboLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	/**
	 * The step this input continues to from the given step, or an invalid tag
	 * when the input does not continue the chain. An invalid result means the
	 * combo ends here.
	 */
	UFUNCTION(BlueprintPure, Category = "Mace|Combo", Meta = (DisplayName = "Find Next Link"))
	static FGameplayTag FindNextStep(const FMaceComboStep& Link, FGameplayTag InputTag);
};
