// Copyright srSerge. All Rights Reserved.

#include "Data/MaceComboSet.h"

UMaceComboSet::UMaceComboSet(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UMaceAttackData* UMaceComboSet::GetStep(int32 StepIndex) const
{
	return Steps.IsValidIndex(StepIndex) ? Steps[StepIndex] : nullptr;
}
