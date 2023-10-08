// Fill out your copyright notice in the Description page of Project Settings.


#include "DieWidget.h"
#include <Components/TextBlock.h>

void UDieWidget::SetTime(float DeltaTime)
{
	FString DeltaTimeText = FString::Printf(TEXT("Time : %.2f"), DeltaTime);
	Time->SetText(FText::FromString(DeltaTimeText));

}
