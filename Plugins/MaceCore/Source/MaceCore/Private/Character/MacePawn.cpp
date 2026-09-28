// Copyright srSerge. All Rights Reserved.

#include "Character/MacePawn.h"

#include "AbilitySystemComponent.h"

#include "Components/MaceAbilitySystemComponent.h"
#include "Components/MaceCollisionComponent.h"
#include "Components/MacePawnExtensionComponent.h"
#include "Data/MacePawnData.h"
#include "MaceGameplayTags.h"

AMacePawn::AMacePawn(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;

	AbilitySystemComponent = CreateDefaultSubobject<UMaceAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);

	// Mixed suits a player-controlled pawn: the owning client gets full effect
	// data, everyone else only sees gameplay cues and tags.
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	PawnExtensionComponent = CreateDefaultSubobject<UMacePawnExtensionComponent>(TEXT("PawnExtensionComponent"));

	// Every combatant gets one, attacker and target alike: whoever throws a
	// punch needs somewhere to keep the swing, and today's target is tomorrow's
	// attacker. It costs nothing while idle, since it only ticks between the
	// opening and closing of an attack window.
	CollisionComponent = CreateDefaultSubobject<UMaceCollisionComponent>(TEXT("CollisionComponent"));
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

void AMacePawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Super runs first on purpose: a Blueprint child that binds its own
	// movement and camera keeps working, and Mace only adds the ability
	// bindings on top.
	if (PawnExtensionComponent)
	{
		PawnExtensionComponent->InitializePlayerInput(PlayerInputComponent);
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
