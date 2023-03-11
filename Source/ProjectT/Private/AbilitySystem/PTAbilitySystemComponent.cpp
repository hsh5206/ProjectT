// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/PTAbilitySystemComponent.h"
#include "Components/WidgetComponent.h"

#include "Characters/BaseHero.h"
#include "Characters/BaseEnemy.h"
#include "AbilitySystem/PTAttributeSet.h"
#include "Widgets/MainScreenWidget.h"
#include "Widgets/EnemyHealthBar.h"

void UPTAbilitySystemComponent::ReceiveDamage(UPTAbilitySystemComponent* SourceASC, float MitigatedDamage)
{
	OnReceiveDamage.Broadcast(SourceASC, MitigatedDamage);

	/** Damage On Screen */
	if (ABaseHero* Hero = Cast<ABaseHero>(GetOwner()))
	{
		Hero->MainWidget->SetHPBarPercent(Hero->Attributes->GetHealth() - MitigatedDamage, Hero->Attributes->GetMaxHealth());
	}
	else if (ABaseEnemy* Enemy = Cast<ABaseEnemy>(GetOwner()))
	{
		UE_LOG(LogTemp, Warning, TEXT("Enemy Health Changed! HP : %f"), Enemy->Attributes->GetHealth() - MitigatedDamage);
		Cast<UEnemyHealthBar>(Enemy->HealthBarWidget->GetWidget())->SetHPBarPercent(Enemy->Attributes->GetHealth() - MitigatedDamage, Enemy->Attributes->GetMaxHealth());
	}
}
