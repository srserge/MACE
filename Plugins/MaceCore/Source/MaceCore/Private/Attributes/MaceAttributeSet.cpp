// Copyright srSerge. All Rights Reserved.

#include "Attributes/MaceAttributeSet.h"

#include "Engine/World.h"

UMaceAttributeSet::UMaceAttributeSet()
{
}

UWorld* UMaceAttributeSet::GetWorld() const
{
	const UObject* Outer = GetOuter();
	check(Outer);

	return Outer->GetWorld();
}
