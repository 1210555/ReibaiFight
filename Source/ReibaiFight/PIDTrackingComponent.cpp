// Fill out your copyright notice in the Description page of Project Settings.


#include "PIDTrackingComponent.h"

// Sets default values for this component's properties
UPIDTrackingComponent::UPIDTrackingComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
	PrimaryComponentTick.bStartWithTickEnabled = true;
}

//ひとだまの初期角度を設定する関数
void UPIDTrackingComponent::SetInitialAngle(float InitialAngle)
{
	CurrentAngle = InitialAngle;
}

// Called when the game starts
void UPIDTrackingComponent::BeginPlay()
{
	Super::BeginPlay();

	//強制的にTickを有効にする
	SetComponentTickEnabled(true);
}


void UPIDTrackingComponent::SetTargetToFollow(USceneComponent* Target)
{
	//ブループリント上でTargetToFollowピンに追従させたい対象を設定
	TargetToFollow = Target;
}

//毎秒呼び出される関数
void UPIDTrackingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	//USceneComponentクラスのポインタがヌル値だった場合は処理を抜ける
	if (!TargetToFollow)
	{
		return;
	}

	// 角度を進める
	// DeltaTimeを掛けて、フレームレートに依存しない回転速度を実現
	//OrbitSpeedは角速度(rad/s)だから度数に変換するためDealtaTime(s)を掛ける
	CurrentAngle += OrbitSpeed * DeltaTime;

	//角度を0～360度に丸める (省略可だが、巨大な数になるのを防ぐ)
	CurrentAngle = FMath::Fmod(CurrentAngle, 360.0f);

	//角度を度(Degree)からラジアン(Radian)に変換する (C++のSin/Cos関数はラジアンを使うため)
	float CurrentAngleRad = FMath::DegreesToRadians(CurrentAngle);

	//新しい目標オフセット座標を計算
	FVector NewOffset;
	// X = Cos(角度) * 半径
	NewOffset.X = FMath::Cos(CurrentAngleRad) * OrbitRadius;
	// Y = Sin(角度) * 半径
	NewOffset.Y = FMath::Sin(CurrentAngleRad) * OrbitRadius;
	// Z軸方向は動かさない（プレイヤーのZ軸をそのまま維持）
	NewOffset.Z = 0.0f;

	// 5. PIDコントローラーが追いかける目標座標を更新
	TargetOffset = NewOffset;

	//以下PID制御の計算
	//目標値（ターゲットの位置 ＋ オフセット）を計算
	FVector TargetLocation = TargetToFollow->GetComponentLocation() + TargetOffset;
	FVector CurrentLocation = GetComponentLocation();

	// 3. 偏差（ズレ）を計算
	FVector Error = TargetLocation - CurrentLocation;

	// --- ここからがPID制御の計算 ---

	// 4. P項 (Proportional) - 現在のズレに比例する力（バネ）
	FVector P_Force = Kp * Error;

	// 5. I項 (Integral) - ズレの蓄積（定常偏差をなくす）
	IntegralError += Error * DeltaTime;
	FVector I_Force = Ki * IntegralError;

	// 6. D項 (Derivative) - ズレの変化率（急激な動きを抑えるブレーキ）
	FVector DerivativeError = (Error - LastError) / DeltaTime;
	LastError = Error;
	FVector D_Force = Kd * DerivativeError;

	// --- PIDここまで ---

	// 7. 合計の力を計算
	FVector TotalForce = P_Force + I_Force + D_Force;

	// 8. 力（加速度）を速度に変え、位置を更新 (簡易的な物理計算)
	CurrentVelocity += TotalForce * DeltaTime;
	FVector NewLocation = CurrentLocation + CurrentVelocity * DeltaTime;

	// 9. 算出した新しい位置に自分自身を移動させる
	SetWorldLocation(NewLocation);
}

void UPIDTrackingComponent::ResetState()
{
	CurrentVelocity = FVector::ZeroVector;
	LastError = FVector::ZeroVector;
	IntegralError = FVector::ZeroVector;
}