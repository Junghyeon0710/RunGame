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
		if (FloorTileNum < 8 && Floortile.Num() >0)
		{
			const int32 FloorIndex = FMath::RandRange(0, Floortile.Num()-1);
			Floor = World->SpawnActor<AFloorTile>(Floortile[FloorIndex], SpawnTransform);
			FloorTileNum++;
		}
		else if(FloortileCorner.Num() > 0)
		{
			const int32 FloorIndexCorner = FMath::RandRange(0, FloortileCorner.Num()-1);
			Floor = World->SpawnActor<AFloorTile>(FloortileCorner[FloorIndexCorner], SpawnTransform);
			FloorTileNum = 0;
		}
		if (Floor)
		{
			SpawnTransform = Floor->GetAttachTransform();
			Floor->BlockerCreate();
			Floor->CoinCreate();
		}
	
	}
}

