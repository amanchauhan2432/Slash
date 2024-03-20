#include "Enemy.h"
#include "Components/CapsuleComponent.h"
#include "../Components/AttributeComponent.h"
#include "../HUD/HealthBarComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AIController.h"
#include "Perception/PawnSensingComponent.h"
#include "../Items/Weapons/Weapon.h"

AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	HealthBarComponent = CreateDefaultSubobject<UHealthBarComponent>(TEXT("HealthBarComponent"));
	HealthBarComponent->SetupAttachment(GetRootComponent());

	PawnSensing = CreateDefaultSubobject<UPawnSensingComponent>("PawnSensing");
	PawnSensing->SetPeripheralVisionAngle(45.f);
	PawnSensing->SightRadius = 4000.f;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->MaxWalkSpeed = 150.f;
	bUseControllerRotationYaw = false;
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	Tags.Add(FName("Enemy"));

	if (HealthBarComponent)
	{
		HealthBarComponent->SetHealthPercent(Attribute->GetHealthPercent());
		HealthBarComponent->SetVisibility(false);
	}

	EnemyController = Cast<AAIController>(GetController());
	MoveToTarget(PatrolTarget);

	if (PawnSensing)
	{
		PawnSensing->OnSeePawn.AddDynamic(this, &AEnemy::PawnSeen);
	}

	if (WeaponClass && WeaponMesh)
	{
		AWeapon* MainWeapon = GetWorld()->SpawnActor<AWeapon>(WeaponClass);
		MainWeapon->EquipSound = nullptr;
		MainWeapon->Equip(GetMesh(), FName("RightHandSocket"), this, this);
		MainWeapon->ItemMesh->SetStaticMesh(WeaponMesh);
		EquippedWeapon = MainWeapon;
	}
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (EnemyState == EEnemyState::EES_Dead) return;

	if (EnemyState > EEnemyState::EES_Patrolling)
	{
		CheckCombatTarget();
	}
	else
	{
		if (InTargetRange(PatrolTarget, InRangeRadius))
		{
			PatrolTarget = ChoosePatrolTarget();
			GetWorldTimerManager().SetTimer(PatrolTimer, this, &AEnemy::PatrolTimerFinished, FMath::RandRange(4.f, 7.f));
		}
	}
}

void AEnemy::CheckCombatTarget()
{
	if (!InTargetRange(CombatTarget, CombatRadius))
	{
		GetWorldTimerManager().ClearTimer(AttackTimer);
		LooseInterest();
		if (EnemyState != EEnemyState::EES_Engaged)
		{
			StartPatrolling();
		}
	}
	else if (!InTargetRange(CombatTarget, AttackRadius) && EnemyState != EEnemyState::EES_Chasing)
	{
		GetWorldTimerManager().ClearTimer(AttackTimer);
		if (EnemyState != EEnemyState::EES_Engaged)
		{
			ChaseTarget();
		}
	}
	else if (InTargetRange(CombatTarget, AttackRadius) && EnemyState != EEnemyState::EES_Attacking && EnemyState != EEnemyState::EES_Engaged)
	{
		StartAttackTimer();
	}
}

void AEnemy::GetHit_Implementation(const FVector& ImpactPoint, AActor* OtherActor)
{
	Super::GetHit_Implementation(ImpactPoint, OtherActor);

	if (HealthBarComponent && EnemyState != EEnemyState::EES_Dead)
	{
		HealthBarComponent->SetVisibility(true);
	}
	GetWorldTimerManager().ClearTimer(PatrolTimer);
	GetWorldTimerManager().ClearTimer(AttackTimer);
	SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);

	if (AttackMontage && GetMesh()->GetAnimInstance())
	{
		GetMesh()->GetAnimInstance()->Montage_Stop(0.25f, AttackMontage);
	}
}

int32 AEnemy::PlayDeathMontage()
{
	int32 RandomSection = Super::PlayDeathMontage();

	DeathPose = StaticCast<EDeathPose>(RandomSection);
	EnemyState = EEnemyState::EES_Dead;
	GetWorldTimerManager().ClearTimer(AttackTimer);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);
	SetLifeSpan(7.f);
	if (HealthBarComponent)
	{
		HealthBarComponent->SetVisibility(false);
	}
	return 0;
}

float AEnemy::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	if (Attribute && HealthBarComponent)
	{
		Attribute->ReceiveDamage(DamageAmount);
		HealthBarComponent->SetHealthPercent(Attribute->GetHealthPercent());
	}
	CombatTarget = EventInstigator->GetPawn();

	if (InTargetRange(CombatTarget, AttackRadius)) //Inside attack radius
	{
		EnemyState = EEnemyState::EES_Attacking;
	}
	else if (!InTargetRange(CombatTarget, AttackRadius)) // Outside attack radius
	{
		ChaseTarget();
	}
	
	return DamageAmount;
}

bool AEnemy::InTargetRange(AActor* Target, double AcceptanceRadius)
{
	if (Target == nullptr) return false;
	const double DistanceToTarget = (Target->GetActorLocation() - GetActorLocation()).Size();
	return DistanceToTarget <= AcceptanceRadius;
}

AActor* AEnemy::ChoosePatrolTarget()
{
	TArray<AActor*> ValidTargets;
	for (AActor* Target : PatrolTargets)
	{
		if (PatrolTarget != Target)
		{
			ValidTargets.AddUnique(Target);
		}
	}
	if (PatrolTargets.Num() > 0)
	{
		return ValidTargets[FMath::RandRange(0, ValidTargets.Num() - 1)];
	}

	return nullptr;
}

void AEnemy::MoveToTarget(AActor* Target)
{
	if (!EnemyController || !Target) return;
	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalActor(Target);
	MoveRequest.SetAcceptanceRadius(30.f);
	EnemyController->MoveTo(MoveRequest);
}

void AEnemy::PatrolTimerFinished()
{
	MoveToTarget(PatrolTarget);
}

void AEnemy::PawnSeen(APawn* SeenPawn)
{
	if (EnemyState != EEnemyState::EES_Chasing &&
		EnemyState != EEnemyState::EES_Dead &&
		EnemyState < EEnemyState::EES_Attacking &&
		SeenPawn->ActorHasTag(FName("EngageableTarget")))
	{
		CombatTarget = SeenPawn;
		GetWorldTimerManager().ClearTimer(PatrolTimer);
		ChaseTarget();
	}
}

void AEnemy::Destroyed()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->Destroy();
	}
}

void AEnemy::Attack()
{
	Super::Attack();

	PlayAttackMontage();

	EnemyState = EEnemyState::EES_Engaged;
}

void AEnemy::LooseInterest()
{
	CombatTarget = nullptr;
	if (HealthBarComponent)
	{
		HealthBarComponent->SetVisibility(false);
	}
}

void AEnemy::StartPatrolling()
{
	EnemyState = EEnemyState::EES_Patrolling;
	GetCharacterMovement()->MaxWalkSpeed = 150.f;
	MoveToTarget(PatrolTarget);
}

void AEnemy::ChaseTarget()
{
	EnemyState = EEnemyState::EES_Chasing;
	GetCharacterMovement()->MaxWalkSpeed = 300.f;
	MoveToTarget(CombatTarget);
}

void AEnemy::StartAttackTimer()
{
	EnemyState = EEnemyState::EES_Attacking;
	const float AttackTime = FMath::RandRange(0.1f, 0.5f);
	GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemy::Attack, AttackTime);
}
