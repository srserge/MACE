// Copyright srSerge. All Rights Reserved.

#pragma once

#include "Components/CapsuleComponent.h"
#include "GameplayTagContainer.h"

#include "MaceHurtboxCapsuleComponent.generated.h"

/**
 * UMaceHurtboxCapsuleComponent
 *
 * One part of a character that can be hit, authored as a capsule.
 *
 * It is a component only while it is being authored: it hangs from a bone in the
 * Blueprint viewport, where it can be seen and dragged into place. At BeginPlay
 * the character's UMaceCollisionComponent reads its shape and destroys it, and
 * from then on the hurtbox is data. That is why a character nobody is hitting
 * pays nothing for carrying twenty of them.
 *
 * Kept apart from the physics asset on purpose. Those bodies are tuned for the
 * ragdoll and these for play, and neither should have to compromise for the
 * other.
 */
UCLASS(ClassGroup = (Mace), Meta = (BlueprintSpawnableComponent, DisplayName = "Mace Hurtbox (Capsule)"))
class MACECORE_API UMaceHurtboxCapsuleComponent : public UCapsuleComponent
{
	GENERATED_BODY()

public:

	UMaceHurtboxCapsuleComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** Which part of the body this is. Use the finest zone that applies. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mace|Hurtbox", Meta = (Categories = "Mace.HitZone"))
	FGameplayTag ZoneTag;
};
