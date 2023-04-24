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
	AttachMeshToSocket(SceneComponent, Name);
	ItemState = EItemState::EIS_Equipped;
	PlayShinkSound();
	DisableSphereCollision();
	SetOwner(NewOwner);
	SetInstigator(NewInvestigator);

	
}

void AWeapon::DisableSphereCollision()
{
	if (Sphere)
		Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AWeapon::PlayShinkSound()
{
	if (ShrinkSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ShrinkSound, GetActorLocation());
	}
}

void AWeapon::AttachMeshToSocket(USceneComponent* SceneComponent, const FName& Name)
{
	if (SceneComponent){
		FAttachmentTransformRules Rules(EAttachmentRule::SnapToTarget, true);
		ItemMesh->AttachToComponent(SceneComponent, Rules, Name);
	}

}

void AWeapon::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (IsBothEnemy(OtherActor)) return;
	FHitResult HitResult;
	BoxTrace(HitResult);
	
	if (const auto HitActor = HitResult.GetActor())
	{
		if (IsBothEnemy(HitActor)) return;
		ApplyDamage(HitActor);
		HitTargetActor(HitResult);
	}

}

bool AWeapon::IsBothEnemy(AActor* OtherActor)
{
	return GetOwner()->ActorHasTag(FName("Enemy")) && OtherActor->ActorHasTag(FName("Enemy"));
}

void AWeapon::ApplyDamage(AActor* const& HitActor)
{
	UGameplayStatics::ApplyDamage(
		HitActor,
		Damage,
		GetInstigator()->GetController(),
		this,
		UDamageType::StaticClass()
	);
}

void AWeapon::HitTargetActor(FHitResult& HitResult)
{
	if (const auto HitInterface = Cast<IHitInterface>(HitResult.GetActor()))
	{
		HitInterface->Execute_GetHit(HitResult.GetActor(), HitResult.ImpactPoint, GetOwner());
	}
	CreateFields(HitResult.ImpactPoint);
}

void AWeapon::BoxTrace(FHitResult& HitResult)
{
	TArray<AActor*> IgnoreArray;
	IgnoreArray.Add(this);

	for (AActor* Actor : IgnoreActors)
	{
		IgnoreArray.AddUnique(Actor);
	}

	UKismetSystemLibrary::BoxTraceSingle(
		this,
		TraceStart->GetComponentLocation(),
		TraceEnd->GetComponentLocation(),
		BoxTraceExtent,
		TraceStart->GetComponentRotation(),
		ETraceTypeQuery::TraceTypeQuery1,
		false,
		IgnoreArray,
		ShowDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
		HitResult,
		true
	);
	const auto HitActor = HitResult.GetActor();
	IgnoreActors.AddUnique(HitActor);
}


