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
    // 필요한 블럭 클래스 배열 선택
    const TArray<TSubclassOf<AFloorTile>> FloorTileClasses = (FloorTileNum < 8) ? FloortileClass : FloortileCornerClass;

    if (FloorTileClasses.Num() == 0)
    {
        return;
    }

    // 임의의 블럭 클래스 선택
    const int32 RandomFloorIndex = FMath::RandRange(0, FloorTileClasses.Num() - 1);

    Floor = World->SpawnActor<AFloorTile>(FloorTileClasses[RandomFloorIndex], FloorSpawnTransform);

    if (Floor)
    {
        // 아이템 스폰 확률
        const int32 ItemSpawn = FMath::RandRange(0, 30);
        FloorSpawnTransform = Floor->GetAttachFloorTransform();

        // 3번째 또는 4번째 블럭의 경우 아무 작업도 수행하지 않음
        if (FloorTileNum == 3 || FloorTileNum == 4)
        {
            ++FloorTileNum;
            return;
        }


		// Blocker 및 Coin 생성
		Floor->BlockerCreate();
		Floor->CoinCreate();
 

        if (ItemSpawn == 0)
        {
            // 아이템 스폰
            Floor->ItemSpawn();
        }
        Floor->SetLifeSpan(20.f);
    }


    // FloorTileNum 업데이트
    FloorTileNum = (FloorTileNum < 8) ? FloorTileNum + 1 : 0;
}

