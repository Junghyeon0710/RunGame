## Unreal5 : RunGame
포토폴리오 목적으로 제작된 간단한 런게임입니다.

- 엔진 : Unreal Engine 5.3
- IDE : Visual Studio 2022
- 툴 : C++ , Blueprint 혼합사용
- 제작기간 : 일주일
- 인원 : 개인 개발

## 영상
[![Video Label](https://img.youtube.com/vi/VhnznJZvzk4/0.jpg)](https://www.youtube.com/watch?v=VhnznJZvzk4')


## 기술 설명
### Character Movement
- 앞으로 자동으로 달리기
- A,D 키로 좌우로 움직일 수 있음
- 턴 지점이 오면 90도 턴

```C++
  // A 키를 눌렀을 때 호출되는 함수
void ARunCharacter::AInput()
{
	// 회전이 가능한 상태가 아니면 함수 종료
	if (!bCanTurn) return;

	// 원하는 회전값에 -90도의 Yaw 회전을 추가
	DesiredRotation += FRotator(0.f, -90.f, 0.f);

	// 회전을 한 번 수행했으므로 회전 상태를 비활성화
	bCanTurn = false;
}

// D 키를 눌렀을 때 호출되는 함수
void ARunCharacter::DInput()
{
	// 회전이 가능한 상태가 아니면 함수 종료
	if (!bCanTurn) return;

	// 원하는 회전값에 90도의 Yaw 회전을 추가
	DesiredRotation += FRotator(0.f, 90.f, 0.f);

	// 회전을 한 번 수행했으므로 회전 상태를 비활성화
	bCanTurn = false;
}

//코너에서 턴
void ARunCharacter::TurnCorner()
{
	if (Controller)
	{
		// 현재 캐릭터의 회전값 가져오기
		FRotator Rotation = GetControlRotation();

		// 목표 회전값과 현재 회전값 비교
		if (DesiredRotation != Rotation)
		{
			// 보간된 회전값 계산 (매 프레임마다 약간씩 회전을 조절)
			FRotator InterpolatedRotation = FMath::RInterpTo(Rotation, DesiredRotation, GetWorld()->DeltaTimeSeconds, 45.f);

			// 플레이어 컨트롤러를 가져오고 회전값 설정
			APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
			if (PlayerController)
			{
				PlayerController->SetControlRotation(InterpolatedRotation);

			}
		}
	}
}
```

## GameMode
- 지정한 바닥 랜덤으로 스폰
- 7번 생성시 코너 바닥 스폰
- 장애물 스폰
- 아이템 스폰

 ```C++
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
 
        // 아이템 스폰 확률
        const int32 ItemSpawn = FMath::RandRange(0, 30);
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
```

## Items
### Item : Magnet
  - 먹을 시 머트리얼 노랗게 변경
  - 주변 코인을 끌어댕김
  - 코인을 캐릭터 매쉬에 Attach 후 Move Component하는 방식 사용

### Item : SpeedBoom
  - 먹을 시 머트리얼 빨갛게 변경
  - 장애물을 다 파괴
  - MaxWalkSpeed 증가

## FloorTile
  ### Components
  - UArrowComponent로 다음 바닥에 이어 붙일 포인트 구현
  - UArrowComponent로 장애물 위치 지정
  - UArrowComponent로 ItemSpawnPoint 위치 지정
  - UBoxComponent로 코인 랜덤 위치 지정
  - UBoxComponent로 캐릭터가 턴 할 수 있는 위치 지정
  - UStaticMeshComponent 바닥, 벽 스태틱매쉬
  ### 기능
  - 장애물 부딪힐시 사망
  - 마주보는 벽 부딪힐시 사망
  - 캐릭터 턴

#### FloorTile 블루프린트

    

