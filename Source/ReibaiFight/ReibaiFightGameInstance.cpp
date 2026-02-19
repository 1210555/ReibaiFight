// Fill out your copyright notice in the Description page of Project Settings.


#include "ReibaiFightGameInstance.h"

UReibaiFightGameInstance::UReibaiFightGameInstance()
{
	TotalManju = 200;
	CurrentRunManju = 0;
	DifficultyMultiplier = 1.0f;
}

void UReibaiFightGameInstance::Init()
{
	Super::Init();
	//ここでセーブデータからTotalManjuをロードする処理を入れる
	TotalManju = 5000;//テスト用に5000にしてる
}

void UReibaiFightGameInstance::AddManju(int32 Amount)
{
	CurrentRunManju += Amount;

	if (OnManjuChanged.IsBound())
	{
		UE_LOG(LogTemp, Warning, TEXT("Manju HOUSOUTYU Current Manju is %d"), CurrentRunManju);
		OnManjuChanged.Broadcast(CurrentRunManju);
	}
}

void UReibaiFightGameInstance::CommitRunManju()
{
	TotalManju += CurrentRunManju;
	CurrentRunManju = 0; // 今回のプレイで獲得したまんじゅうをリセット
	//ここにセーブ処理を入れる
}

void UReibaiFightGameInstance::ResetRunData()
{
	CurrentRunManju = 0; // 今回のプレイで獲得したまんじゅうをリセット
	//ここにリスタートのための処理を入れる（必要なら）
}

float UReibaiFightGameInstance::CalculateDifficulty(int32 ManjuAmount)
{
	// 例：100個捧げるごとに 0.1倍 難しくなる（基本は1.0倍）
	// 500個捧げたら -> 1.0 + 0.5 = 1.5倍
	return 1.0f + (float)ManjuAmount / 1000.0f;
}

bool UReibaiFightGameInstance::ApplyOffering(int32 ManjuAmount)
{
	// 所持まんじゅうが足りているかチェック
	if (ManjuAmount > TotalManju)
	{
		ManjuAmount = TotalManju;
		return false; //足りないときは適用せずにfalseを返す
	}

	// まんじゅうをほこらに捧げる処理
	TotalManju -= ManjuAmount;

	// 難易度倍率を計算して保存
	DifficultyMultiplier = CalculateDifficulty(ManjuAmount);

	UE_LOG(LogTemp, Warning, TEXT("ManjuHounou！ : %d, NewDifficulty: %f"), TotalManju, DifficultyMultiplier);

	return true; //適用成功
}