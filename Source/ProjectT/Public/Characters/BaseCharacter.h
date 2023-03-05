// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"


class UInputMappingContext;
class UInputAction;
class ABaseItem;

UCLASS()
class PROJECTT_API ABaseCharacter : public ACharacter
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
	/** Inventory */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UInventoryComponent* InventoryComponent;
	void OnInventoryPressed();
	/** Interaction */
	void OnInteractionPressed();
	UFUNCTION(Server, Reliable)
	void ServerDestryoItem(ABaseItem* Item);

	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_OverlappingItems)
	TArray<ABaseItem*> OverlappingItems;
	UFUNCTION()
	void OnRep_OverlappingItems();

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> PickupWidgetClass;
	UPROPERTY()
	UUserWidget* PickupWidget;
};
