// Copyright srSerge. All Rights Reserved.

#pragma once

#include "Data/MacePawnData.h"

#include "MacePlayerData.generated.h"

class UMaceInputConfig;

/**
 * UMacePlayerData
 *
 * A pawn data asset for a player-controlled character. Adds the pieces an AI
 * combatant has no use for: input bindings, on-screen UI and weapon slots.
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

	/**
	 * HUD widgets belonging to this character.
	 *
	 * Typed as UObject with a MetaClass filter rather than TSoftClassPtr<UUserWidget>
	 * on purpose: it gives the editor a widget-only picker without dragging UMG
	 * into a runtime gameplay core. A dedicated server should not link UMG.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mace|UI", Meta = (MetaClass = "/Script/UMG.UserWidget"))
	TArray<TSoftClassPtr<UObject>> HUDWidgets;

	/** How many equipment slots the weapon wheel offers. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mace|Equipment", Meta = (ClampMin = "1"))
	int32 QuickBarSlots = 3;
};
