// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterOverlay.h"
#include <Components/TextBlock.h>

void UCharacterOverlay::SetCoin(int32 RunCoin)
{
	FString RunCoinText = FString::Printf(TEXT("%d"), RunCoin);
	Coin->SetText(FText::FromString(RunCoinText));
	
}

void UCharacterOverlay::SetTime(float DeltaTime)
{
	FString DeltaTimeText = FString::Printf(TEXT("%.2f"), DeltaTime);
	ElapsedTime->SetText(FText::FromString(DeltaTimeText));
	
}

void UCharacterOverlay::SetTopScore(int32 InScore)
{
	FString TopScoreText = FString::Printf(TEXT("%d"), InScore);
	TopScore->SetText(FText::FromString(TopScoreText));
}



