// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Enemies/Enemy.h"
#include "Components/CapsuleComponent.h"
#include "DbgFunctions.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Attributes.h"
#include "HUD/HealthBarComponent.h"

// Sets default values
AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	const auto EnemyMesh = GetMesh();
	EnemyMesh->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	EnemyMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECR_Block);
	EnemyMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECR_Ignore);
	EnemyMesh->SetGenerateOverlapEvents(true);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECR_Ignore);
	Attributes = CreateDefaultSubobject<UAttributes>(TEXT("Attributes"));
	HealthBarWidget = CreateDefaultSubobject<UHealthBarComponent>(TEXT("Health Bar"));
	HealthBarWidget->SetupAttachment(GetRootComponent());

}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	
}

void AEnemy::PlayHitReactMontage(FName SectionName)
{
	auto const  AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HitReactMontage)
	{
		AnimInstance->Montage_Play(HitReactMontage);
		AnimInstance->Montage_JumpToSection(SectionName);

	}
}

void AEnemy::PlayDeathReactMontage(FName SectionName)
{
	auto const  AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && DeathReactMontage)
	{
		AnimInstance->Montage_Play(DeathReactMontage);
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

void AEnemy::PlayReactMontage(FName SectionName)
{
	if (Attributes && Attributes->IsAlive())
	{
		PlayHitReactMontage(SectionName);
	}
	else
	{
		PlayDeathReactMontage(SectionName);
	}
}

void AEnemy::GetHit_Implementation(const FVector& ImpactPoint)
{
	//DbgDrawer::SphereAtLoc(GetWorld(), ImpactPoint);
	
	DirectionalHitReact(ImpactPoint);
	if (HitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			HitSound,
			ImpactPoint
		);
	}

	if (HitParticles && GetWorld())
	{
		UGameplayStatics::SpawnEmitterAtLocation(this, HitParticles, ImpactPoint);
	}
}

void AEnemy::DirectionalHitReact(const FVector& ImpactPoint)
{
	const auto ImpactPointLow = FVector(ImpactPoint.X, ImpactPoint.Y, GetActorLocation().Z);
	const auto Forward = GetActorForwardVector();
	const auto ToHit = (ImpactPointLow - GetActorLocation()).GetSafeNormal();

	const auto CosTeta = FVector::DotProduct(Forward, ToHit);
	auto Teta = FMath::RadiansToDegrees(FMath::Acos(CosTeta));

	const auto CrossProduct = FVector::CrossProduct(Forward, ToHit);
	Teta = CrossProduct.Z < 0 ? -Teta : Teta;

	if (Teta >= -45.f && Teta < 45.f)
	{
		PlayReactMontage(FName("FromFront"));
	}

	else if (Teta >= 45.f && Teta < 135.f)
	{
		PlayReactMontage(FName("FromLeft"));
	}

	else if (Teta >= -135.f && Teta < -45.f)
	{
		PlayReactMontage(FName("FromRight"));
	}

	else
	{
		PlayReactMontage(FName("FromBack"));
	}
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (Attributes && HealthBarWidget)
	{
		Attributes->ReceiveDamage(DamageAmount);
		HealthBarWidget->SetHealthPercent(Attributes->GetHealthPercent());
	}
	return DamageAmount;
}

