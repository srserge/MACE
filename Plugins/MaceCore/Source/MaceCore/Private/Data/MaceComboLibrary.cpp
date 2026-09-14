// Copyright srSerge. All Rights Reserved.

#include "Data/MaceComboLibrary.h"

FGameplayTag UMaceComboLibrary::FindNextStep(const FMaceComboStep& Link, FGameplayTag InputTag)
{
	return Link.FindNextStep(InputTag);
}
