// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EchoWidget.generated.h"


UCLASS()
class SLASH_API UEchoWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetHealthbar(float InPercent);
	void SetStaminaBar(float InPercent);

	void SetGoldText(int32 InText);
	void SetExpText(int32 InText);

private:
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HealthBar;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* StaminaBar;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* GoldText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ExpText;
};
