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

	//プレイヤーの全財産（まんじゅう）の数。これをセーブデータからロードする
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Date")
	int32 TotalManju;

	//今回のプレイで入手したまんじゅうの数。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Game Date")
	int32 CurrentRunManju;

	//適用する難易度の倍率。ほこらで設定したもの
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Date")
	float DifficultyMultiplier;

	//ゲーム中にまんじゅうを獲得したら呼ぶ関数
	UFUNCTION(BlueprintCallable, Category = "Game Logic")
	void AddRunManju(int32 Amount);
	//{
	//	CurrentRunManju += Amount;
	//}

	//ゲームオーバー、クリア時に呼ぶ関数。今回のまんじゅうを持ち帰るため
	UFUNCTION(BlueprintCallable, Category = "Game Logic")
	void CommitRunManju();

	//リスタートするときの関数。今回のまんじゅうをリセットするため
	UFUNCTION(BlueprintCallable, Category = "Game Logic")
	void ResetRunData();

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnManjuChanged OnManjuChanged;

	UFUNCTION(BlueprintCallable, Category = "Game Logic")
	void AddManju(int32 Amount);

};
