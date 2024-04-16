// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Treasure.generated.h"

UCLASS()
class SLASH_API ATreasure : public AItem
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATreasure();

	class AEcho* Player;

	UPROPERTY(EditAnywhere, Category = Score)
	int32 GoldValue = 5;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void OnSphereOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	

};
