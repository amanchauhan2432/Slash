// Fill out your copyright notice in the Description page of Project Settings.


#include "Echo.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GroomComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "EnhancedInputComponent.h"
#include "../Items/Weapons/Weapon.h"
#include "Components/BoxComponent.h"

// Sets default values
AEcho::AEcho()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 300.f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom);

	Hair = CreateDefaultSubobject<UGroomComponent>(TEXT("Hair"));
	Hair->SetupAttachment(GetMesh());
	Hair->AttachmentName = FString("head");

	Eyebrows = CreateDefaultSubobject<UGroomComponent>(TEXT("Eyebrows"));
	Eyebrows->SetupAttachment(GetMesh());
	Eyebrows->AttachmentName = FString("head");
}

// Called when the game starts or when spawned
void AEcho::BeginPlay()
{
	Super::BeginPlay();
	
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(EchoMappingContext, 0);
		}
	}
}


// Called every frame
void AEcho::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEcho::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MovementAction, ETriggerEvent::Triggered, this, &AEcho::Movement);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AEcho::Look);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(EquipAction, ETriggerEvent::Started, this, &AEcho::EKeyPressed);
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &AEcho::Attack);
	}
}

void AEcho::Movement(const FInputActionValue& Value)
{
	if (ActionState != EActionState::EAS_Unoccupied) return;
	if (Controller)
	{
		const FRotator Rotation{ 0.f, GetControlRotation().Yaw, 0.f };

		// Forward / Backward Movement
		const FVector ForwardDirection = FRotationMatrix(Rotation).GetUnitAxis(EAxis::X);
		AddMovementInput(ForwardDirection, Value.Get<FVector2D>().Y);

		// Right / Left Movement
		const FVector RightDirection = FRotationMatrix(Rotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(RightDirection, Value.Get<FVector2D>().X);
	}
}

void AEcho::Look(const FInputActionValue& Value)
{
	if (ActionState != EActionState::EAS_Unoccupied) return;
	if (Controller)
	{
		// Yaw Rotation (mouse X movement)
		AddControllerYawInput(Value.Get<FVector2D>().X);

		// Pitch Rotation (mouse Y movement)
		AddControllerPitchInput(Value.Get<FVector2D>().Y);
	}
}

void AEcho::EKeyPressed()
{	
	if (OverlappingWeapon)
	{
		EquippedWeapon = OverlappingWeapon;
		OverlappingWeapon->Equip(GetMesh(), FName("WeaponSocket"));
		CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
		OverlappingWeapon = nullptr;
	}
	else
	{
		if (ActionState == EActionState::EAS_Unoccupied && CharacterState != ECharacterState::ECS_UnEquipped)
		{
			PlayArmDisarmMontage(FName("Disarm"));
			CharacterState = ECharacterState::ECS_UnEquipped;
			ActionState = EActionState::EAS_EquippingWeapon;
		}
		else if (ActionState == EActionState::EAS_Unoccupied && CharacterState == ECharacterState::ECS_UnEquipped && EquippedWeapon)
		{
			PlayArmDisarmMontage(FName("Arm"));
			CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
			ActionState = EActionState::EAS_EquippingWeapon;
		}
	}
}

void AEcho::Attack()
{
	if (CharacterState == ECharacterState::ECS_EquippedOneHandedWeapon && ActionState == EActionState::EAS_Unoccupied)
	{
		PlayAttackMontage();
		ActionState = EActionState::EAS_Attacking;
	}
}

void AEcho::PlayAttackMontage()
{
	if (AttackMontage)
	{
		GetMesh()->GetAnimInstance()->Montage_Play(AttackMontage);
		FName SectionName;
		switch (FMath::RandRange(0, 4))
		{
		case 0:
			SectionName = FName("Attack1");
			break;
		case 1:
			SectionName = FName("Attack2");
			break;
		case 2:
			SectionName = FName("Attack3");
			break;
		case 3:
			SectionName = FName("Attack4");
			break;
		case 4:
			SectionName = FName("Attack5");
			break;
		default:
			SectionName = FName("Attack1");
			break;
		}
		GetMesh()->GetAnimInstance()->Montage_JumpToSection(SectionName, AttackMontage);
	}
}

void AEcho::PlayArmDisarmMontage(FName SectionName)
{
	if (ArmDisarmMontage)
	{
		GetMesh()->GetAnimInstance()->Montage_Play(ArmDisarmMontage);
		GetMesh()->GetAnimInstance()->Montage_JumpToSection(SectionName, ArmDisarmMontage);
	}
}

void AEcho::Arm()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("WeaponSocket"));
	}
}

void AEcho::Disarm()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("SpineSocket"));
	}
}

void AEcho::SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled)
{
	if (EquippedWeapon && EquippedWeapon->WeaponBox)
	{
		EquippedWeapon->WeaponBox->SetCollisionEnabled(CollisionEnabled);
		EquippedWeapon->IgnoreActors.Empty();
	}
}