// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "PTAbilityInput.h"
#include "PTGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTT_API UPTGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	UPTGameplayAbility();

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Ability")
	EPTAbilityInputID AbilityInputID = EPTAbilityInputID::None;
};
