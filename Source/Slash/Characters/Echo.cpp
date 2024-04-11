// Fill out your copyright notice in the Description page of Project Settings.


#include "Echo.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/CapsuleComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GroomComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "EnhancedInputComponent.h"
#include "../Items/Weapons/Weapon.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "../HUD/EchoHUD.h"
#include "../HUD/EchoWidget.h"
#include "../Components/AttributeComponent.h"

// Sets default values
AEcho::AEcho()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);

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

	Tags.Add(FName("EngageableTarget"));
	
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(EchoMappingContext, 0);
		}
		EchoWidget = Cast<AEchoHUD>(PlayerController->GetHUD())->EchoWidget;

		if (EchoWidget)
		{
			EchoWidget->SetHealthbar(1.f);
			EchoWidget->SetStaminaBar(1.f);
			EchoWidget->SetGoldText(0);
			EchoWidget->SetExpText(0);
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

int32 AEcho::PlayDeathMontage()
{
	int32 RandomSection = Super::PlayDeathMontage();

	DeathPose = StaticCast<EDeathPose>(RandomSection);
	ActionState = EActionState::EAS_Dead;
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);
	SetLifeSpan(7.f);

	return 0;
}

float AEcho::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	
	if (EchoWidget)
	{
		EchoWidget->SetHealthbar(Attribute->GetHealthPercent());
	}
	return DamageAmount;
}

void AEcho::GetHit_Implementation(const FVector& ImpactPoint, AActor* OtherActor)
{
	Super::GetHit_Implementation(ImpactPoint, OtherActor);

	SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);
	if (Attribute && Attribute->Health > 0.f)
	{
		ActionState = EActionState::EAS_HitReact;
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
		OverlappingWeapon->Equip(GetMesh(), FName("WeaponSocket"), this, this);
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