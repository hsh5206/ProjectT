// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/PTAbilitySystemComponent.h"

void UPTAbilitySystemComponent::ReceiveDamage(UPTAbilitySystemComponent* SourceASC, float UnmitigateDamage, float MitigatedDamage)
{
	OnReceiveDamage.Broadcast(SourceASC, UnmitigateDamage, MitigatedDamage);
}
