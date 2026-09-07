// Copyright srSerge. All Rights Reserved.

#include "Character/MacePawn.h"

#include "AbilitySystemComponent.h"

#include "Components/MacePawnExtensionComponent.h"
#include "Data/MacePawnData.h"
#include "MaceGameplayTags.h"

AMacePawn::AMacePawn(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);

	// Mixed suits a player-controlled pawn: the owning client gets full effect
	// data, everyone else only sees gameplay cues and tags.
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	PawnExtensionComponent = CreateDefaultSubobject<UMacePawnExtensionComponent>(TEXT("PawnExtensionComponent"));
}

UAbilitySystemComponent* AMacePawn::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AMacePawn::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// Hand the pawn data down before the component's BeginPlay grants anything.
	// The component keeps its own PawnData property for pawns that do not
	// derive from this class, such as AI combatants on a plain pawn.
	if (PawnData && PawnExtensionComponent)
	{
		PawnExtensionComponent->SetPawnData(PawnData);
	}
}

bool AMacePawn::IsAlive_Implementation() const
{
	if (!AbilitySystemComponent)
	{
		return true;
	}

	return !AbilitySystemComponent->HasMatchingGameplayTag(MaceGameplayTags::State_Dead);
}

AActor* AMacePawn::GetCombatAvatar_Implementation() const
{
	return const_cast<AMacePawn*>(this);
}
