// Copyright srSerge. All Rights Reserved.

#pragma once

#include "Components/BoxComponent.h"
#include "GameplayTagContainer.h"

#include "MaceHurtboxBoxComponent.generated.h"

/**
 * UMaceHurtboxBoxComponent
 *
 * One part of a character that can be hit, authored as a box. The same as
 * UMaceHurtboxCapsuleComponent in every respect but its shape: a box suits a
 * torso or a pelvis better than a capsule does.
 */
UCLASS(ClassGroup = (Mace), Meta = (BlueprintSpawnableComponent, DisplayName = "Mace Hurtbox (Box)"))
class MACECORE_API UMaceHurtboxBoxComponent : public UBoxComponent
{
	GENERATED_BODY()

public:

	UMaceHurtboxBoxComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** Which part of the body this is. Use the finest zone that applies. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mace|Hurtbox", Meta = (Categories = "Mace.HitZone"))
	FGameplayTag ZoneTag;
};
