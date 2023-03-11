// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/BaseEnemy.h"
#include "Components/WidgetComponent.h"

#include "Widgets/EnemyHealthBar.h"
#include "AbilitySystem/PTAttributeSet.h"

ABaseEnemy::ABaseEnemy()
{
	HealthBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBar"));
	HealthBarWidget->SetupAttachment(GetRootComponent());
}

void ABaseEnemy::BeginPlay()
{
	Super::BeginPlay();
	if (HealthBarWidget->GetWidget())
	{
		Cast<UEnemyHealthBar>(HealthBarWidget->GetWidget())->SetHPBarPercent(Attributes->GetHealth(), Attributes->GetMaxHealth());
	}
}
