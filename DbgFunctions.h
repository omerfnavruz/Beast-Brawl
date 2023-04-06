#pragma once

#include "GameFramework/Actor.h"

class DbgDrawer
{
public:
	static void SphereDbg(AActor* actor)
	{
		auto World = actor->GetWorld();
		auto Location = actor->GetActorLocation();
		if (World)
		{
			DrawDebugSphere(World, Location, 25.f, 24, FColor::Red, false, 30.f);
		}
	}

	static void SphereAtLoc(UWorld* World, const FVector Location)
	{
		if (World)
		{
			DrawDebugSphere(World, Location, 5.f, 24, FColor::Magenta, false, 2.f);
		}
	}

	static void LineDbg(AActor* actor)
	{
		auto World = actor->GetWorld();
		auto Location = actor->GetActorLocation();
		if (World)
		{
			auto Forward = actor->GetActorForwardVector();
			DrawDebugLine(World, Location, Location + Forward*100.f, FColor::Red, true, -1.f, 0, 1.f);
		}
	}

	static void PointDbg(AActor* actor)
	{
		auto World = actor->GetWorld();
		auto Location = actor->GetActorLocation();
		if (World)
		{
			auto Forward = actor->GetActorForwardVector();
			DrawDebugPoint(World, Location + Forward*100.f, 15.f, FColor::Red, true);
		}
	}

	static void VectorDbg(AActor* actor)
	{
		SphereDbg(actor);
		LineDbg(actor);
		PointDbg(actor);
	}


	static void SphereDbgSF(AActor* actor)
	{
		auto World = actor->GetWorld();
		auto Location = actor->GetActorLocation();
		if (World)
		{
			DrawDebugSphere(World, Location, 25.f, 24, FColor::Red, false);
		}
	}

	static void LineDbgSF(AActor* actor)
	{
		auto World = actor->GetWorld();
		auto Location = actor->GetActorLocation();
		if (World)
		{
			auto Forward = actor->GetActorForwardVector();
			DrawDebugLine(World, Location, Location + Forward * 100.f, FColor::Red, false, -1.f, 0, 1.f);
		}
	}

	static void PointDbgSF(AActor* actor)
	{
		auto World = actor->GetWorld();
		auto Location = actor->GetActorLocation();
		if (World)
		{
			auto Forward = actor->GetActorForwardVector();
			DrawDebugPoint(World, Location + Forward * 100.f, 15.f, FColor::Red, false);
		}
	}

	static void VectorDbgSF(AActor* actor)
	{
		SphereDbgSF(actor);
		LineDbgSF(actor);
		PointDbgSF(actor);
	}


};


