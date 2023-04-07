// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/HealthBarComponent.h"
#include "HUD/HeathBar.h"
#include "Components/ProgressBar.h"

void UHealthBarComponent::SetHealthPercent(float Percent)
{
	if (HealthBarEdit == nullptr)
	{
		HealthBarEdit = Cast<UHeathBar>(GetUserWidgetObject());
	}
	if (HealthBarEdit && HealthBarEdit->HealthBar)
	{
		HealthBarEdit->HealthBar->SetPercent(Percent);
	}
}
