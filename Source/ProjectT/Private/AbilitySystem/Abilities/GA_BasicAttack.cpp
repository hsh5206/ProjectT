// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/GA_BasicAttack.h"

#include "AbilitySystem/PTAbilitySystemComponent.h"
#include "Characters/BaseAnimInstance.h"

UGA_BasicAttack::UGA_BasicAttack()
{

}

void UGA_BasicAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (UBaseAnimInstance* AI = Cast<UBaseAnimInstance>(ActorInfo->AnimInstance.Get()))
	{
		if (BasicAttackMontage)
		{
			AI->Montage_Play(BasicAttackMontage);
		}
	}
	
	//AttackCheck

	FGameplayEffectContextHandle ContextHandle = ActorInfo->AbilitySystemComponent->MakeEffectContext();
	
}

void UGA_BasicAttack::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
