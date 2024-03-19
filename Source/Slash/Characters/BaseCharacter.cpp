#include "BaseCharacter.h"
#include "../Items/Weapons/Weapon.h"
#include "Components/BoxComponent.h"
#include "../Components/AttributeComponent.h"

ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	Attribute = CreateDefaultSubobject<UAttributeComponent>(TEXT("Attribute"));
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseCharacter::SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled)
{
	if (EquippedWeapon && EquippedWeapon->WeaponBox)
	{
		EquippedWeapon->WeaponBox->SetCollisionEnabled(CollisionEnabled);
		EquippedWeapon->IgnoreActors.Empty();
	}
}

void ABaseCharacter::Attack()
{
}

void ABaseCharacter::PlayAttackMontage()
{
	if (GetMesh()->GetAnimInstance() && AttackSectionNames.Num() > 0)
	{
		GetMesh()->GetAnimInstance()->Montage_Play(AttackMontage);
		const int32 RandomSection = FMath::RandRange(0, AttackSectionNames.Num() - 1);
		GetMesh()->GetAnimInstance()->Montage_JumpToSection(AttackSectionNames[RandomSection]);
	}
}

int32 ABaseCharacter::PlayDeathMontage()
{
	if (GetMesh()->GetAnimInstance() && DeathSectionNames.Num() > 0)
	{
		GetMesh()->GetAnimInstance()->Montage_Play(DeathMontage);
		const int32 RandomSection = FMath::RandRange(0, DeathSectionNames.Num() - 1);
		GetMesh()->GetAnimInstance()->Montage_JumpToSection(DeathSectionNames[RandomSection]);
		return RandomSection;
	}
	return 0;
}

void ABaseCharacter::PlayHitReactMontage(const FName& SectionName)
{
	if (HitReactMontage)
	{
		GetMesh()->GetAnimInstance()->Montage_Play(HitReactMontage);
		GetMesh()->GetAnimInstance()->Montage_JumpToSection(SectionName, HitReactMontage);
	}
}

void ABaseCharacter::DirectionalHitReact(const FVector& ImpactPoint)
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
