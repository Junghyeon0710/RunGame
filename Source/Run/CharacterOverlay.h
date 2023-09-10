// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterOverlay.generated.h"

/**
 * 
 */
UCLASS()
class RUN_API UCharacterOverlay : public UUserWidget
{
	GENERATED_BODY()
public:
	void SetCoin(int32 RunCoin);
	void SetTime(float DeltaTime);
private:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Coin;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Time;

};
