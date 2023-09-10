// Fill out your copyright notice in the Description page of Project Settings.


#include "MyHUD.h"
#include "CharacterOverlay.h"

void AMyHUD::BeginPlay()
{
	UWorld* World = GetWorld();
	if (World)
	{
		APlayerController* MyController = World->GetFirstPlayerController();
		if (MyController&& CharacterOverlayClass)
		{
			CharacterOverlay = CreateWidget<UCharacterOverlay>(MyController, CharacterOverlayClass);
			if (CharacterOverlay)
			{
				CharacterOverlay->AddToViewport();
			}	
		}
	}
	
}
