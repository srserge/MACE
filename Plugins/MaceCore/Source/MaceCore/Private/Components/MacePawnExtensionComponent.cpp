// Copyright srSerge. All Rights Reserved.

#include "Components/MacePawnExtensionComponent.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/Pawn.h"

#include "Components/MaceInputBufferComponent.h"
#include "Data/MaceCombatStyle.h"
#include "Data/MaceInputConfig.h"
#include "Data/MacePawnData.h"
#include "Data/MacePlayerData.h"
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

void UMacePawnExtensionComponent::InitializePlayerInput(UInputComponent* PlayerInputComponent)
{
	const UMacePlayerData* PlayerData = Cast<UMacePlayerData>(PawnData);
	if (!PlayerData || !PlayerData->InputConfig)
	{
		// An AI combatant has pawn data but no input config, which is normal.
		return;
	}

	UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (!EnhancedInput)
	{
		UE_LOG(LogMace, Error, TEXT("[%s] expects an enhanced input component. Check DefaultInputComponentClass in DefaultInput.ini."),
			*GetNameSafe(GetOwner()));
		return;
	}

	int32 BoundActions = 0;

	for (const FMaceInputAction& Action : PlayerData->InputConfig->AbilityInputActions)
	{
		if (!Action.InputAction || !Action.InputTag.IsValid())
		{
			continue;
		}

		// The tag rides along as a payload, so one handler serves every action
		// instead of a function per key.
		//
		// Started rather than Triggered: Triggered repeats every frame the key
		// is held unless the input action happens to carry a Pressed trigger,
		// which would fire an attack per frame and flood the input buffer.
		// Started fires once when the input begins, whatever the action's
		// trigger configuration, so a new input action cannot reintroduce the
		// problem by being configured differently.
		EnhancedInput->BindAction(Action.InputAction, ETriggerEvent::Started, this, &UMacePawnExtensionComponent::HandleAbilityInputPressed, Action.InputTag);
		EnhancedInput->BindAction(Action.InputAction, ETriggerEvent::Completed, this, &UMacePawnExtensionComponent::HandleAbilityInputReleased, Action.InputTag);

		++BoundActions;
	}

	UE_LOG(LogMace, Log, TEXT("Bound %d ability input action(s) on [%s] from [%s]."),
		BoundActions, *GetNameSafe(GetOwner()), *GetNameSafe(PlayerData->InputConfig));
}

void UMacePawnExtensionComponent::HandleAbilityInputPressed(FGameplayTag InputTag)
{
	// Recorded before anything else, so a press that no ability can use right
	// now is still available to whatever opens a window a moment later.
	if (UMaceInputBufferComponent* Buffer = UMaceInputBufferComponent::FindInputBufferComponent(GetOwner()))
	{
		Buffer->RecordInput(InputTag);
	}

	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	if (!ASC || !InputTag.IsValid())
	{
		return;
	}

	// Collect first: activating an ability can change the spec array, and
	// iterating it while it moves underneath is how this crashes.
	TArray<FGameplayAbilitySpecHandle> Handles;
	for (const FGameplayAbilitySpec& Spec : ASC->GetActivatableAbilities())
	{
		if (Spec.Ability && Spec.GetDynamicSpecSourceTags().HasTagExact(InputTag))
		{
			Handles.Add(Spec.Handle);
		}
	}

	for (const FGameplayAbilitySpecHandle& Handle : Handles)
	{
		FGameplayAbilitySpec* Spec = ASC->FindAbilitySpecFromHandle(Handle);
		if (!Spec)
		{
			continue;
		}

		if (Spec->IsActive())
		{
			// Already running: let it decide what a second press means. This is
			// what a combo ability listens to in order to chain.
			ASC->AbilitySpecInputPressed(*Spec);

			// AbilitySpecInputPressed alone is not enough. It only calls
			// UGameplayAbility::InputPressed, which is an empty stub in the
			// engine, while the WaitInputPress ability task subscribes to the
			// InputPressed generic replicated event instead. Without this the
			// two paths never meet and the task never fires.
			NotifyAbilityInputEvent(*Spec, EAbilityGenericReplicatedEvent::InputPressed);
		}
		else
		{
			ASC->TryActivateAbility(Handle);
		}
	}

	UE_LOG(LogMaceAbility, Verbose, TEXT("Input [%s] matched %d ability(ies) on [%s]."),
		*InputTag.ToString(), Handles.Num(), *GetNameSafe(GetOwner()));
}

void UMacePawnExtensionComponent::HandleAbilityInputReleased(FGameplayTag InputTag)
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	if (!ASC || !InputTag.IsValid())
	{
		return;
	}

	TArray<FGameplayAbilitySpecHandle> Handles;
	for (const FGameplayAbilitySpec& Spec : ASC->GetActivatableAbilities())
	{
		if (Spec.Ability && Spec.IsActive() && Spec.GetDynamicSpecSourceTags().HasTagExact(InputTag))
		{
			Handles.Add(Spec.Handle);
		}
	}

	for (const FGameplayAbilitySpecHandle& Handle : Handles)
	{
		if (FGameplayAbilitySpec* Spec = ASC->FindAbilitySpecFromHandle(Handle))
		{
			ASC->AbilitySpecInputReleased(*Spec);
			NotifyAbilityInputEvent(*Spec, EAbilityGenericReplicatedEvent::InputReleased);
		}
	}
}

void UMacePawnExtensionComponent::NotifyAbilityInputEvent(FGameplayAbilitySpec& Spec, EAbilityGenericReplicatedEvent::Type EventType)
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	if (!ASC)
	{
		return;
	}

	// The prediction key of the activation this event belongs to, read from the
	// running instance the same way the ability tasks derive the key they
	// subscribe with, so both sides agree on which activation is meant.
	FPredictionKey ActivationKey;
	for (const UGameplayAbility* Instance : Spec.GetAbilityInstances())
	{
		if (Instance)
		{
			ActivationKey = Instance->GetCurrentActivationInfo().GetActivationPredictionKey();
			break;
		}
	}

	// Fired locally so a predicting client reacts on the same frame, and
	// forwarded to the server so the authoritative copy of the ability sees the
	// same event. On the server the local call is already authoritative.
	ASC->InvokeReplicatedEvent(EventType, Spec.Handle, ActivationKey, ASC->ScopedPredictionKey);

	const AActor* Owner = GetOwner();
	if (Owner && !Owner->HasAuthority())
	{
		ASC->ServerSetReplicatedEvent(EventType, Spec.Handle, ActivationKey, ASC->ScopedPredictionKey);
	}
}

const UMaceCombatStyle* UMacePawnExtensionComponent::GetActiveCombatStyle() const
{
	// Derived, not stored. Once equipment exists this reads the active weapon
	// first and only falls through to the base style when the hands are empty,
	// so holstering restores the base style with nothing to remember.
	return PawnData ? PawnData->BaseCombatStyle : nullptr;
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

	// Without this, ability sets are granted but nothing can activate them:
	// the ability system has no idea which actor owns it and which one acts.
	// Owner is whatever actor holds the component, avatar is the pawn, so this
	// is correct whether the ASC lives on the pawn or on the player state.
	ASC->InitAbilityActorInfo(ASC->GetOwner(), Owner);

	int32 GrantedSets = 0;

	// What the character can do regardless of what it holds.
	for (const UMaceAbilitySet* AbilitySet : PawnData->AbilitySets)
	{
		if (AbilitySet)
		{
			AbilitySet->GiveToAbilitySystem(ASC, &GrantedHandles, this);
			++GrantedSets;
		}
	}

	// What the active combat style adds on top. The same handles record both,
	// so a style change revokes only what the style granted once the style
	// component exists to track them separately.
	if (const UMaceCombatStyle* Style = GetActiveCombatStyle())
	{
		for (const UMaceAbilitySet* AbilitySet : Style->AbilitySets)
		{
			if (AbilitySet)
			{
				AbilitySet->GiveToAbilitySystem(ASC, &GrantedHandles, this);
				++GrantedSets;
			}
		}
	}

	bAbilitySystemInitialized = true;

	UE_LOG(LogMace, Log, TEXT("Mace combatant ready on [%s] with pawn data [%s], style [%s], %d ability set(s) granted."),
		*GetNameSafe(Owner), *GetNameSafe(PawnData), *GetNameSafe(GetActiveCombatStyle()), GrantedSets);
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
