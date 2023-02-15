// Copyright Epic Games, Inc. All Rights Reserved.

#include "UE5_EOSTestingGameMode.h"
#include "UE5_EOSTestingCharacter.h"
#include "UObject/ConstructorHelpers.h"

AUE5_EOSTestingGameMode::AUE5_EOSTestingGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
