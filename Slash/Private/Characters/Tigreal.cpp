// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Tigreal.h"
#include "EnhancedInputSubsystems.h"
#include <EnhancedInputComponent.h>
#include "InputActionValue.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Math/UnrealMathUtility.h"
#include "Items/Item.h"
#include "Items/Weapons/Weapon.h"
#include "Animation/AnimMontage.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/Attributes.h"

// Sets default values
ATigreal::ATigreal()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);
	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Block);
	GetMesh()->SetGenerateOverlapEvents(true);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Block);
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 300.f;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(CameraBoom);
	DeathPose = EDeathPose::EDP_Alive;
}

// Called when the game starts or when spawned
void ATigreal::BeginPlay()
{
	Super::BeginPlay();
	Tags.Add(FName("Tigreal"));
	GetMesh()->HideBoneByName(TEXT("sword_bottom"), EPhysBodyOp::PBO_None);
	GetMesh()->HideBoneByName(TEXT("shield_inner"), EPhysBodyOp::PBO_None);
	if (auto PlayerController = Cast<APlayerController>(GetController()))
	{
		if (auto Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(TigrealMappingContext, 0);
		}
	}
}

void ATigreal::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	Super::GetHit_Implementation(ImpactPoint, Hitter);
	UE_LOG(LogTemp, Warning, TEXT("Tigreal Got Hit!!"));
	ActionState = EActionState::EAS_HitReaction;
}

float ATigreal::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Attributes->ReceiveDamage(DamageAmount);
	return DamageAmount;
}

void ATigreal::Jump()
{
	if (LandState == ELandState::ELC_Unlocked)
	{
		Super::Jump();
	}

	//UE_LOG(LogTemp, Warning, TEXT("Jump!!"));
}

void ATigreal::Move(const FInputActionValue& Value)
{
	if (!CanMove())
	{
		return;
	}
	
	const auto MovementVector = Value.Get<FInputActionValue::Axis2D>();

	const auto ControlRotation = GetControlRotation();
	const FRotator YawRotation(0.f, ControlRotation.Yaw, 0.f);
	const auto DirectionX = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const auto DirectionY = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	AddMovementInput(DirectionX, MovementVector.Y);
	AddMovementInput(DirectionY, MovementVector.X);


	//const auto Forward = GetActorForwardVector();
	//const auto Right = GetActorRightVector();
	//AddMovementInput(Forward, MovementVector.Y);
	//AddMovementInput(Right, MovementVector.X);
}

void ATigreal::Look(const FInputActionValue& Value)
{
	const auto DirectionValue = Value.Get<FInputActionValue::Axis2D>();
	if (GetController())
	{
		AddControllerPitchInput(DirectionValue.Y);
		AddControllerYawInput(DirectionValue.X);

	}
}

void ATigreal::Zoom(const FInputActionValue& Value)
{
	//UE_LOG(LogTemp, Warning, TEXT("Wheel: %f"), Value.Get<FInputActionValue::Axis1D>());
	//auto tempLen = CameraBoom->TargetArmLength;
	//auto tempTarget = tempLen - 50 * Value.Get<FInputActionValue::Axis1D>();
	CameraBoom->TargetArmLength -= 50.f*Value.Get<FInputActionValue::Axis1D>();
	//CameraBoom->TargetArmLength = FMath::Lerp(tempLen, tempTarget, 0.1);
}

void ATigreal::FPressed(const FInputActionValue& Value)
{
	auto OverlappingWeapon = Cast<AWeapon>(OverlappingItem);
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Cyan, FString("FPressed"));
	}
	if (OverlappingWeapon)
	{
		OverlappingWeapon->Equip(GetMesh(), FName("hand_rSocket"), this, this);
		TigrealState = ETigrealState::ECS_OneHandEquipped;
		EquippedWeapon = OverlappingWeapon;
		OverlappingItem = nullptr;
	}
	else
	{
		if (CanDisarm())
		{
			PlayArmMontage(FName("DisarmWeapon"));
			TigrealState = ETigrealState::ECS_Unequipped;
			ActionState = EActionState::EAS_Equipping;
		}
		else if (CanArm())
		{
			PlayArmMontage(FName("ArmWeapon"));
			TigrealState = ETigrealState::ECS_OneHandEquipped;
			ActionState = EActionState::EAS_Equipping;
		}
	}
}

void ATigreal::Attack()
{
	if (CanAttack())
	{
		PlayAttackMontage(2);
		ActionState = EActionState::EAS_Attacking;
		LandState = ELandState::ELC_Unlocked;
	}
}

void ATigreal::Die(EDeathPose PossibleDeathPose, const FName& SectionName)
{
	DeathPose = PossibleDeathPose;
	PlayDeathReactMontage(SectionName);
}


void ATigreal::PlayArmMontage(FName SectionName)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Cyan, FString("Playing Arm Montage"));
	}
	auto const  AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && ArmMontage)
	{
		AnimInstance->Montage_Play(ArmMontage);
		AnimInstance->Montage_JumpToSection(SectionName);

	}
}

void ATigreal::DisarmWeapon()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("BackSocket"));
	} 
}

void ATigreal::ArmWeapon()
{
	if (EquippedWeapon) 
	{
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("hand_rSocket"));
	}
}

void ATigreal::HitReactEnd()
{
	ActionState = EActionState::EAS_UnOccupied;
}



// Called every frame
void ATigreal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ATigreal::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);


	if (auto p = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		p->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ATigreal::Move);
		p->BindAction(LookAction, ETriggerEvent::Triggered, this, &ATigreal::Look);
		p->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ATigreal::Jump);
		p->BindAction(ZoomAction, ETriggerEvent::Triggered, this, &ATigreal::Zoom);
		p->BindAction(EquipAction, ETriggerEvent::Triggered, this, &ATigreal::FPressed);
		p->BindAction(AttackAction, ETriggerEvent::Triggered, this, &ATigreal::Attack);
	}

}

