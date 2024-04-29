// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "InputActionValue.h"
#include "CharacterTypes.h"
#include "Echo.generated.h"

UCLASS()
class SLASH_API AEcho : public ABaseCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEcho();

	UPROPERTY(VisibleAnywhere)
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere)
	class UCameraComponent* FollowCamera;

	UPROPERTY(EditAnywhere)
	class UGroomComponent* Hair;

	UPROPERTY(EditAnywhere)
	class UGroomComponent* Eyebrows;

	class AWeapon* OverlappingWeapon;

	ECharacterState CharacterState = ECharacterState::ECS_UnEquipped;

	UPROPERTY(BlueprintReadWrite)
	EActionState ActionState = EActionState::EAS_Unoccupied;

	UPROPERTY(EditAnywhere, Category = Animation)
	class UAnimMontage* ArmDisarmMontage;

	UPROPERTY(EditAnywhere, Category = Animation)
	class UAnimMontage* DodgeMontage;

	UPROPERTY()
	class UEchoWidget* EchoWidget;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AWeapon> WeaponClass;

	int32 TotalEnemies = 0;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void Movement(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);

	// Input
	UPROPERTY(EditAnywhere, Category = Input)
	class UInputMappingContext* EchoMappingContext;

	UPROPERTY(EditAnywhere, Category = Input)
	class UInputAction* MovementAction;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* EquipAction;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* AttackAction;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* DodgeAction;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual int32 PlayDeathMontage() override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* OtherActor) override;

	void EKeyPressed();
	virtual void Attack() override;
	void Dodge();

	void PlayArmDisarmMontage(FName SectionName);

	UFUNCTION(BlueprintCallable)
	void Arm();

	UFUNCTION(BlueprintCallable)
	void Disarm();

	void UpdateGoldText(int32 InValue);
	void UpdateExpText( int32 InValue);

};
