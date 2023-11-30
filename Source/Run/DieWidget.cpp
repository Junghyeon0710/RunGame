// Fill out your copyright notice in the Description page of Project Settings.


#include "DieWidget.h"
#include <Components/TextBlock.h>

void UDieWidget::SetGameElapsedTime(float DeltaTime)
{
	FString DeltaTimeText = FString::Printf(TEXT("TopScore : %.2f"), DeltaTime);
	GameElapsedTime->SetText(FText::FromString(DeltaTimeText));

}
