// Copyright Epic Games, Inc. All Rights Reserved.

#include "ReibaiFightGameMode.h"
#include "ReibaiFightCharacter.h"
#include "UObject/ConstructorHelpers.h"

AReibaiFightGameMode::AReibaiFightGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
