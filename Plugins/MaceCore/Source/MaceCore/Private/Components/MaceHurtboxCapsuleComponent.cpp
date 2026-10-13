// Copyright srSerge. All Rights Reserved.

#include "Components/MaceHurtboxCapsuleComponent.h"

#include "Engine/CollisionProfile.h"

UMaceHurtboxCapsuleComponent::UMaceHurtboxCapsuleComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Only ever a shape to place in the editor. It is read and destroyed before
	// anything could collide with it, so it is set up to collide with nothing in
	// case it outlives that on an actor without a collision component.
	SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	SetGenerateOverlapEvents(false);
	bCanEverAffectNavigation = false;
	CanCharacterStepUpOn = ECB_No;
	SetHiddenInGame(true);

	// Orange, so it does not read as the movement capsule in the viewport.
	ShapeColor = FColor(255, 128, 0);
	InitCapsuleSize(10.0f, 20.0f);
}
