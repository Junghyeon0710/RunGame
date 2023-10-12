// Fill out your copyright notice in the Description page of Project Settings.


#include "Item.h"
#include <Components/SphereComponent.h>
#include "RunCharacter.h"
#include <Kismet/GameplayStatics.h>


// Sets default values
AItem::AItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	SetRootComponent(Sphere);

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
	ItemMesh->SetupAttachment(RootComponent);
	
}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();
	
	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AItem::SphereBeginOverlap);
}

void AItem::SphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor->IsA(ARunCharacter::StaticClass()))
	{
		if (ItemSound)
		{
			UGameplayStatics::PlaySound2D(this, ItemSound);
		}
		Destroy();
	}
}
