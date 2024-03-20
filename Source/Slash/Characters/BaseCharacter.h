#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../Interface/HitInterface.h"
#include "BaseCharacter.generated.h"

UCLASS()
class SLASH_API ABaseCharacter : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:
	ABaseCharacter();
	virtual void Tick(float DeltaTime) override;

	class AWeapon* EquippedWeapon;

	UPROPERTY(EditAnywhere, Category = Animation)
	class UAnimMontage* AttackMontage;

	UPROPERTY(EditAnywhere, Category = Animation)
	TArray<FName> AttackSectionNames;

	UPROPERTY(EditAnywhere, Category = Animation)
	UAnimMontage* DeathMontage;

	UPROPERTY(EditAnywhere, Category = Animation)
	TArray<FName> DeathSectionNames;

	UPROPERTY(EditAnywhere, Category = Animation)
	UAnimMontage* HitReactMontage;

	UPROPERTY(BlueprintReadOnly)
	AActor* CombatTarget;

	UPROPERTY(EditAnywhere)
	class UAttributeComponent* Attribute;

	UPROPERTY(EditAnywhere, Category = Combat)
	USoundBase* HitSound;

	UPROPERTY(EditAnywhere, Category = Combat)
	UParticleSystem* HitParticle;

protected:
	virtual void BeginPlay() override;

public:

	UFUNCTION(BlueprintCallable)
	void SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled);

	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* OtherActor);

	virtual void Attack();

	virtual void PlayAttackMontage();

	virtual int32 PlayDeathMontage();

	void PlayHitReactMontage(const FName& SectionName);

	void DirectionalHitReact(const FVector& ImpactPoint);

	UFUNCTION(BlueprintCallable)
	FVector GetTranslationWarpTarget();

	UFUNCTION(BlueprintCallable)
	FVector GetRotationWarpTarget();
};
