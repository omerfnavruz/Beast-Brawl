// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
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

protected:
	virtual void BeginPlay() override;
	void PlayHitReactMontage(FName SectionName);

private:
	UPROPERTY(EditDefaultsOnly, Category = Montages);
	TObjectPtr<UAnimMontage> HitReactMontage;

	UPROPERTY(EditAnywhere, Category = Sounds);
	TObjectPtr<USoundBase> HitSound;

	UPROPERTY(EditAnywhere, Category = VisualEffects);
	TObjectPtr<UParticleSystem> HitParticles;
public:	
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};