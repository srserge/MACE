// Copyright srSerge. All Rights Reserved.

#pragma once

#include "UObject/Interface.h"

#include "MaceTargetProvider.generated.h"

UINTERFACE(MinimalAPI, BlueprintType)
class UMaceTargetProvider : public UInterface
{
	GENERATED_BODY()
};

/**
 * IMaceTargetProvider
 *
 * One of the seams where the combat styles differ. How the player picks a
 * target is not the same thing in every game:
 *
 *   modern third person  -> soft aim assist, driven by where the camera looks
 *   character action     -> hard lock onto one enemy at a time
 *
 * MaceCore never knows which implementation is active. Declared now, with no
 * implementation, so that adding the second style later does not require
 * reworking the core.
 */
class MACECORE_API IMaceTargetProvider
{
	GENERATED_BODY()

public:

	/** Current target, or null when nothing is targeted. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Mace|Targeting")
	AActor* GetCurrentTarget() const;

	/** Cheap check that avoids resolving the target when only presence matters. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Mace|Targeting")
	bool HasTarget() const;
};
