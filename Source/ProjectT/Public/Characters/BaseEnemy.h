// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseCharacter.h"
#include "BaseEnemy.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTT_API ABaseEnemy : public ABaseCharacter
{
	GENERATED_BODY()
	
public:
	ABaseEnemy();
	virtual void BeginPlay() override;
	UPROPERTY(VisibleAnywhere)
	class UWidgetComponent* HealthBarWidget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI")
	class UBehaviorTree* BTAsset;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI")
	class UBlackboardData* BBAsset;

	/** Attack */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void AttackBasic();
	UFUNCTION(BlueprintCallable)
	void AttackDamageEvent();

	/** Effect */
	UPROPERTY(EditDefaultsOnly, Category = "GAS")
	TSubclassOf<UGameplayEffect> AttackEffect;
};
