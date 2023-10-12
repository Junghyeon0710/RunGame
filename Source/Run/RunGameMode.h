// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "RunGameMode.generated.h"

UCLASS(minimalapi)
class ARunGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ARunGameMode();

	void AddFloorTile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:

	/** ¹Ù´Ú */ 
	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<class AFloorTile>> FloortileClass;

	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<class AFloorTile>> FloortileCornerClass;

	UPROPERTY(VisibleAnywhere)
	FTransform FloorSpawnTransform;

	UPROPERTY(EditAnywhere)
	int32 FloorTileNum;

	AFloorTile* Floor;

	bool bIsStart = false;

};



