#pragma once

#include "CoreMinimal.h"
#include "../Characters/BaseCharacter.h"
#include "../Characters/CharacterTypes.h"
#include "Enemy.generated.h"

UCLASS()
class SLASH_API AEnemy : public ABaseCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();

	UPROPERTY(EditAnywhere, Category = Enemy)
	TSubclassOf<class AWeapon> WeaponClass;

	UPROPERTY(EditAnywhere, Category = Enemy)
	class UStaticMesh* WeaponMesh;

	UPROPERTY(EditAnywhere)
	class UHealthBarComponent* HealthBarComponent;

	UPROPERTY(EditAnywhere)
	class UPawnSensingComponent* PawnSensing;

	UPROPERTY()
	AActor* CombatTarget;

	UPROPERTY(EditANywhere)
	double CombatRadius = 500.f;

	UPROPERTY(EditANywhere)
	double AttackRadius = 150.f;

	class AAIController* EnemyController;

	FTimerHandle PatrolTimer;

	UPROPERTY(BlueprintReadWrite)
	EEnemyState EnemyState = EEnemyState::EES_Patrolling;

	UPROPERTY(BlueprintReadOnly)
	EDeathPose DeathPose;

	// Navigation
	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	AActor* PatrolTarget; // Current patrol target

	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	TArray<AActor*> PatrolTargets;

	UPROPERTY(EditAnywhere)
	double InRangeRadius = 200.f;

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void CheckCombatTarget();

	virtual void GetHit_Implementation(const FVector& ImpactPoint) override;

	virtual int32 PlayDeathMontage() override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	bool InTargetRange(AActor* Target, double AcceptanceRadius);

	AActor* ChoosePatrolTarget();
	void MoveToTarget(AActor* Target);
	void PatrolTimerFinished();

	UFUNCTION()
	void PawnSeen(APawn* SeenPawn);

	virtual void Destroyed() override;

	virtual void Attack() override;

	void LooseInterest();
	void StartPatrolling();
	void ChaseTarget();

	FTimerHandle AttackTimer;
	void StartAttackTimer();
};
