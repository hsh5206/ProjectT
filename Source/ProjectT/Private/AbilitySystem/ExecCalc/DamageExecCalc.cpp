// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/ExecCalc/DamageExecCalc.h"

#include "Characters/BaseHero.h"
#include "Components/InventoryComponent.h"
#include "Items/BaseWeapon.h"

UDamageExecCalc::UDamageExecCalc(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	FAttStruct Attributes;

	RelevantAttributesToCapture.Add(Attributes.DurabilityDef);
	InvalidScopedModifierAttributes.Add(Attributes.DurabilityDef);
	RelevantAttributesToCapture.Add(Attributes.HealthDef);
	InvalidScopedModifierAttributes.Add(Attributes.HealthDef);

	RelevantAttributesToCapture.Add(Attributes.PowerDef);
	RelevantAttributesToCapture.Add(Attributes.AgilityDef);

	RelevantAttributesToCapture.Add(Attributes.DamageDef);
}

void UDamageExecCalc::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, OUT FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	FAttStruct Attributes;

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();
	UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();

	AActor* Target = TargetASC ? TargetASC->GetAvatarActor() : nullptr;
	AActor* Source = SourceASC ? SourceASC->GetAvatarActor() : nullptr;

	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.TargetTags = TargetTags;
	EvaluationParameters.SourceTags = SourceTags;

	float Durability = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(Attributes.DurabilityDef, EvaluationParameters, Durability);

	float Power = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(Attributes.PowerDef, EvaluationParameters, Power);
	float Agility = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(Attributes.AgilityDef, EvaluationParameters, Agility);

	float Damage = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(Attributes.DamageDef, EvaluationParameters, Damage);

	// Set Base Damage
	if (ABaseHero* SourceCharacter = Cast<ABaseHero>(Source))
	{
		if (ABaseWeapon* Weapon = SourceCharacter->InventoryComponent->EquippingWeapon)
		{
			Damage = Weapon->Damage;
			UE_LOG(LogTemp, Warning, TEXT("WeaponDamage : %f"), Damage);
		}
	}
	Damage = (Damage + Power * 2.f) + (1.f + Agility * 0.1f);
	UE_LOG(LogTemp, Warning, TEXT("PureDamage : %f"), Damage);

	// Damage Calcuate
	float MitigatedDamage = Damage - Durability * 2.f;
	UE_LOG(LogTemp, Warning, TEXT("TotalDamage : %f"), MitigatedDamage);


	// Set the Target's damage meta attribute
	//if (MitigatedDamage > 1.f)
	//{
	//	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(Attributes.HealthProperty, EGameplayModOp::Additive, -MitigatedDamage));
	//}
	//else
	//{
	//	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(Attributes.HealthProperty, EGameplayModOp::Additive, -1.f));
	//}

	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(Attributes.HealthProperty, EGameplayModOp::Additive, -MitigatedDamage));



	// Broadcast damages to Target ASC
	if (UPTAbilitySystemComponent* TargetPTASC = Cast<UPTAbilitySystemComponent>(TargetASC))
	{
		UPTAbilitySystemComponent* SourcePTASC = Cast<UPTAbilitySystemComponent>(SourceASC);
		TargetPTASC->ReceiveDamage(SourcePTASC, MitigatedDamage);
	}
}
