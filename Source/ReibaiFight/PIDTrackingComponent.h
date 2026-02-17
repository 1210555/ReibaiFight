// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "PIDTrackingComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable)
class REIBAIFIGHT_API UPIDTrackingComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPIDTrackingComponent();

	//追従するターゲット（プレイヤーなど）を設定する関数
	//ターゲットはBP側で設定する
	UFUNCTION(BlueprintCallable, Category = "PID")
	void SetTargetToFollow(USceneComponent* Target);

	//初期角度を設定する関数
	//これがないと複数のひとだまを発生させたときに同じ位置に現れてしまう
	UFUNCTION(BlueprintCallable, Category = "Orbit")
	void SetInitialAngle(float InitialAngle);

	UPROPERTY()
	TObjectPtr<USceneComponent> TargetToFollow;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PID")
	FVector TargetOffset = FVector(100.0f, 0.0f, 0.0f); // デフォルトのオフセット

	//PID制御のゲイン
	//BPからいいと感じた値に調整できるように
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PID")
	float Kp = 100.0f; // Pゲイン（バネの強さ）

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PID")
	float Ki = 0.0f;   // Iゲイン（ズレの蓄積）

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PID")
	float Kd = 20.0f;  // Dゲイン（ブレーキの強さ）

	void ResetState();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//ひとだまの回転の半径
	//
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orbit")
	float OrbitRadius = 150.0f;

	//ひとだまの回転速度
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orbit")
	float OrbitSpeed = 180.0f;

	//ひとだまの現在の角度
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Orbit")
	float CurrentAngle = 0.0f; // 現在の角度（度）
private:


	// PID計算に必要な内部変数
		FVector CurrentVelocity; // 現在の速度
		FVector LastError;       // 1フレーム前の偏差
		FVector IntegralError;   // 偏差の蓄積

		
};
