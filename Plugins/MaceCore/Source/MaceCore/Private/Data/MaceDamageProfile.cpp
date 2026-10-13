// Copyright srSerge. All Rights Reserved.

#include "Data/MaceDamageProfile.h"

UMaceDamageProfile::UMaceDamageProfile(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

const FMaceDamageZoneRule* UMaceDamageProfile::FindRuleForZone(const FGameplayTagContainer& ZoneTags) const
{
	if (ZoneTags.IsEmpty())
	{
		return nullptr;
	}

	for (const FMaceDamageZoneRule& Rule : ZoneRules)
	{
		if (Rule.ZoneTag.IsValid() && ZoneTags.HasTag(Rule.ZoneTag))
		{
			return &Rule;
		}
	}

	return nullptr;
}
