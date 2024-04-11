// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "EchoHUD.generated.h"

/**
 * 
 */
UCLASS()
class SLASH_API AEchoHUD : public AHUD
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UEchoWidget> EchoWidgetClass;

	UPROPERTY()
	UEchoWidget* EchoWidget;

protected:
	virtual void BeginPlay() override;
};
