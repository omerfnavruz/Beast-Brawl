// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Item.h"
#include "DrawDebugHelpers.h"
#include "DbgFunctions.h"
#include <math.h>
#include "Components/SphereComponent.h"
#include "Characters/Tigreal.h"



// Sets default values
AItem::AItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMeshComponent"));
	RootComponent = ItemMesh;
	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	Sphere->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();


	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnSphereOverlap);
	Sphere->OnComponentEndOverlap.AddDynamic(this, &AItem::EndSphereOverlap);


	//UE_LOG(LogTemp, Warning, TEXT("Begin Play is called!\n"));

	//if (GEngine)
	//{
//		GEngine->AddOnScreenDebugMessage(1, 60.f, FColor::Cyan, FString("Item on screen message!"));
	//}



	//SetActorLocation(FVector(0.f, 0.f, 200.f));
	//SetActorRotation(FRotator(0.f, 45.f, 0.f));

	//DbgDrawer::VectorDbg(this);
	
}

void AItem::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	auto SlashCharacter = Cast<ATigreal>(OtherActor);

	if (SlashCharacter)
	{
		SlashCharacter->SetOverlappingItem(this);
	}
}

void AItem::EndSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	auto SlashCharacter = Cast<ATigreal>(OtherActor);

	if (SlashCharacter)
	{
		SlashCharacter->SetOverlappingItem(nullptr);
	}
}

void AItem::Hover(float DeltaTime)
{
	auto loc = GetActorLocation();
	double x = loc.GetComponentForAxis(EAxis::X);

	AddActorWorldOffset(FVector(0.f, 0.f, amplitude * sin(fTime * freq)));
	AddActorWorldRotation(FRotator(0.f, 20 * amplitude * DeltaTime, 0.f));
}

// Called every frame
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	fTime += DeltaTime;
	//UE_LOG(LogTemp, Warning, TEXT("FPS: %f\n"), 1 / DeltaTime);
	if (ItemState == EItemState::EIS_Hovering)
	{
		Hover(DeltaTime);
	}


#if 0
	if (GEngine)
	{

		auto Name = GetName();
		FString FpsMessage = FString::Printf(TEXT("FPS from %s: %f"), *Name, 1 / DeltaTime);
		//GEngine->AddOnScreenDebugMessage(1, 60.f, FColor::Cyan, FpsMessage);
	}

	
	//DbgDrawer::VectorDbgSF(this);
#endif
	
}

