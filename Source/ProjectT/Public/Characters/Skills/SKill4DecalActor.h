// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SKill4DecalActor.generated.h"

UCLASS()
class PROJECTT_API ASKill4DecalActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ASKill4DecalActor();

	UPROPERTY(EditDefaultsOnly)
	class USceneComponent* DefaultRoot;
	UPROPERTY(EditDefaultsOnly)
	class UDecalComponent* Decal;
	UPROPERTY(EditDefaultsOnly)
	class USphereComponent* SphereCollision;

	UPROPERTY()
	TArray<class ABaseEnemy*> OverlappingEnemies;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UGameplayEffect> Skill4Effect;

protected:
	virtual void BeginPlay() override;

public:	
	UFUNCTION()
	void SphereOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void SphereOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	UFUNCTION()
	void DamageToEnemies();
	UFUNCTION()
	void FinalDamageToEnemies();

};
