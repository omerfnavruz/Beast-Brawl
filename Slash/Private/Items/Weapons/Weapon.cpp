// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Weapons/Weapon.h"
#include "Characters/Tigreal.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Interfaces/HitInterface.h"


AWeapon::AWeapon()
{
	WeaponBoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Weapon Box Component"));
	WeaponBoxComponent->SetupAttachment(GetRootComponent());
	WeaponBoxComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponBoxComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	WeaponBoxComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

	TraceStart = CreateDefaultSubobject<USceneComponent>(TEXT("Box Trace Start"));
	TraceStart->SetupAttachment(GetRootComponent());
	TraceEnd = CreateDefaultSubobject<USceneComponent>(TEXT("Box Trace End"));
	TraceEnd->SetupAttachment(GetRootComponent());
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	WeaponBoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnBoxOverlap);
}

void AWeapon::Equip(USceneComponent* SceneComponent, FName Name, AActor* NewOwner, APawn* NewInvestigator)
{

	if (SceneComponent)
	{
		AttachMeshToSocket(SceneComponent, Name);
		ItemState = EItemState::EIS_Equipped;
		if (ShrinkSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, ShrinkSound, GetActorLocation());
		}
		Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		SetOwner(NewOwner);
		SetInstigator(NewInvestigator);

	}
}

void AWeapon::AttachMeshToSocket(USceneComponent* SceneComponent, const FName& Name)
{
	FAttachmentTransformRules Rules(EAttachmentRule::SnapToTarget, true);
	ItemMesh->AttachToComponent(SceneComponent, Rules, Name);
}


void AWeapon::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);


}

void AWeapon::EndSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::EndSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
}

void AWeapon::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	TArray<AActor*> IgnoreArray;
	IgnoreArray.Add(this);

	for (AActor* Actor : IgnoreActors)
	{
		IgnoreArray.AddUnique(Actor);
	}
	FHitResult HitResult;
	UKismetSystemLibrary::BoxTraceSingle(
		this,
		TraceStart->GetComponentLocation(),
		TraceEnd->GetComponentLocation(),
		FVector(5.f, 5.f, 5.f),
		TraceStart->GetComponentRotation(),
		ETraceTypeQuery::TraceTypeQuery1,
		false,
		IgnoreArray,
		EDrawDebugTrace::None,
		HitResult,
		true
	);
	
	if (const auto HitActor = HitResult.GetActor())
	{
		UGameplayStatics::ApplyDamage(
			HitResult.GetActor(),
			Damage,
			GetInstigator()->GetController(),
			this,
			UDamageType::StaticClass()
		);

		const auto HitInterface = Cast<IHitInterface>(HitActor);
		if (HitInterface)
		{
			HitInterface->Execute_GetHit(HitResult.GetActor(), HitResult.ImpactPoint);
		}
		IgnoreArray.AddUnique(HitResult.GetActor());
		CreateFields(HitResult.ImpactPoint);


	}

}


