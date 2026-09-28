// Copyright srSerge. All Rights Reserved.

#pragma once

#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "Interfaces/MaceCombatant.h"

#include "MaceCharacter.generated.h"

class UAbilitySystemComponent;
class UMacePawnData;
class UMaceAbilitySystemComponent;
class UMaceCollisionComponent;
class UMacePawnExtensionComponent;

/**
 * AMaceCharacter
 *
 * A ready-made character base that wires up everything a Mace combatant needs:
 * an ability system component, a pawn extension component and a collision
 * component, all created in the constructor so a Blueprint child gets them for
 * free.
 *
 * This class is a convenience, not a requirement. The real product is the
 * components themselves, and any pawn can host them. It exists so the common case is
 * one reparent away instead of manual component wiring.
 *
 * It knows nothing about any specific movement implementation. A Blueprint
 * child brings its own movement, animation and input; this class only adds the
 * combat plumbing underneath.
 *
 * Derives from ACharacter, so it pairs with a CharacterMovementComponent. A
 * Mover-based pawn needs an APawn sibling instead: same two components,
 * different base.
 */
UCLASS(Blueprintable, Meta = (DisplayName = "Mace Character"))
class MACECORE_API AMaceCharacter : public ACharacter, public IAbilitySystemInterface, public IMaceCombatant
{
	GENERATED_BODY()

public:

	AMaceCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~ Begin IAbilitySystemInterface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	//~ End IAbilitySystemInterface

	//~ Begin IMaceCombatant
	virtual bool IsAlive_Implementation() const override;
	virtual AActor* GetCombatAvatar_Implementation() const override;
	//~ End IMaceCombatant

	UFUNCTION(BlueprintPure, Category = "Mace|Combat")
	UMacePawnExtensionComponent* GetPawnExtensionComponent() const { return PawnExtensionComponent; }

	UFUNCTION(BlueprintPure, Category = "Mace|Combat")
	UMaceCollisionComponent* GetCollisionComponent() const { return CollisionComponent; }

protected:

	virtual void PostInitializeComponents() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mace|Combat")
	TObjectPtr<UMaceAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mace|Combat")
	TObjectPtr<UMacePawnExtensionComponent> PawnExtensionComponent;

	/** Detects what an attack sweeps through. Idle until an attack opens a window. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mace|Combat")
	TObjectPtr<UMaceCollisionComponent> CollisionComponent;

	/**
	 * Which character this is: abilities, attributes, animation and UI.
	 *
	 * TEMPORARY: a hard reference, resolved while the pawn is constructed.
	 * Everything the pawn data points at loads with the pawn, which is fine
	 * with one test character and will not be once there are several.
	 *
	 * TODO: move to TSoftObjectPtr<UMacePawnData> loaded asynchronously, so a
	 * character's ability sets, animation blueprint and widgets stream in
	 * instead of blocking the spawn. The inner references of the pawn data are
	 * already soft for exactly this reason, so only this field has to change.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mace|Combat")
	TObjectPtr<const UMacePawnData> PawnData;
};
