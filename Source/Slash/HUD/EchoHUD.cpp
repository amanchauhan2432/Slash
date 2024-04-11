// Fill out your copyright notice in the Description page of Project Settings.


#include "EchoHUD.h"
#include "EchoWidget.h"

void AEchoHUD::BeginPlay()
{
	Super::BeginPlay();

	if (EchoWidgetClass)
	{
		EchoWidget = CreateWidget<UEchoWidget>(GetWorld()->GetFirstPlayerController(), EchoWidgetClass);
		EchoWidget->AddToViewport();
	}
}
