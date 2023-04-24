// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/TigrealAnimInstance.h"
#include "Characters/Tigreal.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"

void UTigrealAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	Tigreal = Cast<ATigreal>(TryGetPawnOwner());
	if (Tigreal)
	{
		TigrealMovement = Tigreal->GetCharacterMovement();
	}
}

void UTigrealAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);
	if (TigrealMovement)
	{
		GroundSpeed = UKismetMathLibrary::VSizeXY(TigrealMovement->Velocity);
		IsFalling = TigrealMovement->IsFalling();
		TigrealState = Tigreal->GetTigrealState();
		DeathPose = Tigreal->GetTigrealDeathPose();
	}
}
