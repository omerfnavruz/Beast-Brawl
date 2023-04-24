// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CharacterTypes.h"
#include "Interfaces/HitInterface.h"
#include "BaseCharacter.generated.h"

UCLASS()
class SLASH_API ABaseCharacter : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:
	ABaseCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;
protected:
	virtual void BeginPlay() override;
	void DirectionalHitReact(const FVector& ImpactPoint);
	virtual void Attack();
	float CalculateHitAngle(const FVector& ImpactPoint);
	int32 PlayRandomMontageSection(UAnimMontage* Montage, const TArray<FName>& SectionNames);
	void PlayMontageSection(UAnimMontage* Montage, const FName& SectionName);
	virtual void PlayAttackMontage(const uint8 NumberOfAnimations);
	void StopAttackMontage();
	virtual void PlayHitReactMontage(FName SectionName);
	virtual void PlayDeathReactMontage(FName SectionName);
	void PlayReactMontage(FName SectionName, EDeathPose PossibleDeathPose);
	bool IsAlive();
	void SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled);
	virtual bool CanAttack() { return false; }
	virtual void Die(EDeathPose PossibleDeathPose, const FName& SectionName);
	void ShowBlood(const FVector& ImpactPoint);
	void PlayHitSound(const FVector& ImpactPoint);
	UFUNCTION(BlueprintCallable)
	void SetBoxTrace(ECollisionEnabled::Type CollisionEnabled);

	UFUNCTION(BlueprintCallable)
	virtual void EndAttack();

	UFUNCTION(BlueprintCallable)
	FVector GetTranslationWarpTarget();

	UFUNCTION(BlueprintCallable)
	FVector GetRotationWarpTarget();

	UPROPERTY(VisibleAnywhere, Category = Weapon);
	TObjectPtr<class AWeapon> EquippedWeapon;

	UPROPERTY(EditDefaultsOnly, Category = Montages);
	TObjectPtr<class UAnimMontage> AttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = Montages);
	TObjectPtr<UAnimMontage> HitReactMontage;

	UPROPERTY(EditDefaultsOnly, Category = Montages);
	TObjectPtr<UAnimMontage> DeathReactMontage;

	UPROPERTY(BlueprintReadOnly);
	EDeathPose DeathPose = EDeathPose::EDP_Alive;


	UPROPERTY(VisibleAnywhere, Category = Attributes);
	TObjectPtr<class UAttributes> Attributes;

	UPROPERTY(BlueprintReadOnly, Category = Combat);
	AActor* CombatTarget;

	UPROPERTY(EditAnywhere, Category = Combat);
	double WarpTargetDistance = 75.f;

	UPROPERTY(EditAnywhere, Category = Sounds);
	TObjectPtr<USoundBase> HitSound;

	UPROPERTY(EditAnywhere, Category = VisualEffects);
	TObjectPtr<UParticleSystem> HitParticles;
};
