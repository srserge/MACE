// Copyright srSerge. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"

#include "MaceDebugLibrary.generated.h"

/**
 * UMaceDebugLibrary
 *
 * Diagnostics for Blueprint that end up where Mace's own diagnostics do.
 *
 * Print String always writes to LogBlueprintUserMessages at plain Log level, so
 * in the Output Log a Blueprint warning looks like any other message and only a
 * text prefix tells it apart. These nodes write to LogMace at Warning level
 * instead: yellow, filterable with the rest of the Mace categories, and tagged
 * with the object that raised them.
 *
 * Lives in MaceCore rather than in the MaceDebug plugin on purpose. That plugin
 * is a DeveloperTool module, which does not exist in a packaged build, so a
 * sample asset calling one of its nodes would fail to load there.
 */
UCLASS()
class MACECORE_API UMaceDebugLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	/**
	 * Reports something wrong but not fatal: a missing asset, a setup that
	 * silently does nothing. Written to LogMace as a warning and, unless told
	 * otherwise, to the screen.
	 *
	 * Development only, like Print String: the call is stripped from shipping.
	 */
	UFUNCTION(BlueprintCallable, Category = "Mace|Debug", Meta = (WorldContext = "WorldContextObject", CallableWithoutWorldContext, DevelopmentOnly, AdvancedDisplay = "bPrintToScreen,Duration", DisplayName = "Mace Warning", Keywords = "log print warning"))
	static void MaceWarning(const UObject* WorldContextObject, const FString& Message, bool bPrintToScreen = true, float Duration = 5.0f);
};
