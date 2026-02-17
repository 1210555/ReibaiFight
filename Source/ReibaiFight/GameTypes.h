// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameTypes.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class EMiniBossState : uint8
{
	Normal UMETA(DisplayName = "Normal"),
	Angry UMETA(DisplayName = "Angry")
};

UCLASS()
class REIBAIFIGHT_API UGameTypes : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
};
