// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CharacterTypes.h"
#include "TigrealAnimInstance.generated.h"


class ATigreal;
class UCharacterMovementComponent;
/**
 */
UCLASS()
class SLASH_API UTigrealAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaTime) override;


	UPROPERTY(BlueprintReadOnly);
	TObjectPtr<ATigreal> Tigreal;

	UPROPERTY(BlueprintReadOnly, Category = Movement);
	TObjectPtr<UCharacterMovementComponent> TigrealMovement;

	UPROPERTY(BlueprintReadOnly, Category = Movement);
	float GroundSpeed;

	UPROPERTY(BlueprintReadOnly, Category = Movement);
	bool IsFalling;

	UPROPERTY(BlueprintReadOnly, Category = "Movement | CharacterState");
	ETigrealState TigrealState;

};
