// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/PTAttributeSet.h"
#include "Net/UnrealNetwork.h"

UPTAttributeSet::UPTAttributeSet()
{
}

void UPTAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	DOREPLIFETIME_CONDITION_NOTIFY(UPTAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPTAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPTAttributeSet, Mana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPTAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);

}

void UPTAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPTAttributeSet, Health, OldHealth);

}

void UPTAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPTAttributeSet, MaxHealth, OldMaxHealth);

}

void UPTAttributeSet::OnRep_Mana(const FGameplayAttributeData& OldMana)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPTAttributeSet, Mana, OldMana);

}

void UPTAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPTAttributeSet, MaxMana, OldMaxMana);

}
