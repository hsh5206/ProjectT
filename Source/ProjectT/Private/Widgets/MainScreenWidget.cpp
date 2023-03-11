// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/MainScreenWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

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

void UMainScreenWidget::SetHPPortion(int32 num)
{
	HPTEXT->SetText(FText::FromString(FString::Printf(TEXT("%d"), num)));
}

int32 UMainScreenWidget::GetHPPortion()
{
	int32 IntNum = FCString::Atoi(*HPTEXT->GetText().ToString());
	return IntNum;
}

void UMainScreenWidget::SetMPPortion(int32 num)
{
	MPTEXT->SetText(FText::FromString(FString::Printf(TEXT("%d"), num)));
}

int32 UMainScreenWidget::GetMPPortion()
{
	int32 IntNum = FCString::Atoi(*MPTEXT->GetText().ToString());
	return IntNum;
}
