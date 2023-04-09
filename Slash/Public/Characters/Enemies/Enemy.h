// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
#include "Characters/CharacterTypes.h"
#include "Enemy.generated.h"

class UAnimMontage;
UCLASS()
class SLASH_API AEnemy : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:
	AEnemy();
	virtual void GetHit_Implementation(const FVector& ImpactPoint) override;

	void DirectionalHitReact(const FVector& ImpactPoint);
	virtual void Tick(float DeltaTime) override;
	void CheckCombat();
	void CheckPatrol();
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
protected:
	virtual void BeginPlay() override;
	void GoToRandomPatrol();
	void GoToActor(AActor* Target, double AcceptanceRadius);
	void PlayHitReactMontage(FName SectionName);
	void PlayDeathReactMontage(FName SectionName);
	void PlayReactMontage(FName SectionName, EDeathPose PossibleDeathPose);
	bool IsInRange(AActor* Target, double Radius);

	FTimerHandle PatrolTimer;
	void PatrolTimerFinished();

	UFUNCTION()
	void PawnSeen(APawn* SeenPawn);


	UPROPERTY(VisibleAnywhere, Category = Attributes);
	TObjectPtr<class UAttributes> Attributes;
	UPROPERTY(VisibleAnywhere, Category = Attributes);
	TObjectPtr<class UHealthBarComponent> HealthBarWidget;

	UPROPERTY(EditAnywhere, Category = Attributes);
	double SeeRadius = 1500;

	UPROPERTY(EditAnywhere, Category = Attributes);
	double AttackRadius = 150;

	UPROPERTY(BlueprintReadOnly);
	EDeathPose DeathPose = EDeathPose::EDP_Alive;


	UPROPERTY()
	TObjectPtr<class AAIController> EnemyController;

	// Current patrol target
	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	TObjectPtr<AActor> PatrolTarget;

	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	TArray<TObjectPtr<AActor>> PatrolTargets;

	EEnemyState CurrentState = EEnemyState::EES_Patrolling;

private:
	UPROPERTY(EditDefaultsOnly, Category = Montages);
	TObjectPtr<UAnimMontage> HitReactMontage;

	UPROPERTY(EditDefaultsOnly, Category = Montages);
	TObjectPtr<UAnimMontage> DeathReactMontage;

	UPROPERTY(EditAnywhere, Category = Sounds);
	TObjectPtr<USoundBase> HitSound;

	UPROPERTY(EditAnywhere, Category = VisualEffects);
	TObjectPtr<UParticleSystem> HitParticles;

	UPROPERTY();
	TObjectPtr<AActor> CombatTarget = nullptr;

	UPROPERTY(VisibleAnywhere);
	TObjectPtr<class UPawnSensingComponent> PawnSensing;



};
