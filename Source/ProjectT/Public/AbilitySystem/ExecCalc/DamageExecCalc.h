// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../PTAttributeSet.h"
#include "GameplayEffectExecutionCalculation.h"
#include "DamageExecCalc.generated.h"

struct FAttStruct
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Durability);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Health);

	DECLARE_ATTRIBUTE_CAPTUREDEF(Power);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Agility);

	DECLARE_ATTRIBUTE_CAPTUREDEF(Damage);

	FAttStruct()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UPTAttributeSet, Durability, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UPTAttributeSet, Health, Target, false);

		DEFINE_ATTRIBUTE_CAPTUREDEF(UPTAttributeSet, Power, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UPTAttributeSet, Agility, Source, false);

		DEFINE_ATTRIBUTE_CAPTUREDEF(UPTAttributeSet, Damage, Source, true);
	}
};

UCLASS()
class PROJECTT_API UDamageExecCalc : public UGameplayEffectExecutionCalculation
{
	GENERATED_UCLASS_BODY()
	
public:
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, OUT FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
