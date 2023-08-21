#include "Enemy.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "../Components/AttributeComponent.h"
#include "../HUD/HealthBarComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AIController.h"
#include "Perception/PawnSensingComponent.h"

AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	Attribute = CreateDefaultSubobject<UAttributeComponent>(TEXT("Attribute"));

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
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (EnemyState > EEnemyState::EES_Patrolling)
	{
		if (!InTargetRange(CombatTarget, CombatRadius))
		{
			CombatTarget = nullptr;
			if (HealthBarComponent)
			{
				HealthBarComponent->SetVisibility(false);
			}
			
			// Out of radius then loose interest
			EnemyState = EEnemyState::EES_Patrolling;
			GetCharacterMovement()->MaxWalkSpeed = 150.f;
			MoveToTarget(PatrolTarget);
		}
		else if (!InTargetRange(CombatTarget, AttackRadius) && EnemyState != EEnemyState::EES_Chasing)
		{
			// Out of attack range, chase
			EnemyState = EEnemyState::EES_Chasing;
			GetCharacterMovement()->MaxWalkSpeed = 300.f;
			MoveToTarget(CombatTarget);
		}
		else if (InTargetRange(CombatTarget, AttackRadius) && EnemyState != EEnemyState::EES_Attacking)
		{
			// In attack range, attack
			EnemyState = EEnemyState::EES_Attacking;
		}
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

void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::GetHit_Implementation(const FVector& ImpactPoint)
{
	if (HealthBarComponent)
	{
		HealthBarComponent->SetVisibility(true);
	}
	if (Attribute && Attribute->IsAlive())
	{
		DirectionalHitReact(ImpactPoint);
	}
	else
	{
		PlayDeathMontage();
	}

	if (HitSound && HitParticle)
	{
		UGameplayStatics::PlaySoundAtLocation(this, HitSound, ImpactPoint);
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticle, ImpactPoint);
	}
}

void AEnemy::PlayHitReactMontage(const FName& SectionName)
{
	if (HitReactMontage)
	{
		GetMesh()->GetAnimInstance()->Montage_Play(HitReactMontage);
		GetMesh()->GetAnimInstance()->Montage_JumpToSection(SectionName, HitReactMontage);
	}
}

void AEnemy::PlayDeathMontage()
{
	if (DeathMontage)
	{
		GetMesh()->GetAnimInstance()->Montage_Play(DeathMontage);

		int32 Selection = FMath::RandRange(1, 6);
		FString SectionName = FString::Printf(TEXT("Death%d"), Selection);
		DeathPose = static_cast<EDeathPose>(Selection);
		GetMesh()->GetAnimInstance()->Montage_JumpToSection(*SectionName, DeathMontage);
	}

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetLifeSpan(7.f);
	if (HealthBarComponent)
	{
		HealthBarComponent->SetVisibility(false);
	}
}

void AEnemy::DirectionalHitReact(const FVector& ImpactPoint)
{
	const FVector Forward = GetActorForwardVector();
	const FVector ToHit = (ImpactPoint - GetActorLocation()).GetSafeNormal();
	const double CosTheta = FVector::DotProduct(Forward, ToHit);
	double Theta = FMath::Acos(CosTheta);
	Theta = FMath::RadiansToDegrees(Theta);

	const FVector CrossProduct = FVector::CrossProduct(Forward, ToHit);
	if (CrossProduct.Z < 0)
	{
		Theta *= -1.f;
	}

	FName SectionName("Back");
	if (Theta >= -45.f && Theta < 45)
	{
		SectionName = FName("Front");
	}
	else if (Theta >= -135.f && Theta < -45.f)
	{
		SectionName = FName("Left");
	}
	else if (Theta >= 45.f && Theta < 135.f)
	{
		SectionName = FName("Right");
	}
	PlayHitReactMontage(SectionName);
}

float AEnemy::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	if (Attribute && HealthBarComponent)
	{
		Attribute->ReceiveDamage(DamageAmount);
		HealthBarComponent->SetHealthPercent(Attribute->GetHealthPercent());
	}
	CombatTarget = EventInstigator->GetPawn();

	EnemyState = EEnemyState::EES_Attacking;
	MoveToTarget(CombatTarget);
	GetCharacterMovement()->MaxWalkSpeed = 300.f;
	
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
	if (EnemyState == EEnemyState::EES_Chasing) return;

	if (SeenPawn->ActorHasTag(FName("EchoCharacter")))
	{
		GetWorldTimerManager().ClearTimer(PatrolTimer);
		GetCharacterMovement()->MaxWalkSpeed = 300.f;
		CombatTarget = SeenPawn;
		if (EnemyState != EEnemyState::EES_Attacking)
		{
			EnemyState = EEnemyState::EES_Chasing;
			MoveToTarget(CombatTarget);
		}
	}
}
