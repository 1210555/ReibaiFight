// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Experience.generated.h"

//USTRUCT(BlueprintType)
//struct FExperienceCounts
//{
//	GENERATED_BODY()
//
//	//100経験値の数
//	UPROPERTY(BlueprintReadOnly)
//	int32 BigExperienceCount = 0;
//
//	//25経験値の数
//	UPROPERTY(BlueprintReadOnly)
//	int32 MiddleExperienceCount = 0;
//
//	//5経験値の数
//	UPROPERTY(BlueprintReadOnly)
//	int32 SmallExperienceCount = 0;
//};

UCLASS()
class REIBAIFIGHT_API AExperience : public AActor
{
	GENERATED_BODY()
	
private:
	//ホーミングにかかってる時間
	float CurrentHomingSpeed = 0.0f;

	//ホーミングの進行度合い。０～１の百分率で表す
	float CurrentAlpha = 0.0f;

	//経験値の軌道の長さを保存しとく
	float TotalPathLength = 0.0f;

	FVector InitialLocation;

	FVector ControlPoint;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	bool IsHoming = false;

	UPROPERTY()
	AActor* HomingTarget;


	UPROPERTY(EditAnywhere, Category = "Homing")
	float HomingDuration = 1.0f;

	UPROPERTY(EditAnywhere,Category="Homing")
	float HomingArcHeight = 300.0f;

	UPROPERTY(EditAnywhere, Category = "Homing")
	float HomingAccelaration = 300.0f;

public:	
	AExperience();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//これがよばれたらホーミングを開始する
	UFUNCTION(BlueprintCallable,Category="Movement")
	void StartHoming(AActor* TargetActor);

	// 自分が持っている経験値量（拾われたらこの値をプレイヤーに渡す）
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Experience")
	int32 ExperienceValue = 10;

	// 初期化関数：量と大きさを指定する
	void InitializeExperience(int32 Value, float ScaleSize);
};
