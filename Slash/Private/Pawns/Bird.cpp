// Fill out your copyright notice in the Description page of Project Settings.


#include "Pawns/Bird.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputSubsystems.h"
#include <EnhancedInputComponent.h>
#include "InputActionValue.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

// Sets default values
ABird::ABird()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Skeletal Mesh"));
	SetRootComponent(Capsule);
	SkeletalMesh->SetupAttachment(GetRootComponent());
	
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 300.f;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(CameraBoom);

	AutoPossessPlayer = EAutoReceiveInput::Player0;


}

// Called when the game starts or when spawned
void ABird::BeginPlay()
{
	Super::BeginPlay();
	
	
	if (auto PlayerController = Cast<APlayerController>(GetController()))
	{
		if (auto Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(BirdMappingContext, 0);
		}
	}

}

void ABird::Move(const FInputActionValue& Value)
{
	const auto DirectionValue = Value.Get<float>();
	
	if (Controller && DirectionValue != 0)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Value: %f"), DirectionValue);
		auto Forward = GetActorForwardVector();
		AddMovementInput(Forward, DirectionValue);
	}

}

void ABird::Look(const FInputActionValue& Value)
{
	const auto DirectionValue = Value.Get<FInputActionValue::Axis2D>();
	if (GetController())
	{
		AddControllerPitchInput(DirectionValue.Y);
		AddControllerYawInput(DirectionValue.X);

	}
}
// Called every frame
void ABird::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	

}

// Called to bind functionality to input
void ABird::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (auto p = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		p->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ABird::Move);
		p->BindAction(LookAction, ETriggerEvent::Triggered, this, &ABird::Look);
	}

}

