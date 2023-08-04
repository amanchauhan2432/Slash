// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"


enum class EItemState : uint8
{
	EIS_Hovering UMETA(DisplayName = "Hovering"),
	EIS_Equipped UMETA(DisplayName = "Equipped")
};

UCLASS()
class SLASH_API AItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItem();

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* ItemMesh;

	UPROPERTY(EditAnywhere)
	class USphereComponent* Sphere;

	UPROPERTY(BlueprintReadOnly)
	float RunningTime;

	UPROPERTY(EditAnywhere, Category = Sine)
	float Amplitude = 0.25f;

	UPROPERTY(EditAnywhere, Category = Sine)
	float TimeConstant = 5.f;

	UPROPERTY(EditAnywhere)
	class UNiagaraComponent* EmbersEffect;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnSphereOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnSphereOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	EItemState ItemState = EItemState::EIS_Hovering;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
