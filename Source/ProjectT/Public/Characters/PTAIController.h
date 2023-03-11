// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "PTAIController.generated.h"


UCLASS()
class PROJECTT_API APTAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	APTAIController();

	static const FName Key_Target;
	AActor* Opponent;

protected:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void Tick(float DeltaSeconds) override;

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI", meta = (AllowPrivateAccess = "true"))
	UBlackboardComponent* BlackboardComponent;

	APawn* ControlledPawn;
	
	
public:
	/** AI Perception */
	class UAISenseConfig_Sight* SightConfig;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float AISightRadius = 1500.f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float AILoseSightRadius = 50.f;

	UFUNCTION(BlueprintCallable)
	void PerceptionUpdated(const TArray<AActor*>& UpdatedActors);
};
