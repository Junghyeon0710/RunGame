#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterOverlay.generated.h"

/**
 * 캐릭터 오버레이 위젯
 */
UCLASS()
class RUN_API UCharacterOverlay : public UUserWidget
{
	GENERATED_BODY()

public:
	// 코인 수를 설정하는 함수
	void SetCoin(int32 RunCoin);

	// 경과 시간을 설정하는 함수
	void SetTime(float DeltaTime);

private:
	// 코인을 나타내는 텍스트 블록 위젯
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Coin;

	// 경과 시간을 나타내는 텍스트 블록 위젯
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ElapsedTime;
};