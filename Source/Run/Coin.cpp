// Fill out your copyright notice in the Description page of Project Settings.


#include "Coin.h"
#include "RunCharacter.h"


void ACoin::SphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::SphereBeginOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	
	if (OtherActor && OtherActor->IsA(ARunCharacter::StaticClass()))
	{
		ARunCharacter* RunCharacter = Cast<ARunCharacter>(OtherActor);
		if (RunCharacter)
		{
			RunCharacter->PlusCoin();
		}
	}
}
