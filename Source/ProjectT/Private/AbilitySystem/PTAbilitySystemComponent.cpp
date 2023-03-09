// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/PTAbilitySystemComponent.h"

void UPTAbilitySystemComponent::ReceiveDamage(UPTAbilitySystemComponent* SourceASC, float MitigatedDamage)
{
	OnReceiveDamage.Broadcast(SourceASC, MitigatedDamage);
}
