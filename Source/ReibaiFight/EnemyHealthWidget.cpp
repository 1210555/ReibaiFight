// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyHealthWidget.h"
#include "Components/ProgressBar.h"//UProgressBarクラスを使うために必要


void UEnemyHealthWidget::UpdateHealthBar(float Percent)
{
	if (EnemyHealthBar)
	{
		UE_LOG(LogTemp, Warning, TEXT("敵の体力バー更新！UEnemyHealthWidgetクラス"));

		EnemyHealthBar->SetPercent(Percent);
	}
}