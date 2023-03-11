// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/PTAbilitySystemComponent.h"
#include "Components/WidgetComponent.h"

void UPTAbilitySystemComponent::ReceiveDamage(UPTAbilitySystemComponent* SourceASC, float MitigatedDamage)
{
	OnReceiveDamage.Broadcast(SourceASC, MitigatedDamage);

	/** Damage On Screen */

	
}
