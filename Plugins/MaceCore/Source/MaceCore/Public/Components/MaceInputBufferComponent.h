// Copyright srSerge. All Rights Reserved.

#pragma once

#include "Components/PawnComponent.h"
#include "GameplayTagContainer.h"

#include "MaceInputBufferComponent.generated.h"

/**
 * UMaceInputBufferComponent
 *
 * Remembers when each input tag was last pressed, so an action that arrives a
 * moment too early is not thrown away.
 *
 * This is what separates a combo that feels responsive from one that eats
 * presses. A player who hits the button just before the window opens expects
 * the attack to come out; without a buffer, nothing happens and the input feels
 * broken even though the game is behaving as written.
 *
 * Deliberately generic. It knows nothing about combos: it records tags and
 * answers questions about them. A reload queued during a shot, a dodge queued
 * during a hit reaction and a combo chain all use the same three functions.
 *
 * Every press routed through the pawn extension component is recorded here
 * automatically, so nothing has to remember to feed it.
 */
UCLASS(ClassGroup = (Mace), Blueprintable, Meta = (BlueprintSpawnableComponent, DisplayName = "Mace Input Buffer"))
class MACECORE_API UMaceInputBufferComponent : public UPawnComponent
{
	GENERATED_BODY()

public:

	UMaceInputBufferComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintPure, Category = "Mace|Input", Meta = (DefaultToSelf = "Actor"))
	static UMaceInputBufferComponent* FindInputBufferComponent(const AActor* Actor);

	/** Records a press. Called by the pawn extension component on every input. */
	void RecordInput(FGameplayTag InputTag);

	/**
	 * True if the tag was pressed within the window, and clears it so the same
	 * press cannot be consumed twice.
	 *
	 * This is the one to call when acting on the input: a combo asks for it the
	 * moment its window opens.
	 */
	UFUNCTION(BlueprintCallable, Category = "Mace|Input")
	bool ConsumeBufferedInput(FGameplayTag InputTag, float WindowSeconds = 0.3f);

	/** Same question without consuming, for UI or for a check before deciding. */
	UFUNCTION(BlueprintPure, Category = "Mace|Input")
	bool HasBufferedInput(FGameplayTag InputTag, float WindowSeconds = 0.3f) const;

	/** Seconds since the tag was last pressed, or a large number if never. */
	UFUNCTION(BlueprintPure, Category = "Mace|Input")
	float GetTimeSinceInput(FGameplayTag InputTag) const;

	/** Forgets a pending press, for when an action is cancelled outright. */
	UFUNCTION(BlueprintCallable, Category = "Mace|Input")
	void ClearBufferedInput(FGameplayTag InputTag);

	/** Forgets everything. Death, respawn, cutscene. */
	UFUNCTION(BlueprintCallable, Category = "Mace|Input")
	void ClearAll();

private:

	/** World time of the last press of each tag. */
	TMap<FGameplayTag, double> LastPressTimes;
};
