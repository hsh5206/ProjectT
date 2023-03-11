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
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* HPTEXT;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* MPTEXT;

	UFUNCTION(BlueprintCallable)
	void SetHPBarPercent(float CurrentHP, float MaxHP);
	UFUNCTION(BlueprintCallable)
	void SetMPBarPercent(float CurrentMP, float MaxMP);
	UFUNCTION(BlueprintCallable)
	void SetEXPBarPercent(float CurrentEXP, float MaxEXP);
	UFUNCTION(BlueprintCallable)
	void SetHPPortion(int32 num);
	UFUNCTION(BlueprintCallable)
	int32 GetHPPortion();
	UFUNCTION(BlueprintCallable)
	void SetMPPortion(int32 num);
	UFUNCTION(BlueprintCallable)
	int32 GetMPPortion();
};
