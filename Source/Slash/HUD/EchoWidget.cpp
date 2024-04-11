// Fill out your copyright notice in the Description page of Project Settings.


#include "EchoWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UEchoWidget::SetHealthbar(float InPercent)
{
	if (HealthBar)
	{
		HealthBar->SetPercent(InPercent);
	}
}

void UEchoWidget::SetStaminaBar(float InPercent)
{
	if (StaminaBar)
	{
		StaminaBar->SetPercent(InPercent);
	}
}

void UEchoWidget::SetGoldText(int32 InText)
{
	FText Text = FText::FromString(FString::FromInt(InText));
	GoldText->SetText(Text);
}

void UEchoWidget::SetExpText(int32 InText)
{
	FText Text = FText::FromString(FString::FromInt(InText));
	ExpText->SetText(Text);
}
