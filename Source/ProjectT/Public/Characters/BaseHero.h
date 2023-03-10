// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseCharacter.h"
#include "BaseHero.generated.h"

class UInputMappingContext;
class UInputAction;
class ABaseItem;

UCLASS()
class PROJECTT_API ABaseHero : public ABaseCharacter
{
	GENERATED_BODY()
	
public:
	ABaseHero();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostInitializeComponents() override;

protected:
	virtual void BeginPlay() override;
	virtual void PawnClientRestart() override;

	UPROPERTY(EditDefaultsOnly)
	class UCameraComponent* Camera;
	UPROPERTY(EditDefaultsOnly)
	class USpringArmComponent* SpringArm;

	UPROPERTY()
	class APlayerController* PlayerController;

	/** Enhanced Input */
	UPROPERTY(EditDefaultsOnly)
	class UInputMappingContext* DefaultMappingContext;
	UPROPERTY(EditDefaultsOnly)
	UInputAction* MoveInputAction;
	UPROPERTY(EditDefaultsOnly)
	UInputAction* JumpInputAction;
	UPROPERTY(EditDefaultsOnly)
	UInputAction* InventoryAction;
	UPROPERTY(EditDefaultsOnly)
	UInputAction* Interaction;
	UPROPERTY(EditDefaultsOnly)
	UInputAction* BasicAttackAction;
	UPROPERTY(EditDefaultsOnly)
	UInputAction* Skill_1_Action;
	UPROPERTY(EditDefaultsOnly)
	UInputAction* Skill_2_Action;
	UPROPERTY(EditDefaultsOnly)
	UInputAction* Skill_3_Action;
	UPROPERTY(EditDefaultsOnly)
	UInputAction* Skill_4_Action;

public:
	/** Move */
	bool bIsFirst = false;
	bool bIsMovePressed = false;
	void OnMoveAction();
	void OnMoveActionEnd();
	/** Jump */
	void OnJumpAction();
	void OnJumpActionEnd();
	virtual void Landed(const FHitResult& Hit) override;
	/** Interaction */
	void OnInteractionPressed();
	UFUNCTION(Server, Reliable)
	void ServerDestryoItem(ABaseItem* Item);
	/** BasicAttack */
	void BasicAttack();
	UFUNCTION(BlueprintCallable)
	void AttackStartComboState();
	UFUNCTION(BlueprintCallable)
	void AttackEndComboState();
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void ServerAttackStartComboState();
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void ServerAttackEndComboState();
	/** Skill */
	UFUNCTION()
	void Skill_1();
	UFUNCTION(BlueprintCallable)
	void Skill_1_DamageEvent();
	UFUNCTION()
	void Skill_2();
	UFUNCTION(BlueprintCallable)
	void Skill_2_DamageEvent();
	UFUNCTION()
	void Skill_3();
	UFUNCTION(BlueprintCallable)
	void Skill_3_DamageEvent();
	UPROPERTY(EditDefaultsOnly, Category = "Skills")
	TSubclassOf<AActor> Skill3Actor;
	UFUNCTION()
	void Skill_4();
	UFUNCTION(BlueprintCallable)
	void Skill_4_DamageEvent();

	/** Combat */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UCombatComponent* CombatComponent;
	UFUNCTION(Server, Reliable)
	void ServerSetActorRotationToMousePointer(FRotator Rotation);
	UFUNCTION(NetMulticast, Reliable)
	void MulticastSetActorRotationToMousePointer(FRotator Rotation);
	/** Combo Attack */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = Attack, Meta = (AllowPrivateAccess = "true"))
	bool CanNextCombo = true;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = "true"))
	int32 MaxCombo = 3;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = "true"))
	int32 CurrentCombo = 0;
	UFUNCTION(Server, Reliable)
	void ServerComboCombatStateChanged();

	/** Inventory */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UInventoryComponent* InventoryComponent;
	void OnInventoryPressed();
	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_OverlappingItems)
	TArray<ABaseItem*> OverlappingItems;
	UFUNCTION()
	void OnRep_OverlappingItems();

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> PickupWidgetClass;
	UPROPERTY()
	UUserWidget* PickupWidget;

	UPROPERTY(EditDefaultsOnly, Category = Rune)
	class UStaticMeshComponent* Rune1;
	UPROPERTY(EditDefaultsOnly, Category = Rune)
	class UStaticMeshComponent* Rune2;
	UPROPERTY(EditDefaultsOnly, Category = Rune)
	class UStaticMeshComponent* Rune3;

	/** Effect */
	UPROPERTY(EditDefaultsOnly, Category="GAS")
	TSubclassOf<UGameplayEffect> SKill1Effect;
};
