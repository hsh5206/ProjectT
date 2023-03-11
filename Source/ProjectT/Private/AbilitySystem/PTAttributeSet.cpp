// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/PTAttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"
#include "Components/WidgetComponent.h"
#include "AbilitySystemBlueprintLibrary.h"

#include "Characters/BaseHero.h"
#include "Characters/BaseEnemy.h"
#include "AbilitySystem/PTAttributeSet.h"
#include "Widgets/MainScreenWidget.h"
#include "Widgets/EnemyHealthBar.h"

UPTAttributeSet::UPTAttributeSet()
{

}

void UPTAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	DOREPLIFETIME_CONDITION_NOTIFY(UPTAttributeSet, LV, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPTAttributeSet, EXP, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPTAttributeSet, MaxEXP, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPTAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPTAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPTAttributeSet, Mana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPTAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPTAttributeSet, Vigor, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPTAttributeSet, Power, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPTAttributeSet, Agility, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPTAttributeSet, Durability, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPTAttributeSet, StatPoint, COND_None, REPNOTIFY_Always);
}

void UPTAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
	}
	else if (Attribute == GetManaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxMana());
	}
	else if (Attribute == GetEXPAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxEXP());
	}
	
}

void UPTAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	if (Attribute == GetHealthAttribute())
	{
		/** HealthBar Change */
		if (ABaseHero* Hero = Cast<ABaseHero>(GetOwningActor()))
		{
			if (UMainScreenWidget* MainWidget = Hero->MainWidget)
			{
				Hero->MainWidget->SetHPBarPercent(GetHealth(), GetMaxHealth());
			}
		}
		else if (ABaseEnemy* Enemy = Cast<ABaseEnemy>(GetOwningActor()))
		{
			if (UEnemyHealthBar* HealthBar = Cast<UEnemyHealthBar>(Enemy->HealthBarWidget->GetWidget()))
			{
				HealthBar->SetHPBarPercent(GetHealth(), GetMaxHealth());
			}
		}
	}
	else if (Attribute == GetManaAttribute())
	{
		/** ManaBar Change */
		if (ABaseHero* Hero = Cast<ABaseHero>(GetOwningActor()))
		{
			if (UMainScreenWidget* MainWidget = Hero->MainWidget)
			{
				Hero->MainWidget->SetMPBarPercent(GetMana(), GetMaxMana());
			}
		}
	}
	else if (Attribute == GetEXPAttribute())
	{
		/** EXPBar Change */
		if (ABaseHero* Hero = Cast<ABaseHero>(GetOwningActor()))
		{
			if (UMainScreenWidget* MainWidget = Hero->MainWidget)
			{
				if (FMath::IsNearlyEqual(GetEXP(), GetMaxEXP()))
				{
					FGameplayEventData Payload;
					FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName("Event.LvUp.LvUp"));

					UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Hero, Tag, Payload);
				}

				Hero->MainWidget->SetEXPBarPercent(GetEXP(), GetMaxEXP());

			}
		}
	}
}

void UPTAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		if (FMath::IsNearlyEqual(GetHealth(), 0.f))
		{
			DeathDelegate.Broadcast();
		}
	}
}

void UPTAttributeSet::OnRep_LV(const FGameplayAttributeData& OldLV)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPTAttributeSet, LV, OldLV);
}

void UPTAttributeSet::OnRep_EXP(const FGameplayAttributeData& OldEXP)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPTAttributeSet, EXP, OldEXP);
}

void UPTAttributeSet::OnRep_MaxEXP(const FGameplayAttributeData& OldMaxEXP)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPTAttributeSet, MaxEXP, OldMaxEXP);
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

void UPTAttributeSet::OnRep_Vigor(const FGameplayAttributeData& OldVigor)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPTAttributeSet, Vigor, OldVigor);
}

void UPTAttributeSet::OnRep_Power(const FGameplayAttributeData& OldPower)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPTAttributeSet, Power, OldPower);
}

void UPTAttributeSet::OnRep_Agility(const FGameplayAttributeData& OldAgility)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPTAttributeSet, Agility, OldAgility);
}

void UPTAttributeSet::OnRep_Durability(const FGameplayAttributeData& OldDurability)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPTAttributeSet, Durability, OldDurability);
}

void UPTAttributeSet::OnRep_StatPoint(const FGameplayAttributeData& OldStatPoint)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPTAttributeSet, StatPoint, OldStatPoint);

}
