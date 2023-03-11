// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/MainScreenWidget.h"
#include "Components/ProgressBar.h"

void UMainScreenWidget::SetHPBarPercent(float CurrentHP, float MaxHP)
{
	HPBar->SetPercent(CurrentHP / MaxHP);
}

void UMainScreenWidget::SetMPBarPercent(float CurrentMP, float MaxMP)
{
	ManaBar->SetPercent(CurrentMP / MaxMP);
}

void UMainScreenWidget::SetEXPBarPercent(float CurrentEXP, float MaxEXP)
{
	EXPBar->SetPercent(CurrentEXP / MaxEXP);
}
