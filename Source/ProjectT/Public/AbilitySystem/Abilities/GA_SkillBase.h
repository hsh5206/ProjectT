// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/PTGameplayAbility.h"
#include "GA_SKillBase.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTT_API UGA_SKillBase : public UPTGameplayAbility
{
	GENERATED_BODY()
	
public:
	UGA_SKillBase();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	class ABaseHero* Character;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* SkllMontage;

};
