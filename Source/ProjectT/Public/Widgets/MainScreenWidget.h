// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainScreenWidget.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTT_API UMainScreenWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HPBar;
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* ManaBar;
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* EXPBar;

	void SetHPBarPercent(float CurrentHP, float MaxHP);
	void SetMPBarPercent(float CurrentMP, float MaxMP);
	void SetEXPBarPercent(float CurrentEXP, float MaxEXP);
};
