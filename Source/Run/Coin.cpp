// Fill out your copyright notice in the Description page of Project Settings.


#include "Coin.h"
#include "RunCharacter.h"
#include <Kismet/GameplayStatics.h>


void ACoin::SphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{	
	if (OtherActor && OtherActor->IsA(ARunCharacter::StaticClass()))
	{
		if (ItemSound)
		{
			UGameplayStatics::PlaySound2D(this, ItemSound);
		}
		ARunCharacter* RunCharacter = Cast<ARunCharacter>(OtherActor);
		if (RunCharacter)
		{
			RunCharacter->PlusCoin();
		}
		if (!RunCharacter->bHasMagnet)
		{
			Destroy();
		}
	}
}
