// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "ReibaiFightGameInstance.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnManjuChanged, int32, NewCount);

UCLASS()
class REIBAIFIGHT_API UReibaiFightGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UReibaiFightGameInstance();

	void Init() override; // これを追加

	//プレイヤーの全財産（まんじゅう）の数。これをセーブデータからロードする
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Date")
	int32 TotalManju;

	//今回のプレイで入手したまんじゅうの数。HUDにこの数が表示される
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Game Date")
	int32 CurrentRunManju;

	//適用する難易度の倍率。ほこらで設定したもの
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Date")
	float DifficultyMultiplier;

	//ゲームオーバー、クリア時に呼ぶ関数。今回のまんじゅうを持ち帰るため
	UFUNCTION(BlueprintCallable, Category = "Game Logic")
	void CommitRunManju();

	//リスタートするときの関数。今回のまんじゅうをリセットするため
	UFUNCTION(BlueprintCallable, Category = "Game Logic")
	void ResetRunData();

	//HUDに数が増えたことを知らせる。HUDでバインドして、まんじゅうの数が変わるたびにHUDを更新する
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnManjuChanged OnManjuChanged;

	//これをよんでHUDに反映させる。
	UFUNCTION(BlueprintCallable, Category = "Game Logic")
	void AddManju(int32 Amount);

	//プレイヤーがスライダーで決めた「捧げる量」を受け取り、難易度を計算して保存する関数
	UFUNCTION(BlueprintCallable, Category = "Shrine System")
	bool ApplyOffering(int32 ManjuAmount);

	// 捧げる量に応じた難易度を「予測」して返す関数
	//これをほこらWBPの画面で値によって背景色を変える
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Shrine System")
	float CalculateDifficulty(int32 ManjuAmount);
};
