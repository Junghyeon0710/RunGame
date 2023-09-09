// Fill out your copyright notice in the Description page of Project Settings.


#include "TurnPoint.h"
#include <Components/BoxComponent.h>
#include "RunCharacter.h"

// Sets default values
ATurnPoint::ATurnPoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	TurnPointBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TurnPointBox"));
	SetRootComponent(TurnPointBox);

}



// Called when the game starts or when spawned
void ATurnPoint::BeginPlay()
{
	Super::BeginPlay();
	
	TurnPointBox->OnComponentBeginOverlap.AddDynamic(this, &ATurnPoint::BoxBeginOverlap);
}

// Called every frame
void ATurnPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATurnPoint::BoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ARunCharacter* Character = Cast<ARunCharacter>(OtherActor);
	if (Character)
	{
		Character->SetCanTurn(true);
	}
}