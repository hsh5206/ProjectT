// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/EnemyHealthBar.h"
#include "Components/ProgressBar.h"


void UEnemyHealthBar::SetHPBarPercent(float CurrentHP, float MaxHP)
{
	HPBar->SetPercent(CurrentHP / MaxHP);
}
