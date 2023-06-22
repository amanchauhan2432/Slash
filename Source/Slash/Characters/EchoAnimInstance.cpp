// Fill out your copyright notice in the Description page of Project Settings.


#include "EchoAnimInstance.h"
#include "Echo.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"

void UEchoAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Echo = Cast<AEcho>(TryGetPawnOwner());
}

void UEchoAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (Echo)
	{
		Speed = UKismetMathLibrary::VSizeXY(Echo->GetCharacterMovement()->Velocity);
		bIsFalling = Echo->GetCharacterMovement()->IsFalling();
	}
}
