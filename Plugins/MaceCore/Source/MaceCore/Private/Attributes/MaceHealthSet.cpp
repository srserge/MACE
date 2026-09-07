// Copyright srSerge. All Rights Reserved.

#include "Attributes/MaceHealthSet.h"

#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"

#include "MaceLogChannels.h"

UMaceHealthSet::UMaceHealthSet()
	: Health(100.0f)
	, MaxHealth(100.0f)
{
}

void UMaceHealthSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UMaceHealthSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMaceHealthSet, MaxHealth, COND_None, REPNOTIFY_Always);
}

void UMaceHealthSet::OnRep_Health(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMaceHealthSet, Health, OldValue);
}

void UMaceHealthSet::OnRep_MaxHealth(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMaceHealthSet, MaxHealth, OldValue);
}

void UMaceHealthSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetMaxHealthAttribute())
	{
		// A character with zero maximum health is always dead, which is never
		// what a designer meant to configure.
		NewValue = FMath::Max(NewValue, 1.0f);
	}
	else if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
	}
}

void UMaceHealthSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	AActor* OwningActor = GetOwningActor();

	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		const float AppliedDamage = GetDamage();

		// Turn the meta attribute into a health change, then clear it so it
		// never accumulates between executions.
		SetHealth(FMath::Clamp(GetHealth() - AppliedDamage, 0.0f, GetMaxHealth()));
		SetDamage(0.0f);

		UE_LOG(LogMaceDamage, Verbose, TEXT("%s took %.1f damage, %.1f health left."),
			*GetNameSafe(OwningActor), AppliedDamage, GetHealth());

		OnHealthChanged.Broadcast(OwningActor);
	}
	else if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		OnHealthChanged.Broadcast(OwningActor);
	}

	if ((GetHealth() <= 0.0f) && !bOutOfHealth)
	{
		bOutOfHealth = true;
		OnOutOfHealth.Broadcast(OwningActor);
	}
	else if (GetHealth() > 0.0f)
	{
		bOutOfHealth = false;
	}
}
