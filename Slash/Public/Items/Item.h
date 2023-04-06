// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"
class USphereComponent;

enum class EItemState
{
	EIS_Hovering,
	EIS_Equipped
};

UCLASS()
class SLASH_API AItem : public AActor
{
	GENERATED_BODY()
private:
	
	UPROPERTY(VisibleAnywhere);
	float fTime{};

	UPROPERTY(EditAnywhere);
	float amplitude = 1.f;

	UPROPERTY(EditAnywhere)
	float freq = 5.f;
	
public:	
	// Sets default values for this actor's properties
	AItem();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	virtual void EndSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	FORCEINLINE void Hover(float DeltaTime);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly);
	TObjectPtr<UStaticMeshComponent> ItemMesh;

	EItemState ItemState = EItemState::EIS_Hovering;
	UPROPERTY(VisibleAnywhere);
	TObjectPtr<USphereComponent> Sphere;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
