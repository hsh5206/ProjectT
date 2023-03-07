// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/PTGameplayAbility.h"
#include "GA_BasicAttack.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTT_API UGA_BasicAttack : public UPTGameplayAbility
{
	GENERATED_BODY()

public:
	UGA_BasicAttack();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled);

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	UGameplayEffect* AttackEffectToTarget;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* BasicAttackMontage;
};
