// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "Tigreal.generated.h"


class AItem;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
class USpringArmComponent;
class UCameraComponent;
class UAnimMontage;
class USlashOverlay;

UCLASS()
class SLASH_API ATigreal : public ABaseCharacter
{
	GENERATED_BODY()

public:
	ATigreal();
	virtual void Tick(float DeltaTime) override;
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
protected:
	virtual void BeginPlay() override;
	virtual void Jump() override;
	void Move(const FInputActionValue& Value); 
	void Look(const FInputActionValue& Value);
	void Zoom(const FInputActionValue& Value);
	void FPressed(const FInputActionValue& Value);
	virtual void Attack() override ;
	virtual void Die(EDeathPose PossibleDeathPose, const FName& SectionName) override;
	void PlayArmMontage(FName SectionName);
	void UpdateHealth(float DamageAmount);
	virtual bool CanAttack() override{
		return (CanMove() && TigrealState != ETigrealState::ECS_Unequipped);
	}
	void InitializeSlashOverlay();
	bool CanMove() { return ActionState == EActionState::EAS_UnOccupied && LandState == ELandState::ELC_Unlocked; }
	bool CanDisarm() { return TigrealState != ETigrealState::ECS_Unequipped && ActionState == EActionState::EAS_UnOccupied; }
	bool CanArm() { return TigrealState == ETigrealState::ECS_Unequipped && ActionState == EActionState::EAS_UnOccupied && EquippedWeapon; }



	UFUNCTION(BlueprintCallable)
	void DisarmWeapon();

	UFUNCTION(BlueprintCallable)
	void ArmWeapon();

	virtual void EndAttack() override { if (ActionState != EActionState::EAS_UnOccupied) ActionState = EActionState::EAS_UnOccupied;	LandState = ELandState::ELC_Unlocked; }

	UFUNCTION(BlueprintCallable)
	void LandLockMove() { if(LandState != ELandState::ELS_Locked) LandState = ELandState::ELS_Locked; }

	UFUNCTION(BlueprintCallable)
	void LandUnlockMove() { if(LandState != ELandState::ELC_Unlocked) LandState = ELandState::ELC_Unlocked; }

	UFUNCTION(BlueprintCallable)
	void UnlockEquipMove() { if(ActionState != EActionState::EAS_UnOccupied) ActionState = EActionState::EAS_UnOccupied; }

	UFUNCTION(BlueprintCallable)
	void HitReactEnd();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input);
	TObjectPtr<UInputMappingContext> TigrealMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input);
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input);
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input);
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input);
	TObjectPtr<UInputAction> ZoomAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input);
	TObjectPtr<UInputAction> EquipAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input);
	TObjectPtr<UInputAction> AttackAction;

private:
	UPROPERTY(VisibleAnywhere);
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere);
	TObjectPtr<UCameraComponent> Camera;

	UPROPERTY(VisibleInstanceOnly);
	TObjectPtr<AItem> OverlappingItem;

	ETigrealState TigrealState = ETigrealState::ECS_Unequipped;
	EActionState ActionState = EActionState::EAS_UnOccupied;
	ELandState LandState = ELandState::ELC_Unlocked;

	UPROPERTY(EditDefaultsOnly, Category = Montages);
	TObjectPtr<UAnimMontage> ArmMontage;

	UPROPERTY()
		USlashOverlay* SlashOverlay;

public:
	FORCEINLINE void SetOverlappingItem(AItem* Item) { OverlappingItem = Item; }
	FORCEINLINE ETigrealState GetTigrealState(void) const { return TigrealState;  }
	FORCEINLINE EDeathPose GetTigrealDeathPose(void) const { return DeathPose; }

};
