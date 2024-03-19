// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Item.h"
#include "Weapon.generated.h"

/**
 * 
 */
UCLASS()
class SLASH_API AWeapon : public AItem
{
	GENERATED_BODY()
	
public:
	AWeapon();

	class AEcho* Player;

	UPROPERTY(EditAnywhere, Category = Sound)
	class USoundBase* EquipSound;

	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* WeaponBox;

	UPROPERTY(VisibleAnywhere)
	class USceneComponent* BoxTraceStart;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* BoxTraceEnd;

	TArray<AActor*> IgnoreActors;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	float Damage  =  20.f;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	FVector BoxTraceExtent = FVector(5.f);

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	bool bShowBoxDebug = false;

protected:

	virtual void BeginPlay() override;

	virtual void OnSphereOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	virtual void OnSphereOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

	UFUNCTION()
	void OnBoxOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintImplementableEvent)
	void CreateFields(const FVector& FieldLocation);

public:
	void Equip(USceneComponent* InParent, FName InSocketName, AActor* NewOwner, APawn* NewInstigator);
	void AttachMeshToSocket(USceneComponent* InParent, FName InSocketName);

	void BoxTrace(FHitResult& BoxHit);

};
