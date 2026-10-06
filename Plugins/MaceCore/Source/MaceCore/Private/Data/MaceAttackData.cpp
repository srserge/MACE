// Copyright srSerge. All Rights Reserved.

#include "Data/MaceAttackData.h"

#include "GameFramework/Actor.h"

UMaceAttackData::UMaceAttackData(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Montage = nullptr;
	DamageEffect = nullptr;
	CameraShake = nullptr;
}

FVector UMaceAttackData::GetWorldImpactVelocity(const AActor* Attacker) const
{
	if (!Attacker)
	{
		return FVector::ZeroVector;
	}

	const FRotator Facing(0.0f, Attacker->GetActorRotation().Yaw, 0.0f);
	return Facing.RotateVector(ImpactVelocity);
}
