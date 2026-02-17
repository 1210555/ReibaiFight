// Fill out your copyright notice in the Description page of Project Settings.


#include "Experience.h"

// Sets default values
AExperience::AExperience()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AExperience::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AExperience::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//ホーミング状態になっていてかつターゲットが有効な場合
	if(IsHoming && HomingTarget)
	{
		// ホーミング速度を更新し、それに応じた距離も計算
		CurrentHomingSpeed += HomingAccelaration * DeltaTime;
		float CurrentDistance = CurrentHomingSpeed * DeltaTime;

		// 進行度合いを更新
		CurrentAlpha = CurrentDistance / TotalPathLength;
		// 進行度合い (0.0 ～ 1.0) を計算
		// Clampを使って1.0を超えないようにする
		float Alpha = FMath::Clamp(CurrentAlpha, 0.0f, 1.0f);

		// --- ベジェ曲線の計算 (2次ベジェ) ---

		// P0: 開始地点 (保存しておいたもの)
		FVector P0 = InitialLocation;

		// P1: 制御点 (保存しておいた上空の点)
		FVector P1 = ControlPoint;

		// P2: 目標地点 (動いているターゲットの現在位置を毎フレーム取得！)
		// ※足元ではなく、少し上(+50)を狙うと地面に埋まらない
		FVector P2 = HomingTarget->GetActorLocation() + FVector(0.0f, 0.0f, 50.0f);

		// A = P0とP1の間を Alpha で補間
		FVector A = FMath::Lerp(P0, P1, Alpha);
		// B = P1とP2の間を Alpha で補間
		FVector B = FMath::Lerp(P1, P2, Alpha);

		// 最終位置 = AとBの間を Alpha で補間
		FVector NewLocation = FMath::Lerp(A, B, Alpha);

		// 移動反映
		SetActorLocation(NewLocation);
	}
}

void AExperience::StartHoming(AActor* TargetActor)
{
	
	if (!TargetActor) return;

	IsHoming = true;
	HomingTarget = TargetActor;
	CurrentHomingSpeed = 0.0f;
	CurrentAlpha = 0.0f;
	TotalPathLength = 0.0f;
	InitialLocation = GetActorLocation();
	FVector TargetLocation = HomingTarget->GetActorLocation();
	FVector TopPoint = (InitialLocation + TargetLocation) * 0.5f;//すぐに上がるのをイメージ
	ControlPoint =TopPoint + FVector(0, 0, HomingArcHeight);//上に少し上げる。エディタで変更可

	float Distance1 = FVector::Dist(InitialLocation, ControlPoint);
	float Distance2 = FVector::Dist(ControlPoint, TargetLocation);
	TotalPathLength = Distance1 + Distance2;
}

//FExperienceCounts AExperience::CalculateExperienceAmount(float DroppedExperiecne)
//{
//    int32 BigExperience = 100.0f;//1つのメッシュあたりの経験値量
//	int32 MiddleExperience = 25.0f;
//	int32 SmallExperience = 5.0f;
//	int32 RemainingExperience = FMath::FloorToInt(DroppedExperiecne);
//
//	FExperienceCounts ExperienceCounts;
//
//	ExperienceCounts.BigExperienceCount = RemainingExperience / BigExperience;
//	RemainingExperience = RemainingExperience % BigExperience;
//
//	ExperienceCounts.MiddleExperienceCount = RemainingExperience / BigExperience;
//	RemainingExperience = RemainingExperience % BigExperience;
//
//	ExperienceCounts.SmallExperienceCount = RemainingExperience / BigExperience;
//	RemainingExperience = RemainingExperience % BigExperience;
//
//	//戻り値に100,25,5の経験値の数を入れた配列を返す？
//	return ExperienceCounts;
//}

//どのくらいの経験値を持っているか、大きさをどうするか
void AExperience::InitializeExperience(int32 Value, float ScaleSize)
{
	// 1. 点数を保存
	ExperienceValue = Value;

	// 2. 大きさを変更 (1.0f が標準)
	SetActorScale3D(FVector(ScaleSize));
}