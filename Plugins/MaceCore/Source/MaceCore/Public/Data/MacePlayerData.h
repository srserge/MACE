// Copyright srSerge. All Rights Reserved.

#pragma once

#include "Data/MacePawnData.h"

#include "MacePlayerData.generated.h"

class UMaceInputConfig;

/**
 * UMacePlayerData
 *
 * A pawn data asset for a player-controlled character. Adds the pieces an AI
 * combatant has no use for: input bindings and weapon slots.
 *
 * Widgets are not here. A player finds its HUD the same way an enemy finds its
 * health bar, through the widget roles in UMacePawnData::Widgets, so the two
 * kinds of character share one mechanism instead of keeping two.
 */
UCLASS(BlueprintType, Const, Meta = (DisplayName = "Mace Player Data"))
class MACECORE_API UMacePlayerData : public UMacePawnData
{
	GENERATED_BODY()

public:

	UMacePlayerData(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** Which input actions this character responds to, and under which tags. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mace|Input")
	TObjectPtr<UMaceInputConfig> InputConfig;

	/** How many equipment slots the weapon wheel offers. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mace|Equipment", Meta = (ClampMin = "1"))
	int32 QuickBarSlots = 3;
};
