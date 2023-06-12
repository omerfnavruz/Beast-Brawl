// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../BaseCharacter.h"
#include "Interfaces/HitInterface.h"
#include "Enemy.generated.h"

UCLASS()
class SLASH_API AEnemy : public ABaseCharacter
{
	GENERATED_BODY()

public:
	AEnemy();
	static void ResetCounters();
	static int32_t GetDeadCount();
protected:
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;
	void ShowHealthBar();
	virtual void Tick(float DeltaTime) override;
	bool IsDead();
	bool IsPatrolling();
	void CheckCombat();
	virtual bool CanAttack() override;
	bool IsEngaged();
	void ClearAttackTimer();
	bool InAttackRange();
	bool OutsideAttackRange();
	bool OutOfSight();
	void CheckPatrol();
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	void UpdateHealth(float DamageAmount);
	virtual void Destroyed() override;
	void StartAttackTimer();
	virtual void BeginPlay() override;
	void EquipWeapon();
	void GoToRandomPatrol();
	void GoToActor(AActor* Target, double AcceptanceRadius);
	virtual void Attack() override;
	virtual void EndAttack() override;
	void LoseInterest();
	void Patrol();
	virtual void Die (EDeathPose PossibleDeathPose, const FName& SectionName) override;
	void SpawnNewEnemy();
	bool IsInRange(AActor* Target, double Radius);
	void PatrolTimerFinished();
	
	FTimerHandle PatrolTimer;
	FTimerHandle AttackTimer;

	UPROPERTY(EditAnywhere, Category = Combat)
	float AttackMin = 0.5f;

	UPROPERTY(EditAnywhere, Category = Combat)
	float AttackMax = 1.f;

	UFUNCTION()
	void PawnSeen(APawn* SeenPawn);

	void Chase(APawn*& SeenPawn);

	void Chase();

	bool CanNotChase();



	UPROPERTY(VisibleAnywhere, Category = Attributes);
	TObjectPtr<class UHealthBarComponent> HealthBarWidget;

	UPROPERTY(EditAnywhere, Category = Attributes);
	double SeeRadius = 3000;

	UPROPERTY(EditAnywhere, Category = Attributes);
	double AttackRadius = 180.f;


	UPROPERTY()
	TObjectPtr<class AAIController> EnemyController;

	// Current patrol target
	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	TObjectPtr<AActor> PatrolTarget;

	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	TArray<TObjectPtr<AActor>> PatrolTargets;


	EEnemyState CurrentState = EEnemyState::EES_Patrolling;

private:
	UPROPERTY(EditAnywhere);
	TArray<TSubclassOf<AEnemy>> EnemyToBeSpawned;

	UPROPERTY(VisibleAnywhere);
	TObjectPtr<class UPawnSensingComponent> PawnSensing;

	UPROPERTY(EditAnywhere);
	TSubclassOf<class AWeapon> WeaponClass;

	FVector InitialPosition;
	FRotator InitialRotation;

	static int deadCount;
	static int aliveCount;

	
};
