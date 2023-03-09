// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/GA_BasicAttack.h"
#include "Kismet/KismetMathLibrary.h"

#include "AbilitySystem/PTAbilitySystemComponent.h"
#include "Characters/BaseHero.h"
#include "AbilitySystem/AT_PlayMontageAndWaitForEvent.h"

UGA_BasicAttack::UGA_BasicAttack()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	FGameplayTag Ability1Tag = FGameplayTag::RequestGameplayTag(FName("Ability.Attack.BasicAttack"));
	AbilityTags.AddTag(Ability1Tag);
	ActivationOwnedTags.AddTag(Ability1Tag);

	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Attack")));
}

void UGA_BasicAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}

	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	Character = Cast<ABaseHero>(ActorInfo->AvatarActor);

	FHitResult HitResult;
	ActorInfo->PlayerController->GetHitResultUnderCursorByChannel(ETraceTypeQuery::TraceTypeQuery1, true, HitResult);

	FRotator Rotation = UKismetMathLibrary::FindLookAtRotation(ActorInfo->AvatarActor->GetActorLocation(), HitResult.Location);
	Cast<ABaseHero>(ActorInfo->AvatarActor)->ServerSetActorRotationToMousePointer(Rotation);

	SectionName = *FString::Printf(TEXT("Attack%d"), Cast<ABaseHero>(ActorInfo->AvatarActor)->CurrentCombo);
	UE_LOG(LogTemp, Warning, TEXT("%d"), Cast<ABaseHero>(ActorInfo->AvatarActor)->CurrentCombo);

	UAT_PlayMontageAndWaitForEvent* Task = UAT_PlayMontageAndWaitForEvent::PlayMontageAndWaitForEvent(this, NAME_None, BasicAttackMontage, FGameplayTagContainer(), 1.0f, SectionName, false, 1.0f);

	Task->OnBlendOut.AddDynamic(this, &UGA_BasicAttack::OnCompleted);
	Task->OnCompleted.AddDynamic(this, &UGA_BasicAttack::OnCompleted);
	Task->OnInterrupted.AddDynamic(this, &UGA_BasicAttack::OnCancelled);
	Task->OnCancelled.AddDynamic(this, &UGA_BasicAttack::OnCancelled);
	Task->EventReceived.AddDynamic(this, &UGA_BasicAttack::EventReceived);

	Task->ReadyForActivation();
}

void UGA_BasicAttack::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_BasicAttack::OnCancelled(FGameplayTag EventTag, FGameplayEventData EventData)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UGA_BasicAttack::OnCompleted(FGameplayTag EventTag, FGameplayEventData EventData)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UGA_BasicAttack::EventReceived(FGameplayTag EventTag, FGameplayEventData EventData)
{
	if (EventTag == FGameplayTag::RequestGameplayTag(FName("Event.Montage.EndAbility")))
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		return;
	}

	if (EventTag == FGameplayTag::RequestGameplayTag(FName("Event.Attack.BasicAttackEnd")))
	{
		Character->ServerStopMontage(BasicAttackMontage);
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		return;
	}

	/*if (EventTag == FGameplayTag::RequestGameplayTag(FName("Event.Hit.SmallHit")))
	{
		FGameplayEffectContextHandle EffectContext = Character->AbilitySystemComponent->MakeEffectContext();
		FGameplayEffectSpecHandle SpecHandle = Character->AbilitySystemComponent->MakeOutgoingSpec(AttackEffectToTarget, 1, EffectContext);
		if (SpecHandle.IsValid())
		{
			UPTAbilitySystemComponent* TargetASC = Cast<ABaseCharacter>(EventData.Target)->AbilitySystemComponent;
			FActiveGameplayEffectHandle ActiveGEHandle = Character->AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
		}
		return;
	}*/
}
