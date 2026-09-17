// Copyright srSerge. All Rights Reserved.

#pragma once

#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"

#include "MaceAttackData.generated.h"

class UAnimMontage;
class UCameraShakeBase;
class UGameplayEffect;

/** Which limb throws the attack. Drives alternation, so two lefts never chain. */
UENUM(BlueprintType)
enum class EMaceAttackHand : uint8
{
	/** Neither, or both: a headbutt, a shoulder charge. */
	None,

	Left,

	Right
};

/**
 * UMaceAttackData
 *
 * One blow, and everything that blow carries: the animation, the limb, what it
 * does on impact and how it is traced.
 *
 * Everything travelling together is the point of the asset. Split up, changing
 * a hook means finding its damage on a notify, its sound on the montage and its
 * camera shake in the ability; here it is one asset and one edit.
 *
 * The impact fields are declared before anything reads them. The shape of an
 * attack is already known, so the stage that adds tracing and damage adds
 * behaviour rather than schema, and the attacks authored today survive it.
 *
 * Const, like every other Mace data asset: nothing writes to these at runtime.
 * PIE does not duplicate data assets, so a runtime write would persist into the
 * editor asset and outlive the session that made it.
 */
UCLASS(BlueprintType, Const, Meta = (DisplayName = "Mace Attack Data"))
class MACECORE_API UMaceAttackData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UMaceAttackData(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/**
	 * What this attack is. Identity is a tag rather than an index into the
	 * combo, because indices break the moment a sequence is reordered and this
	 * value outlives the sequence it was thrown from: it is what the next
	 * selection reads as "the previous attack".
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mace|Attack", Meta = (Categories = "Mace.Attack"))
	FGameplayTag AttackTag;

	/**
	 * The attack animation.
	 *
	 * Without ANS_ComboWindow on it, the attack plays but nothing can be
	 * chained from it: the notify is what opens the door.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mace|Attack")
	TObjectPtr<UAnimMontage> Montage;

	/** Which limb throws it. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mace|Attack")
	EMaceAttackHand Hand = EMaceAttackHand::None;

	//~ Impact. Declared now, read once hit detection exists.

	/** How a hit from this attack is classified, before the zone rule adjusts it. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mace|Impact", Meta = (Categories = "Mace.Damage.Class"))
	FGameplayTag DamageClass;

	/** Damage before the target's zone multiplier. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mace|Impact", Meta = (ClampMin = "0.0"))
	float BaseDamage = 0.0f;

	/** Effect applied to whoever is hit. The magnitude arrives by caller. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mace|Impact")
	TSubclassOf<UGameplayEffect> DamageEffect;

	/** False for an attack a guard cannot stop, such as a low kick. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mace|Impact")
	bool bCanBeBlocked = true;

	/** True when landing it on a blocking target breaks the guard. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mace|Impact")
	bool bBreaksGuard = false;

	/** Effect and sound played at the point of impact. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mace|Feedback", Meta = (Categories = "GameplayCue"))
	FGameplayTag HitCue;

	/** Camera shake on the attacker when the blow connects. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mace|Feedback")
	TSubclassOf<UCameraShakeBase> CameraShake;

	//~ Tracing. Per attack rather than per character, because the limb that
	//~ swings is a property of the blow: a hook traces one hand, a spinning
	//~ kick traces a foot and a shin.

	/** Sockets the hit trace sweeps between. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mace|Trace")
	TArray<FName> TraceSockets;

	/** Radius of the swept sphere, in centimetres. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mace|Trace", Meta = (ClampMin = "0.0"))
	float TraceRadius = 12.0f;
};
