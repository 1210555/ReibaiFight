// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyHealthWidget.h"
#include "Components/ProgressBar.h"//UProgressBarクラスを使うために必要


void UEnemyHealthWidget::UpdateHealthBar(float Percent)
{
	if (EnemyHealthBar)
	{
		UE_LOG(LogTemp, Warning, TEXT("Succeeded Update！ 現在のPercent: %f"), Percent);

		EnemyHealthBar->SetPercent(Percent);
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Not Found Enemy HealthWidget！"));
	}
}