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
#include "Items/Weapons/Weapon.h"

int AEnemy::deadCount = 0;
int AEnemy::aliveCount = 0;

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

void AEnemy::ResetCounters()
{
	deadCount = 0;
	aliveCount = 0;
}

int32_t AEnemy::GetDeadCount()
{
	return deadCount;
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	InitialPosition = GetActorLocation();
	InitialRotation = GetActorRotation();
	Tags.Add(FName("Enemy"));
	HealthBarWidget->SetVisibility(false);
	EnemyController = Cast<AAIController>(GetController());
	GoToRandomPatrol();
	if (PawnSensing)
	{
		PawnSensing->OnSeePawn.AddDynamic(this, &AEnemy::PawnSeen);
	}

	EquipWeapon();
	aliveCount++;
	aliveCount = aliveCount > 0 ? aliveCount : 1;
}

void AEnemy::EquipWeapon()
{
	auto World = GetWorld();
	if (World && WeaponClass)
	{
		auto DefaultWeapon = World->SpawnActor<AWeapon>(WeaponClass);
		DefaultWeapon->Equip(GetMesh(), FName("RightHandSocket"), this, this);
		EquippedWeapon = DefaultWeapon;
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

void AEnemy::GoToActor(AActor* Target, double AcceptanceRadius = 10.f)
{
	if (EnemyController && Target)
	{
		FAIMoveRequest MoveRequest;
		MoveRequest.SetGoalActor(Target);
		MoveRequest.SetAcceptanceRadius(AcceptanceRadius);
		EnemyController->MoveTo(MoveRequest);
	}
}

void AEnemy::Attack()
{
	Super::Attack();
	CurrentState = EEnemyState::EES_Engaged;
	PlayAttackMontage(4);
	//UE_LOG(LogTemp, Warning, TEXT("Attack"));
}

void AEnemy::EndAttack()
{
	ClearAttackTimer();
	if (IsDead() || !IsAlive())
		return;
	CurrentState = EEnemyState::EES_NoState;
	CheckCombat();
}


void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (IsDead()) return;
	if (IsPatrolling())
		CheckPatrol();
	else
		CheckCombat();
}

bool AEnemy::IsDead()
{
	return CurrentState == EEnemyState::EES_Dead;
}



bool AEnemy::IsPatrolling()
{
	return CurrentState == EEnemyState::EES_Patrolling;
}

void AEnemy::CheckCombat()
{
	if (CombatTarget)
	{
		if (OutOfSight())
		{
			ClearAttackTimer();
			LoseInterest();
		}
		else if (OutsideAttackRange())
		{
			ClearAttackTimer();
			Chase();
		}
		else if (CanAttack())
		{
			StartAttackTimer();
		}
	}
}

bool AEnemy::CanAttack()
{
	return InAttackRange() && CurrentState != EEnemyState::EES_Attacking && CurrentState != EEnemyState::EES_Dead && !IsEngaged();
}

bool AEnemy::IsEngaged()
{
	return CurrentState == EEnemyState::EES_Engaged;
}

void AEnemy::ClearAttackTimer()
{
	GetWorldTimerManager().ClearTimer(AttackTimer);
}

bool AEnemy::InAttackRange()
{
	return IsInRange(CombatTarget, AttackRadius);
}

bool AEnemy::OutsideAttackRange()
{
	return !IsInRange(CombatTarget, AttackRadius);
}

bool AEnemy::OutOfSight()
{
	return !IsInRange(CombatTarget, SeeRadius);
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

void AEnemy::LoseInterest()
{
	HealthBarWidget->SetVisibility(false);
	CombatTarget = nullptr;
	Patrol();
	//UE_LOG(LogTemp, Warning, TEXT("Lose Interest"));
}
void AEnemy::Patrol()
{
	CurrentState = EEnemyState::EES_Patrolling;
	GetCharacterMovement()->MaxWalkSpeed = 150.f;
	GoToActor(PatrolTarget);
}
void AEnemy::Die(EDeathPose PossibleDeathPose, const FName& SectionName)
{
	//UE_LOG(LogTemp, Warning, TEXT("State:%d"), (int)CurrentState);
	if (IsDead())
		return;
	ClearAttackTimer();
	SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);
	CurrentState = EEnemyState::EES_Dead;
	DeathPose = PossibleDeathPose;
	PlayDeathReactMontage(SectionName);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HealthBarWidget->SetVisibility(false);
	SetLifeSpan(10.f);
	aliveCount--;
	aliveCount = aliveCount < 0 ? 0 : aliveCount;
	deadCount++;
	SpawnNewEnemy();
}

void AEnemy::SpawnNewEnemy()
{
	int spawnNum = deadCount / 3 + 1;
	auto World = GetWorld();
	//UE_LOG(LogTemp, Warning, TEXT("SpawnNum: %d DeadCound:%d AliveCount: %d World:%d"), spawnNum, deadCount, aliveCount, (bool)World);
	if (aliveCount == 0 && World)
	{
		for(int i=0; i<spawnNum; i++)
			World->SpawnActor<AEnemy>(EnemyToBeSpawned[0], InitialPosition, InitialRotation);
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
		Chase(SeenPawn);
	}
}

void AEnemy::Chase(APawn*& SeenPawn)
{
	if (CurrentState == EEnemyState::EES_Chasing || CurrentState == EEnemyState::EES_Attacking) return;
	CombatTarget = SeenPawn;
	if (OutOfSight()) return;
	Chase();
}

void AEnemy::Chase()
{
	if (CanNotChase())
	{
		return;
	}
	GetCharacterMovement()->MaxWalkSpeed = 300.f;
	CurrentState = EEnemyState::EES_Chasing;
	GoToActor(CombatTarget);
	//UE_LOG(LogTemp, Warning, TEXT("Chase Player"));
	
}

bool AEnemy::CanNotChase()
{
	return CurrentState == EEnemyState::EES_Chasing || CurrentState == EEnemyState::EES_Dead || !(CombatTarget) || IsEngaged();
}

void AEnemy::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	GetWorldTimerManager().ClearTimer(PatrolTimer);
	GetWorldTimerManager().ClearTimer(AttackTimer);
	StopAttackMontage();
	Super::GetHit_Implementation(ImpactPoint, Hitter);
	ShowHealthBar();
	SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);
	EndAttack();
}

void AEnemy::ShowHealthBar()
{
	if (!IsDead()) HealthBarWidget->SetVisibility(true);
}


float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (IsDead())
		return 0;
	UpdateHealth(DamageAmount);
	CombatTarget = EventInstigator->GetPawn();
	GetWorldTimerManager().ClearTimer(AttackTimer);
	if (InAttackRange())
	{
		CurrentState = EEnemyState::EES_Attacking;
	}
	else
	{
		Chase();
	}
	return DamageAmount;
}

void AEnemy::UpdateHealth(float DamageAmount)
{
	if (Attributes && HealthBarWidget)
	{
		Attributes->ReceiveDamage(DamageAmount);
		HealthBarWidget->SetHealthPercent(Attributes->GetHealthPercent());
	}
}


void AEnemy::Destroyed()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->Destroy();
	}
}

void AEnemy::StartAttackTimer()
{
	CurrentState = EEnemyState::EES_Attacking;
	const float AttackTime = FMath::RandRange(AttackMin, AttackMax);
	GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemy::Attack, AttackTime);
}