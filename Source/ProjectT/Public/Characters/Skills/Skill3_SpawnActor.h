// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AbilitySystem/PTAbilitySystemComponent.h"
#include "Skill3_SpawnActor.generated.h"

struct FMappingEffectHandle
{
	FName ActorName;
	FActiveGameplayEffectHandle EffectHandle;
};

UCLASS()
class PROJECTT_API ASkill3_SpawnActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ASkill3_SpawnActor();
	void Tick(float DeltaTime) override;

	UPROPERTY(EditDefaultsOnly)
	class USceneComponent* DefaultRoot;
	UPROPERTY(EditDefaultsOnly)
	class UBoxComponent* BoxCollision;
	UPROPERTY(EditDefaultsOnly)
	class UParticleSystemComponent* Particle;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UGameplayEffect> Skill3Effect;

	TArray<FMappingEffectHandle> RemoveEffectHandles;

	UPROPERTY(EditDefaultsOnly)
	float MoveSpeed = 300.f;

protected:
	virtual void BeginPlay() override;
	

public:	
	UFUNCTION()
	void BoxOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void BoxOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

};
