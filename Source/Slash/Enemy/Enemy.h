#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../Interface/HitInterface.h"
#include "../Characters/CharacterTypes.h"
#include "Enemy.generated.h"

UCLASS()
class SLASH_API AEnemy : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();

	UPROPERTY(EditAnywhere, Category = Animation)
	UAnimMontage* HitReactMontage;

	UPROPERTY(EditAnywhere, Category = Animation)
	UAnimMontage* DeathMontage;

	UPROPERTY(EditAnywhere, Category = Sound)
	USoundBase* HitSound;

	UPROPERTY(EditAnywhere, Category = Particle)
	UParticleSystem* HitParticle;

	UPROPERTY(EditAnywhere)
	class UAttributeComponent* Attribute;

	UPROPERTY(EditAnywhere)
	class UHealthBarComponent* HealthBarComponent;

	UPROPERTY(EditAnywhere)
	class UPawnSensingComponent* PawnSensing; 

	UPROPERTY(BlueprintReadOnly)
	EDeathPose DeathPose = EDeathPose::EDP_Alive;

	UPROPERTY()
	AActor* CombatTarget;

	UPROPERTY(EditANywhere)
	double CombatRadius = 500.f;

	UPROPERTY(EditANywhere)
	double AttackRadius = 150.f;

	class AAIController* EnemyController;

	FTimerHandle PatrolTimer;

	EEnemyState EnemyState = EEnemyState::EES_Patrolling;

	// Navigation
	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	AActor* PatrolTarget; // Current patrol target

	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	TArray<AActor*> PatrolTargets;

	UPROPERTY(EditAnywhere)
	double InRangeRadius = 200.f;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void GetHit_Implementation(const FVector& ImpactPoint) override;

	void PlayHitReactMontage(const FName& SectionName);

	void PlayDeathMontage();

	void DirectionalHitReact(const FVector& ImpactPoint);

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	bool InTargetRange(AActor* Target, double AcceptanceRadius);

	AActor* ChoosePatrolTarget();
	void MoveToTarget(AActor* Target);
	void PatrolTimerFinished();

	UFUNCTION()
	void PawnSeen(APawn* SeenPawn);
};
