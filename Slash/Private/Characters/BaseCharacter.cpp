// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/BaseCharacter.h"
#include "Components/BoxComponent.h"
#include "Items/Weapons/Weapon.h"
#include "Components/Attributes.h"
#include "Kismet/GameplayStatics.h"
// Sets default values
ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	Attributes = CreateDefaultSubobject<UAttributes>(TEXT("Attributes"));
}


void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABaseCharacter::DirectionalHitReact(const FVector& ImpactPoint)
{
	const auto Teta = CalculateHitAngle(ImpactPoint);

	if (Teta >= -60.f && Teta < 60.f)
	{
		PlayReactMontage(FName("FromFront"), EDeathPose::EDP_FromFront);
	}
	 
	else if (Teta >= 60.f && Teta < 120.f)
	{
		PlayReactMontage(FName("FromLeft"), EDeathPose::EDP_FromLeft);
	}

	else if (Teta >= -120.f && Teta < -60.f)
	{
		PlayReactMontage(FName("FromRight"), EDeathPose::EDP_FromRight);
	}

	else
	{
		PlayReactMontage(FName("FromBack"), EDeathPose::EDP_FromBack);
	}
}

// Called every frame
void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseCharacter::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	if (Hitter)
	{
		SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);
		PlayHitSound(ImpactPoint);
		ShowBlood(ImpactPoint);
		DirectionalHitReact(ImpactPoint);
	}

}


void ABaseCharacter::Die(EDeathPose PossibleDeathPose, const FName& SectionName)
{
}

void ABaseCharacter::SetBoxTrace(ECollisionEnabled::Type CollisionEnabled)
{
	if (EquippedWeapon)
	{
		if (const auto Box = EquippedWeapon->GetWeaponBox())
		{
			Box->SetCollisionEnabled(CollisionEnabled);
			if(CollisionEnabled == ECollisionEnabled::NoCollision) EquippedWeapon->IgnoreActors.Empty();
		}
	}

}

void ABaseCharacter::ShowBlood(const FVector& ImpactPoint)
{
	if (HitParticles && GetWorld())
	{
		UGameplayStatics::SpawnEmitterAtLocation(this, HitParticles, ImpactPoint);
	}
}

void ABaseCharacter::PlayHitSound(const FVector& ImpactPoint)
{
	if (HitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			HitSound,
			ImpactPoint
		);
	}
}
void ABaseCharacter::EndAttack()
{
}

void ABaseCharacter::Attack()
{
}


float ABaseCharacter::CalculateHitAngle(const FVector& ImpactPoint)
{
	const auto ImpactPointLow = FVector(ImpactPoint.X, ImpactPoint.Y, GetActorLocation().Z);
	const auto Forward = GetActorForwardVector();
	const auto ToHit = (ImpactPointLow - GetActorLocation()).GetSafeNormal();

	const auto CosTeta = FVector::DotProduct(Forward, ToHit);
	auto Teta = FMath::RadiansToDegrees(FMath::Acos(CosTeta));

	const auto CrossProduct = FVector::CrossProduct(Forward, ToHit);
	Teta = CrossProduct.Z < 0 ? -Teta : Teta;
	return Teta;
}

int32 ABaseCharacter::PlayRandomMontageSection(UAnimMontage* Montage, const TArray<FName>& SectionNames)
{
	if (SectionNames.Num() <= 0) return -1;
	const int32 MaxSectionIndex = SectionNames.Num() - 1;
	const int32 Selection = FMath::RandRange(0, MaxSectionIndex);
	PlayMontageSection(Montage, SectionNames[Selection]);
	return Selection;
}


void ABaseCharacter::PlayMontageSection(UAnimMontage* Montage, const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && Montage)
	{
		AnimInstance->Montage_Play(Montage);
		AnimInstance->Montage_JumpToSection(SectionName, Montage);
	}
}

void ABaseCharacter::PlayAttackMontage(const uint8 NumberOfAnimations)
{
	auto const  AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && AttackMontage)
	{
		AnimInstance->Montage_Play(AttackMontage);
		const auto AnimSelection = FMath::RandRange(1, NumberOfAnimations);
		FString SectionString = FString::Printf(TEXT("Attack%d"), AnimSelection);
		FName SectionName;
		SectionName = FName(SectionString);
		AnimInstance->Montage_JumpToSection(SectionName);

	}
}


void ABaseCharacter::StopAttackMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->Montage_Stop(0.25f, AttackMontage);
	}
}

void ABaseCharacter::PlayHitReactMontage(FName SectionName)
{
	PlayMontageSection(HitReactMontage, SectionName);
}

void ABaseCharacter::PlayDeathReactMontage(FName SectionName)
{
	PlayMontageSection(DeathReactMontage, SectionName);
}

void ABaseCharacter::PlayReactMontage(FName SectionName, EDeathPose PossibleDeathPose)
{
	if (IsAlive())
	{
		PlayHitReactMontage(SectionName);
	}
	else if (Attributes)
	{
		Die(PossibleDeathPose, SectionName);
	}
}
bool ABaseCharacter::IsAlive()
{
	return Attributes->IsAlive() && Attributes;
}


FVector ABaseCharacter::GetTranslationWarpTarget()
{
	if (CombatTarget == nullptr) return FVector();

	const FVector CombatTargetLocation = CombatTarget->GetActorLocation();
	const FVector Location = GetActorLocation();

	FVector TargetToMe = (Location - CombatTargetLocation).GetSafeNormal();
	TargetToMe *= WarpTargetDistance;

	return CombatTargetLocation + TargetToMe;

}

FVector ABaseCharacter::GetRotationWarpTarget()
{
	if (CombatTarget)
	{
		return CombatTarget->GetActorLocation();
	}
	return FVector();
}

void ABaseCharacter::SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled)
{
	if (EquippedWeapon && EquippedWeapon->GetWeaponBox())
	{
		EquippedWeapon->GetWeaponBox()->SetCollisionEnabled(CollisionEnabled);
		EquippedWeapon->IgnoreActors.Empty();
	}
}