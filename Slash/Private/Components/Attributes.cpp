// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Attributes.h"

// Sets default values for this component's properties
UAttributes::UAttributes()
{

	PrimaryComponentTick.bCanEverTick = true;


}


// Called when the game starts
void UAttributes::BeginPlay()
{
	Super::BeginPlay();


	
}


// Called every frame
void UAttributes::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

