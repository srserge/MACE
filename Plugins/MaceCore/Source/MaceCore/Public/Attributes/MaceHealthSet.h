// Copyright srSerge. All Rights Reserved.

#pragma once

#include "Attributes/MaceAttributeSet.h"

#include "MaceHealthSet.generated.h"

struct FGameplayEffectModCallbackData;

/** Broadcast when an attribute event happens. Owner may be null on clients. */
DECLARE_MULTICAST_DELEGATE_OneParam(FMaceHealthEvent, AActor* /*Owner*/);

/**
 * UMaceHealthSet
 *
 * Health, and the single funnel every point of damage has to pass through.
 *
 * Damage is a meta attribute: never replicated, never stored. It exists only
 * for the duration of an effect execution, is turned into a health subtraction
 * and is cleared. Combined with HideFromModifiers on Health, that guarantees
 * nothing can write health directly, which is what makes mitigation, blocking,
 * invulnerability and the zone health floor implementable in one place.
 */
UCLASS(BlueprintType)
class MACECORE_API UMaceHealthSet : public UMaceAttributeSet
{
	GENERATED_BODY()

public:

	UMaceHealthSet();

	MACE_ATTRIBUTE_ACCESSORS(UMaceHealthSet, Health);
	MACE_ATTRIBUTE_ACCESSORS(UMaceHealthSet, MaxHealth);
	MACE_ATTRIBUTE_ACCESSORS(UMaceHealthSet, Damage);

	/** Fires once when health first reaches zero. */
	mutable FMaceHealthEvent OnOutOfHealth;

	/** Fires whenever health changes for any reason. */
	mutable FMaceHealthEvent OnHealthChanged;

protected:

	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldValue);

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:

	/** Current health. Hidden from modifiers so only executions can change it. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health, Category = "Mace|Health", Meta = (HideFromModifiers, AllowPrivateAccess = true))
	FGameplayAttributeData Health;

	/** Maximum health. An attribute so effects can raise or lower the ceiling. */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth, Category = "Mace|Health", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxHealth;

	/** Meta attribute. Incoming damage for one execution, then cleared. */
	UPROPERTY(BlueprintReadOnly, Category = "Mace|Health", Meta = (HideFromModifiers, AllowPrivateAccess = true))
	FGameplayAttributeData Damage;

	/** Guards OnOutOfHealth so it fires once rather than on every further hit. */
	bool bOutOfHealth = false;
};
