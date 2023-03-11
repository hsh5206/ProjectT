// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GameplayEffectTypes.h"
#include "BaseCharacter.generated.h"

class UPTAbilitySystemComponent;
class UPTAttributeSet;
class UPTGameplayAbility;

UCLASS()
class PROJECTT_API ABaseCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ABaseCharacter();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bIsDead = false;

protected:
	virtual void BeginPlay() override;

public:
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void ServerPlayMontage(UAnimMontage* Montage, FName SectionName = FName("Default"));
	UFUNCTION(NetMulticast, Reliable)
	void MulticastPlayMontage(UAnimMontage* Montage, FName SectionName);
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void ServerStopMontage(UAnimMontage* Montage);
	UFUNCTION(NetMulticast, Reliable)
	void MulticastStopMontage(UAnimMontage* Montage);
	/** Death */
	UFUNCTION(Server, Reliable)
	void ServerDeath();
	UFUNCTION(NetMulticast, Reliable)
	void MulticastDeath();
	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* DeathMontage;
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite)
	TSubclassOf<class ABaseItem> SpawnItemAfterDead;

/**
*
*	GAS
*
*/
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	UPTAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	UPTAttributeSet* Attributes;

public:
	virtual void InitializeAttributes();
	virtual void GiveAbilities();
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

	/** Effect */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "GAS")
	TArray<TSubclassOf<class UGameplayEffect>> DefaultAttributeEffects;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "GAS")
	TSubclassOf<class UGameplayEffect> MoveEffect;
	FActiveGameplayEffectHandle MoveEffectHandle;

	/** Tags */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "GAS")
	FGameplayTagContainer CantMoveTag;

	/** Ability */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "GAS")
	TArray<TSubclassOf<UPTGameplayAbility>> DefaultsAbilities;

	/** Event */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "GAS")
	FGameplayTag BasicAttackEventTag;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "GAS")
	FGameplayTag HitEventTag;
};
