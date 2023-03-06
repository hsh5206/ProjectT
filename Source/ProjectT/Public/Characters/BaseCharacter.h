// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GameplayEffectTypes.h"
#include "BaseCharacter.generated.h"

class UInputMappingContext;
class UInputAction;
class ABaseItem;

class UPTAbilitySystemComponent;
class UPTAttributeSet;
class UPTGameplayAbility;

UCLASS()
class PROJECTT_API ABaseCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ABaseCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void PostInitializeComponents() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual void BeginPlay() override;
	virtual void PawnClientRestart() override;

	UPROPERTY(EditDefaultsOnly)
	class UCameraComponent* Camera;
	UPROPERTY(EditDefaultsOnly)
	class USpringArmComponent* SpringArm;

	UPROPERTY()
	class APlayerController* PlayerController;

public:	
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

	/** Inventory */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UInventoryComponent* InventoryComponent;
	void OnInventoryPressed();

	/** Combat */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UCombatComponent* CombatComponent;

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

protected:
	virtual void InitializeAttributes();
	virtual void GiveAbilities();
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

	/** Effect */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "GAS")
	TSubclassOf<class UGameplayEffect> DefaultAttributeEffect;

	/** Ability */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "GAS")
	TArray<TSubclassOf<UPTGameplayAbility>> DefaultsAbilities;
};
