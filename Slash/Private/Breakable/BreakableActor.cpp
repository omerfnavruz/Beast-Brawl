

#include "Breakable/BreakableActor.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "Items/Treasures/Treasure.h"
#include "Components/CapsuleComponent.h"
// Sets default values
ABreakableActor::ABreakableActor()
{

	PrimaryActorTick.bCanEverTick = false;
	GeometryCollection = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("Geometry Component"));
	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule Component"));
	SetRootComponent(GeometryCollection);
	Capsule->SetupAttachment(GetRootComponent());
	GeometryCollection->SetGenerateOverlapEvents(true);
	GeometryCollection->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GeometryCollection->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	Capsule->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	Capsule->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);

}


void ABreakableActor::BeginPlay()
{
	Super::BeginPlay();
	
}


void ABreakableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABreakableActor::GetHit_Implementation(const FVector& ImpactPoint)
{
	if (DidBreak) return;
	auto World = GetWorld();
	if (World && Treasures_BP.Num() > 0)
	{
		DidBreak = true;
		auto Loc = GetActorLocation();
		Loc.Z += 70.f;
		const int index = FMath::RandRange(0.0, Treasures_BP.Num() - 0.01);
		World->SpawnActor<ATreasure>(Treasures_BP[index], Loc, GetActorRotation());
		Capsule->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		//GeometryCollection->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	}

}

