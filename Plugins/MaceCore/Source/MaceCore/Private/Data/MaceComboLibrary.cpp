// Copyright srSerge. All Rights Reserved.

#include "Data/MaceComboLibrary.h"

FGameplayTag UMaceComboLibrary::FindNextLink(const FMaceComboLink& Link, FGameplayTag InputTag)
{
	return Link.FindNextLink(InputTag);
}
