#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterOverlay.generated.h"

/**
 * ĳ���� �������� ����
 */
UCLASS()
class RUN_API UCharacterOverlay : public UUserWidget
{
	GENERATED_BODY()

public:
	// ���� ���� �����ϴ� �Լ�
	void SetCoin(int32 RunCoin);

	// ��� �ð��� �����ϴ� �Լ�
	void SetTime(float DeltaTime);

	// ��� �ð��� �����ϴ� �Լ�
	UFUNCTION(BlueprintCallable)
	void SetTopScore(int32 InScore);

private:
	// ������ ��Ÿ���� �ؽ�Ʈ ��� ����
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Coin;

	// ��� �ð��� ��Ÿ���� �ؽ�Ʈ ��� ����
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ElapsedTime;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TopScore;
};