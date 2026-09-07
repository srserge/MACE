// Copyright srSerge. All Rights Reserved.

#pragma once

#include "AbilitySystemComponent.h"
#include "AttributeSet.h"

#include "MaceAttributeSet.generated.h"

/**
 * Declares the four accessors GAS expects for an attribute.
 *
 * MACE_ATTRIBUTE_ACCESSORS(UMaceHealthSet, Health) generates:
 *     static FGameplayAttribute GetHealthAttribute();
 *     float GetHealth() const;
 *     void  SetHealth(float NewVal);
 *     void  InitHealth(float NewVal);
 */
#define MACE_ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 * UMaceAttributeSet
 *
 * Base class for every attribute set in Mace. Exists so shared behaviour has
 * somewhere to live and so attribute sets can be recognised by a common type.
 */
UCLASS()
class MACECORE_API UMaceAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:

	UMaceAttributeSet();

	virtual UWorld* GetWorld() const override;
};
