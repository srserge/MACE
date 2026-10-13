// Copyright srSerge. All Rights Reserved.

#pragma once

#include "AbilitySystemComponent.h"
#include "GameplayTagContainer.h"

#include "MaceAbilitySystemComponent.generated.h"

/**
 * UMaceAbilitySystemComponent
 *
 * The ability system component every Mace combatant uses.
 *
 * Lyra subclasses the ability system component for the same reason this does:
 * anything that is about "how this project drives abilities" belongs next to
 * the abilities, not scattered across pawn components. Input is the first such
 * thing, and the tag relationship mapping will be the second.
 *
 * What it adds on top of the engine's component is an input buffer: it
 * remembers when each input tag was last pressed, so a press that arrives a
 * moment too early is not thrown away.
 *
 * That buffer is what separates a combo that feels responsive from one that
 * eats presses. A player who hits the button just before the window opens
 * expects the attack to come out; without a buffer nothing happens and the
 * input feels broken even though the game is behaving exactly as written.
 *
 * The buffer is deliberately generic. It knows nothing about combos: it records
 * tags and answers questions about them. A reload queued during a shot, a dodge
 * queued during a hit reaction and a combo chain all use the same three
 * functions.
 *
 * Every press routed through the pawn extension component is recorded here
 * automatically, so no caller has to remember to feed it.
 */
UCLASS(ClassGroup = (Mace), Meta = (BlueprintSpawnableComponent, DisplayName = "Mace Ability System"))
class MACECORE_API UMaceAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:

	UMaceAbilitySystemComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/**
	 * The Mace ability system component on an actor, or null when the actor has
	 * none or has a plain engine one.
	 *
	 * Resolved through UAbilitySystemGlobals rather than FindComponentByClass,
	 * so it also finds a component that lives on the player state instead of on
	 * the pawn.
	 */
	UFUNCTION(BlueprintPure, Category = "Mace|Ability", Meta = (DefaultToSelf = "Actor"))
	static UMaceAbilitySystemComponent* FindAbilitySystemComponent(const AActor* Actor);

	//~ Input buffer.
	//~
	//~ Lyra has no equivalent: its ability input is routed but never remembered
	//~ past the frame it arrived on, because none of its abilities chain. The
	//~ placement follows Lyra's reasoning even though the feature does not
	//~ exist there, since the ability system component is where Lyra keeps
	//~ everything about ability input.

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
	void ClearAllBufferedInput();

protected:

	//~ Begin UActorComponent
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~ End UActorComponent

private:

	/** World time of the last press of each tag. */
	TMap<FGameplayTag, double> LastPressTimes;
};
