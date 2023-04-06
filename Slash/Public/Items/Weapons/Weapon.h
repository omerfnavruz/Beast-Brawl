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
	void Equip(USceneComponent* SceneComponent, FName Name);
	void AttachMeshToSocket(USceneComponent* SceneComponent, const FName& Name);
	FORCEINLINE TObjectPtr<UBoxComponent> GetWeaponBox() const { return WeaponBoxComponent; }

	TArray<AActor*> IgnoreActors;
protected:
	virtual void BeginPlay() override;
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	virtual void EndSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;
	UFUNCTION()
	virtual void OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintImplementableEvent)
	void CreateFields(const FVector& FieldLocation);
private:
	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	TObjectPtr<USoundBase> ShrinkSound;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBoxComponent> WeaponBoxComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> TraceStart;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> TraceEnd;

};
