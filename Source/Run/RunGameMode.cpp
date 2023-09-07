// Copyright Epic Games, Inc. All Rights Reserved.

#include "RunGameMode.h"
#include "RunCharacter.h"
#include "UObject/ConstructorHelpers.h"

ARunGameMode::ARunGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
