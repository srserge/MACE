// Copyright srSerge. All Rights Reserved.

#pragma once

#include "AbilitySystemInterface.h"
#include "GameFramework/Pawn.h"
#include "Interfaces/MaceCombatant.h"

#include "MacePawn.generated.h"

class UAbilitySystemComponent;
class UMacePawnData;
class UMacePawnExtensionComponent;

/**
 * AMacePawn
 *
 * The APawn-based sibling of AMaceCharacter. Same two components, same
 * interfaces, different base class.
 *
 * It exists because a movement system is a choice, not a given. ACharacter
 * hard-wires a character movement component, which suits some projects and not
 * others: a Mover-driven pawn derives from APawn and brings its own movement,
 * and so does anything built on a custom movement component.
 *
 * That is what "movement agnostic" means in practice. The product is the pair
 * of components, which work on any pawn. These two base classes are only
 * convenience wrappers so the common cases are one reparent away, and adding a
 * third for some other base would be another dozen lines.
 *
 * Deliberately knows nothing about Mover, or about any other movement plugin:
 * MaceCore must not depend on anything experimental.
 */
UCLASS(Blueprintable, Meta = (DisplayName = "Mace Pawn"))
class MACECORE_API AMacePawn : public APawn, public IAbilitySystemInterface, public IMaceCombatant
{
	GENERATED_BODY()

public:

	AMacePawn(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~ Begin IAbilitySystemInterface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	//~ End IAbilitySystemInterface

	//~ Begin IMaceCombatant
	virtual bool IsAlive_Implementation() const override;
	virtual AActor* GetCombatAvatar_Implementation() const override;
	//~ End IMaceCombatant

	UFUNCTION(BlueprintPure, Category = "Mace|Combat")
	UMacePawnExtensionComponent* GetPawnExtensionComponent() const { return PawnExtensionComponent; }

protected:

	virtual void PostInitializeComponents() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mace|Combat")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mace|Combat")
	TObjectPtr<UMacePawnExtensionComponent> PawnExtensionComponent;

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
