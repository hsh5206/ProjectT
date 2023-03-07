// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "BasePlayerState.generated.h"

class UPTAttributeSet;

UCLASS()
class PROJECTT_API ABasePlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	UPTAttributeSet* Attributes;
};
