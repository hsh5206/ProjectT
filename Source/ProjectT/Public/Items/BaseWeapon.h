// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/BaseItem.h"
#include "BaseWeapon.generated.h"

class UBoxComponent;

UCLASS()
class PROJECTT_API ABaseWeapon : public ABaseItem
{
	GENERATED_BODY()

public:
	ABaseWeapon();

protected:
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable)
	void OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(VisibleAnywhere)
	float Damage = 10;

	TArray<AActor*> IgnoreActors;

	FORCEINLINE UBoxComponent* GetWeaponBox() const { return WeaponBox; }
	void WeaponCollisionEanbled();
	void WeaponCollisionDisabled();

private:
	UPROPERTY(VisibleAnywhere)
	UBoxComponent* WeaponBox;
	UPROPERTY(VisibleAnywhere)
	USceneComponent* TraceStart;
	UPROPERTY(VisibleAnywhere)
	USceneComponent* TraceEnd;
};
