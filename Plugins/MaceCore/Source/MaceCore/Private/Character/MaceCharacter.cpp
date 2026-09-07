// Copyright srSerge. All Rights Reserved.

#include "Character/MaceCharacter.h"

#include "AbilitySystemComponent.h"

#include "Components/MacePawnExtensionComponent.h"
#include "Data/MacePawnData.h"
#include "MaceGameplayTags.h"

AMaceCharacter::AMaceCharacter(const FObjectInitializer& ObjectInitializer)
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

UAbilitySystemComponent* AMaceCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AMaceCharacter::PostInitializeComponents()
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

bool AMaceCharacter::IsAlive_Implementation() const
{
	if (!AbilitySystemComponent)
	{
		return true;
	}

	return !AbilitySystemComponent->HasMatchingGameplayTag(MaceGameplayTags::State_Dead);
}

AActor* AMaceCharacter::GetCombatAvatar_Implementation() const
{
	return const_cast<AMaceCharacter*>(this);
}
