// Copyright srSerge. All Rights Reserved.

#pragma once

#include "Components/PawnComponent.h"
#include "Data/MaceAbilitySet.h"

#include "MacePawnExtensionComponent.generated.h"

class UAbilitySystemComponent;
class UMacePawnData;

/**
 * UMacePawnExtensionComponent
 *
 * The entry point of the whole system. Add it to any pawn, point it at a pawn
 * data asset, and the pawn becomes a Mace combatant: its ability sets are
 * granted to whatever ability system component the pawn exposes.
 *
 * Derives from UPawnComponent rather than UActorComponent so it plugs into the
 * game framework component manager, which is what allows it to attach to a pawn
 * the plugin does not own.
 *
 * The ability system component is always resolved through
 * UAbilitySystemGlobals rather than a direct cast, so a project that keeps its
 * ASC on the PlayerState instead of the pawn works without changes here.
 */
UCLASS(ClassGroup = (Mace), Blueprintable, Meta = (BlueprintSpawnableComponent, DisplayName = "Mace Pawn Extension"))
class MACECORE_API UMacePawnExtensionComponent : public UPawnComponent
{
	GENERATED_BODY()

public:

	UMacePawnExtensionComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** Convenience lookup so callers do not repeat FindComponentByClass. */
	UFUNCTION(BlueprintPure, Category = "Mace|Combat", Meta = (DefaultToSelf = "Actor"))
	static UMacePawnExtensionComponent* FindPawnExtensionComponent(const AActor* Actor);

	UFUNCTION(BlueprintPure, Category = "Mace|Combat")
	const UMacePawnData* GetPawnData() const { return PawnData; }

	/**
	 * Replaces the pawn data at runtime, revoking whatever the previous one
	 * granted. Server only.
	 */
	UFUNCTION(BlueprintCallable, Category = "Mace|Combat")
	void SetPawnData(const UMacePawnData* InPawnData);

	/** The ability system component this pawn uses, or null if it has none. */
	UFUNCTION(BlueprintPure, Category = "Mace|Combat")
	UAbilitySystemComponent* GetAbilitySystemComponent() const;

protected:

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/** Grants every ability set in the current pawn data. Server only. */
	void InitializeAbilitySystem();

	/** Revokes everything the current pawn data granted. Server only. */
	void UninitializeAbilitySystem();

	/** Which character this pawn is. Set it here or through SetPawnData. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mace|Combat")
	TObjectPtr<const UMacePawnData> PawnData;

private:

	/** Everything the current pawn data granted, kept so it can be taken back. */
	FMaceAbilitySet_GrantedHandles GrantedHandles;

	bool bAbilitySystemInitialized = false;
};
