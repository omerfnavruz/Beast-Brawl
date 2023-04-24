// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Weapon.generated.h"

class UBoxComponent;
/**
 * 
 */
UCLASS()
class SLASH_API AWeapon : public AItem
{
	GENERATED_BODY()
	

public:
	AWeapon();
	void Equip(USceneComponent* SceneComponent, FName Name, AActor* NewOwner, APawn* NewInvestigator);

	FORCEINLINE TObjectPtr<UBoxComponent> GetWeaponBox() const { return WeaponBoxComponent; }
	void AttachMeshToSocket(USceneComponent* SceneComponent, const FName& Name);

	TArray<AActor*> IgnoreActors;
protected:
	virtual void BeginPlay() override;
	void BoxTrace(FHitResult& HitResult);
	void DisableSphereCollision();
	void PlayShinkSound();


	UFUNCTION(BlueprintImplementableEvent)
		void CreateFields(const FVector& FieldLocation);

	UFUNCTION()
		virtual void OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	bool IsBothEnemy(AActor* OtherActor);

	void ApplyDamage(AActor* const& HitActor);

	void HitTargetActor(FHitResult& HitResult);

private:
	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	TObjectPtr<USoundBase> ShrinkSound;


	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	FVector BoxTraceExtent = FVector(5.f, 5.f, 5.f);

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	bool ShowDebug = false;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBoxComponent> WeaponBoxComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> TraceStart;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> TraceEnd;

	UPROPERTY(VisibleAnywhere);
	float Damage = 20.f;


};
