// Copyright Epic Games, Inc. All Rights Reserved.

#include "RunGameMode.h"
#include "RunCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include <Kismet/GameplayStatics.h>
#include "FloorTile.h"
#include "Blocker.h"

ARunGameMode::ARunGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}


void ARunGameMode::BeginPlay()
{
	Super::BeginPlay();

	for (int32 i = 0; i < 10; i++)
	{
		AddFloorTile();
	}

}
void ARunGameMode::AddFloorTile()
{
	UWorld* World = GetWorld();
	if (World)
	{
		AFloorTile* Floor = World->SpawnActor<AFloorTile>(Floortile, SpawnTransform);
		if (Floor)
		{
			SpawnTransform = Floor->GetAttachTransform();
		}
	}
}

