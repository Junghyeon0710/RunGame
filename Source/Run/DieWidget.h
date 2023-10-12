// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DieWidget.generated.h"

/**
 * 
 */
UCLASS()
class RUN_API UDieWidget : public UUserWidget
{
	GENERATED_BODY()
public:

	void SetGameElapsedTime(float DeltaTime);

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* GameElapsedTime;
};
