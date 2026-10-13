// Copyright srSerge. All Rights Reserved.

#include "Debug/MaceDebugLibrary.h"

#include "Engine/Engine.h"

#include "MaceLogChannels.h"

void UMaceDebugLibrary::MaceWarning(const UObject* WorldContextObject, const FString& Message, bool bPrintToScreen, float Duration)
{
#if !UE_BUILD_SHIPPING
	// The Blueprint compiler fills the world context with self, so the caller
	// is named without anyone having to type it. Blueprints that have no world,
	// such as a notify state, pass nothing and are reported as None.
	const FString Source = GetNameSafe(WorldContextObject);

	UE_LOG(LogMace, Warning, TEXT("[%s] %s"), *Source, *Message);

	// The screen has no log categories, so the prefix does the job there that
	// LogMace: Warning does in the Output Log.
	if (bPrintToScreen && GEngine)
	{
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, Duration, FColor::Yellow,
			FString::Printf(TEXT("Mace Warning [%s]: %s"), *Source, *Message));
	}
#endif
}
