// Fill out your copyright notice in the Description page of Project Settings.


#include "ReibaiFightGameInstance.h"

UReibaiFightGameInstance::UReibaiFightGameInstance()
{
	TotalManju = 0;
	CurrentRunManju = 0;
	DifficultyMultiplier = 1.0f;
}

void UReibaiFightGameInstance::AddRunManju(int32 Amount)
{
	CurrentRunManju += Amount;
}

void UReibaiFightGameInstance::AddManju(int32 Amount)
{
	CurrentRunManju += Amount;

	if (OnManjuChanged.IsBound())
	{
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