// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Enemies/Enemy.h"
#include "Components/CapsuleComponent.h"
#include "DbgFunctions.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Attributes.h"
#include "HUD/HealthBarComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AIController.h"
#include "Perception/PawnSensingComponent.h"


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
	PawnSensing = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("Pawn Sensing"));
	PawnSensing->SightRadius = 4000.f;
	PawnSensing->SetPeripheralVisionAngle(60.f);
	GetCharacterMovement()->MaxWalkSpeed = 150;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	HealthBarWidget->SetVisibility(false);
	EnemyController = Cast<AAIController>(GetController());
	GoToRandomPatrol();
	if (PawnSensing)
	{
		PawnSensing->OnSeePawn.AddDynamic(this, &AEnemy::PawnSeen);
	}
}

void AEnemy::GoToRandomPatrol()
{
	if (PatrolTargets.Num() > 1)
	{
		int32 RandIndex = FMath::RandRange(0, PatrolTargets.Num() - 1);
		if (PatrolTarget == PatrolTargets[RandIndex])
		{
			RandIndex = (RandIndex!=0) ? RandIndex - 1 : RandIndex + 1;
		}
		PatrolTarget = PatrolTargets[RandIndex];
		GetWorldTimerManager().SetTimer(PatrolTimer, this, &AEnemy::PatrolTimerFinished, 5.f);

	}
}

void AEnemy::GoToActor(AActor* Target, double AcceptanceRadius = 15.f)
{
	if (EnemyController && Target)
	{
		FAIMoveRequest MoveRequest;
		MoveRequest.SetGoalActor(Target);
		MoveRequest.SetAcceptanceRadius(AcceptanceRadius);
		EnemyController->MoveTo(MoveRequest);
	}
}


void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if(CurrentState != EEnemyState::EES_Patrolling)
		CheckCombat();
	else
		CheckPatrol();
}

void AEnemy::CheckCombat()
{
	if (CombatTarget)
	{
		if (!IsInRange(CombatTarget, SeeRadius))
		{
			HealthBarWidget->SetVisibility(false);
			CombatTarget = nullptr;
			CurrentState = EEnemyState::EES_Patrolling;
			GetCharacterMovement()->MaxWalkSpeed = 125.f;
			GoToActor(PatrolTarget);
			UE_LOG(LogTemp, Warning, TEXT("Lose Interest"));
		}
		else if(!IsInRange(CombatTarget, AttackRadius) && CurrentState!=EEnemyState::EES_Chasing)
		{
			CurrentState = EEnemyState::EES_Chasing;
			GetCharacterMovement()->MaxWalkSpeed = 300.f;
			GoToActor(CombatTarget);
			UE_LOG(LogTemp, Warning, TEXT("Chase Player"));
		}
		else if (IsInRange(CombatTarget, AttackRadius) && CurrentState != EEnemyState::EES_Attacking)
		{
			CurrentState = EEnemyState::EES_Attacking;
			UE_LOG(LogTemp, Warning, TEXT("Attack"));
		}
	}
}

void AEnemy::CheckPatrol()
{
	if (PatrolTarget)
	{
		if (IsInRange(PatrolTarget, 200.f))
		{
			GoToRandomPatrol();
		}
	}
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

void AEnemy::PlayReactMontage(FName SectionName, EDeathPose PossibleDeathPose)
{
	if (Attributes && Attributes->IsAlive())
	{
		HealthBarWidget->SetVisibility(true);
		PlayHitReactMontage(SectionName);
	}
	else if (Attributes)
	{
		DeathPose = PossibleDeathPose;
		PlayDeathReactMontage(SectionName);
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		HealthBarWidget->SetVisibility(false);
		SetLifeSpan(10.f);
	}
}

bool AEnemy::IsInRange(AActor* Target, double Radius)
{
	if (!Target)
		return false;
	const double Distance = (Target->GetActorLocation() - GetActorLocation()).Size();
	return Distance < Radius;
}

void AEnemy::PatrolTimerFinished()
{
	GoToActor(PatrolTarget, 15.f);
}

void AEnemy::PawnSeen(APawn* SeenPawn)
{
	if (CurrentState == EEnemyState::EES_Chasing) return;
	if (SeenPawn->ActorHasTag(FName("Tigreal")))
	{
		GetWorldTimerManager().ClearTimer(PatrolTimer);
		GetCharacterMovement()->MaxWalkSpeed = 300.f;
		CombatTarget = SeenPawn;
		if (CurrentState != EEnemyState::EES_Attacking)
		{
			CurrentState = EEnemyState::EES_Chasing;
			GoToActor(CombatTarget);
			UE_LOG(LogTemp, Warning, TEXT("Pawn Seen, Chase Player"));
		}
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
	CombatTarget = EventInstigator->GetPawn();
	GetCharacterMovement()->MaxWalkSpeed = 300.f;
	if (CurrentState != EEnemyState::EES_Chasing && CombatTarget)
	{
		CurrentState = EEnemyState::EES_Chasing;
		GoToActor(CombatTarget);
	}

	return DamageAmount;
}

