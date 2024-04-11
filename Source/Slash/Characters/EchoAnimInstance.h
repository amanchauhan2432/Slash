// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CharacterTypes.h"
#include "EchoAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class SLASH_API UEchoAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	
	virtual void NativeInitializeAnimation() override;

	virtual void NativeUpdateAnimation(float DeltaTime) override;

	class AEcho* Echo;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	float Speed;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	bool bIsFalling;

	UPROPERTY(BlueprintReadOnly, Category = "Movement | CharacterState")
	ECharacterState CharacterState;

	UPROPERTY(BlueprintReadOnly, Category = "Movement | CharacterState")
	EActionState ActionState;

	UPROPERTY(BlueprintReadOnly, Category = "Movement | CharacterState")
	EDeathPose DeathPose;
};
