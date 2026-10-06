// Copyright srSerge. All Rights Reserved.

#include "Abilities/MaceDamageExecution.h"

#include "AbilitySystemComponent.h"
#include "Engine/HitResult.h"

#include "Attributes/MaceHealthSet.h"
#include "Combat/MaceCombatLibrary.h"
#include "Components/MacePawnExtensionComponent.h"
#include "Data/MaceDamageProfile.h"
#include "Data/MacePawnData.h"
#include "MaceGameplayTags.h"
#include "MaceLogChannels.h"

namespace
{
	/** The target's health, captured so a zone's floor can be respected. */
	struct FMaceDamageCaptures
	{
		FGameplayEffectAttributeCaptureDefinition TargetHealth;

		FMaceDamageCaptures()
		{
			// Not snapshotted: the floor has to be measured against the health
			// the target has when the hit lands, not when the attack started.
			TargetHealth = FGameplayEffectAttributeCaptureDefinition(UMaceHealthSet::GetHealthAttribute(), EGameplayEffectAttributeCaptureSource::Target, /*bSnapshot=*/false);
		}
	};

	const FMaceDamageCaptures& DamageCaptures()
	{
		static const FMaceDamageCaptures Captures;
		return Captures;
	}

	const UMaceDamageProfile* FindDamageProfile(const AActor* Target)
	{
		const UMacePawnExtensionComponent* PawnExtension = UMacePawnExtensionComponent::FindPawnExtensionComponent(Target);
		const UMacePawnData* PawnData = PawnExtension ? PawnExtension->GetPawnData() : nullptr;
		return PawnData ? PawnData->DamageProfile.Get() : nullptr;
	}
}

UMaceDamageExecution::UMaceDamageExecution()
{
	RelevantAttributesToCapture.Add(DamageCaptures().TargetHealth);
}

void UMaceDamageExecution::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	const float BaseDamage = FMath::Max(0.0f, Spec.GetSetByCallerMagnitude(MaceGameplayTags::SetByCaller_Damage, /*WarnIfNotFound=*/true, 0.0f));
	if (BaseDamage <= 0.0f)
	{
		return;
	}

	FAggregatorEvaluateParameters EvaluateParams;
	EvaluateParams.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	EvaluateParams.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	float TargetHealth = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageCaptures().TargetHealth, EvaluateParams, TargetHealth);

	const UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();
	const AActor* Target = TargetASC ? TargetASC->GetAvatarActor() : nullptr;

	// The zone comes from where the blow actually landed, resolved by the
	// target's hurtboxes, not from what the attack was aiming at.
	const FHitResult* Hit = Spec.GetContext().GetHitResult();
	const FGameplayTag Zone = Hit ? UMaceCombatLibrary::GetHitZone(*Hit) : FGameplayTag();

	float Multiplier = 1.0f;
	float HealthFloor = 0.0f;

	if (const UMaceDamageProfile* Profile = FindDamageProfile(Target))
	{
		// No zone at all uses the profile's default rule. A zone the profile has
		// no rule for is left unmodified, as the profile's own contract says.
		const FMaceDamageZoneRule* Rule = Zone.IsValid() ? Profile->FindRuleForZone(FGameplayTagContainer(Zone)) : &Profile->DefaultRule;
		if (Rule)
		{
			Multiplier = Rule->DamageMultiplier;
			HealthFloor = Rule->MinHealthFloor;
		}
	}

	float FinalDamage = BaseDamage * Multiplier;

	// A zone with a floor can wound but not kill: the hit takes at most what
	// would leave the target standing at that floor. A target already at or
	// below it takes nothing from this zone, so it has to be finished elsewhere.
	if (HealthFloor > 0.0f)
	{
		FinalDamage = FMath::Min(FinalDamage, FMath::Max(0.0f, TargetHealth - HealthFloor));
	}

	UE_LOG(LogMaceDamage, Verbose, TEXT("[%s] hit on zone [%s]: %.1f base x %.2f = %.1f (health %.1f, floor %.1f)."),
		*GetNameSafe(Target), *Zone.ToString(), BaseDamage, Multiplier, FinalDamage, TargetHealth, HealthFloor);

	if (FinalDamage <= 0.0f)
	{
		return;
	}

	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UMaceHealthSet::GetDamageAttribute(), EGameplayModOp::Additive, FinalDamage));
}
