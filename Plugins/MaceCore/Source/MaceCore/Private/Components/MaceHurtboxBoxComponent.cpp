// Copyright srSerge. All Rights Reserved.

#include "Components/MaceHurtboxBoxComponent.h"

#include "Engine/CollisionProfile.h"

UMaceHurtboxBoxComponent::UMaceHurtboxBoxComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// See UMaceHurtboxCapsuleComponent: a shape to author, never one to collide.
	SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	SetGenerateOverlapEvents(false);
	bCanEverAffectNavigation = false;
	CanCharacterStepUpOn = ECB_No;
	SetHiddenInGame(true);

	ShapeColor = FColor(255, 128, 0);
	InitBoxExtent(FVector(10.0f));
}
