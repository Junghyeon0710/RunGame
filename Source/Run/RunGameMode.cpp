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
    Floor = GetWorld()->SpawnActor<AFloorTile>(FloortileClass[0],FVector(0,0,0), FRotator::ZeroRotator);
    FloorSpawnTransform = Floor->GetAttachFloorTransform();
	for (int32 i = 0; i < 8; i++)
	{
		AddFloorTile();
		if (i == 7)
		{
			bIsStart = true;
		}
	}
}

void ARunGameMode::AddFloorTile()
{
    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }
    if (!bIsStart)
    {
        Floor = World->SpawnActor<AFloorTile>(FloortileClass[0], FloorSpawnTransform);
        FloorSpawnTransform = Floor->GetAttachFloorTransform();
        Floor->SetLifeSpan(7.f);
        return;
    }
    // �ʿ��� �� Ŭ���� �迭 ����
    const TArray<TSubclassOf<AFloorTile>> FloorTileClasses = (FloorTileNum < 8) ? FloortileClass : FloortileCornerClass;

    if (FloorTileClasses.Num() == 0)
    {
        return;
    }

    // ������ �� Ŭ���� ����
    const int32 RandomFloorIndex = FMath::RandRange(0, FloorTileClasses.Num() - 1);

    Floor = World->SpawnActor<AFloorTile>(FloorTileClasses[RandomFloorIndex], FloorSpawnTransform);

    if (Floor)
    {
        // ������ ���� Ȯ��
        const int32 ItemSpawn = FMath::RandRange(0, 30);
        FloorSpawnTransform = Floor->GetAttachFloorTransform();

        // 3��° �Ǵ� 4��° ���� ��� �ƹ� �۾��� �������� ����
        if (FloorTileNum == 3 || FloorTileNum == 4)
        {
            ++FloorTileNum;
            return;
        }


		// Blocker �� Coin ����
		Floor->BlockerCreate();
		Floor->CoinCreate();
 

        if (ItemSpawn == 0)
        {
            // ������ ����
            Floor->ItemSpawn();
        }
        Floor->SetLifeSpan(20.f);
    }


    // FloorTileNum ������Ʈ
    FloorTileNum = (FloorTileNum < 8) ? FloorTileNum + 1 : 0;
}

