// Copyright srSerge. All Rights Reserved.

#include "Components/MacePawnExtensionComponent.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "GameFramework/Pawn.h"

#include "Data/MacePawnData.h"
#include "MaceLogChannels.h"

UMacePawnExtensionComponent::UMacePawnExtensionComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

UMacePawnExtensionComponent* UMacePawnExtensionComponent::FindPawnExtensionComponent(const AActor* Actor)
{
	return Actor ? Actor->FindComponentByClass<UMacePawnExtensionComponent>() : nullptr;
}

UAbilitySystemComponent* UMacePawnExtensionComponent::GetAbilitySystemComponent() const
{
	// Resolved through globals rather than a cast: this finds the component
	// whether it lives on the pawn or on the player state.
	return UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner());
}

void UMacePawnExtensionComponent::BeginPlay()
{
	Super::BeginPlay();

	InitializeAbilitySystem();
}

void UMacePawnExtensionComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UninitializeAbilitySystem();

	Super::EndPlay(EndPlayReason);
}

void UMacePawnExtensionComponent::SetPawnData(const UMacePawnData* InPawnData)
{
	AActor* Owner = GetOwner();
	if (!Owner || !Owner->HasAuthority())
	{
		return;
	}

	if (PawnData == InPawnData)
	{
		return;
	}

	UninitializeAbilitySystem();

	PawnData = InPawnData;

	InitializeAbilitySystem();
}

void UMacePawnExtensionComponent::InitializeAbilitySystem()
{
	if (bAbilitySystemInitialized)
	{
		return;
	}

	AActor* Owner = GetOwner();
	if (!Owner || !Owner->HasAuthority())
	{
		// Granting ability sets is server authoritative.
		return;
	}

	if (!PawnData)
	{
		UE_LOG(LogMace, Warning, TEXT("[%s] has a Mace Pawn Extension component but no pawn data assigned."), *GetNameSafe(Owner));
		return;
	}

	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	if (!ASC)
	{
		UE_LOG(LogMace, Warning, TEXT("[%s] has pawn data [%s] but no ability system component was found. Add one to the pawn or its player state."),
			*GetNameSafe(Owner), *GetNameSafe(PawnData));
		return;
	}

	int32 GrantedSets = 0;
	for (const UMaceAbilitySet* AbilitySet : PawnData->AbilitySets)
	{
		if (AbilitySet)
		{
			AbilitySet->GiveToAbilitySystem(ASC, &GrantedHandles, this);
			++GrantedSets;
		}
	}

	bAbilitySystemInitialized = true;

	UE_LOG(LogMace, Log, TEXT("Mace combatant ready on [%s] with pawn data [%s], %d ability set(s) granted."),
		*GetNameSafe(Owner), *GetNameSafe(PawnData), GrantedSets);
}

void UMacePawnExtensionComponent::UninitializeAbilitySystem()
{
	if (!bAbilitySystemInitialized)
	{
		return;
	}

	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		GrantedHandles.TakeFromAbilitySystem(ASC);
	}

	bAbilitySystemInitialized = false;
}
