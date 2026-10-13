// Copyright srSerge. All Rights Reserved.

#pragma once

#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"

#include "MaceInputConfig.generated.h"

class UInputAction;

/** Pairs an input action with the gameplay tag the rest of the system knows it by. */
USTRUCT(BlueprintType)
struct FMaceInputAction
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mace")
	TObjectPtr<const UInputAction> InputAction = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mace", Meta = (Categories = "Mace.InputTag"))
	FGameplayTag InputTag;
};

/**
 * UMaceInputConfig
 *
 * Maps input actions to gameplay tags so that abilities never reference an
 * UInputAction directly. Rebinding a control, or shipping a different control
 * scheme per character, becomes a data change.
 */
UCLASS(BlueprintType, Const, Meta = (DisplayName = "Mace Input Config"))
class MACECORE_API UMaceInputConfig : public UDataAsset
{
	GENERATED_BODY()

public:

	UMaceInputConfig(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** Actions bound to C++ functions, such as movement and camera look. */
	UFUNCTION(BlueprintCallable, Category = "Mace|Input")
	const UInputAction* FindNativeInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound = true) const;

	/** Actions bound to an ability through its input tag. */
	UFUNCTION(BlueprintCallable, Category = "Mace|Input")
	const UInputAction* FindAbilityInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound = true) const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mace|Input", Meta = (TitleProperty = "InputTag"))
	TArray<FMaceInputAction> NativeInputActions;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mace|Input", Meta = (TitleProperty = "InputTag"))
	TArray<FMaceInputAction> AbilityInputActions;
};
