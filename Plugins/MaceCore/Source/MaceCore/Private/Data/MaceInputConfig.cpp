// Copyright srSerge. All Rights Reserved.

#include "Data/MaceInputConfig.h"

#include "MaceLogChannels.h"

UMaceInputConfig::UMaceInputConfig(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

const UInputAction* UMaceInputConfig::FindNativeInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound) const
{
	for (const FMaceInputAction& Action : NativeInputActions)
	{
		if (Action.InputAction && (Action.InputTag == InputTag))
		{
			return Action.InputAction;
		}
	}

	if (bLogNotFound)
	{
		UE_LOG(LogMace, Error, TEXT("Can't find native input action for tag [%s] on input config [%s]."), *InputTag.ToString(), *GetNameSafe(this));
	}

	return nullptr;
}

const UInputAction* UMaceInputConfig::FindAbilityInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound) const
{
	for (const FMaceInputAction& Action : AbilityInputActions)
	{
		if (Action.InputAction && (Action.InputTag == InputTag))
		{
			return Action.InputAction;
		}
	}

	if (bLogNotFound)
	{
		UE_LOG(LogMace, Error, TEXT("Can't find ability input action for tag [%s] on input config [%s]."), *InputTag.ToString(), *GetNameSafe(this));
	}

	return nullptr;
}
