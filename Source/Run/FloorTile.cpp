// Fill out your copyright notice in the Description page of Project Settings.


#include "FloorTile.h"
#include <Components/ArrowComponent.h>
#include <Components/BoxComponent.h>
#include "RunGameMode.h"
#include "RunCharacter.h"
#include "Blocker.h"

// Sets default values
AFloorTile::AFloorTile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	SetRootComponent(Scene);

	Floor = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Floor"));
	Floor->SetupAttachment(Scene);

	Wall = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Wall"));
	Wall->SetupAttachment(GetRootComponent());

	Wall2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Wall2"));
	Wall2->SetupAttachment(GetRootComponent());

	AttachPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("AttachPoint"));
	AttachPoint->SetupAttachment(RootComponent);

	BoxPoint = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	BoxPoint->SetupAttachment(RootComponent);

	SpawnPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("SpawnPoint"));
	SpawnPoint->SetupAttachment(RootComponent);

	SpawnPointL = CreateDefaultSubobject<UArrowComponent>(TEXT("SpawnPointL"));
	SpawnPointL->SetupAttachment(RootComponent);

	SpawnPointR = CreateDefaultSubobject<UArrowComponent>(TEXT("SpawnPointR"));
	SpawnPointR->SetupAttachment(RootComponent);


}

// Called when the game starts or when spawned
void AFloorTile::BeginPlay()
{
	Super::BeginPlay();

	BoxPoint->OnComponentBeginOverlap.AddDynamic(this, &AFloorTile::BoxBeginOverlap);
	
}

void AFloorTile::BoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UWorld* World = GetWorld();
	if (World && OtherActor->IsA(ARunCharacter::StaticClass()))
	{
		if (SpawnPoints.Num() > 0)
		{
			const int32 Blockerindex = FMath::RandRange(0, SpawnPoints.Num() - 1);
			Blocker = World->SpawnActor<ABlocker>(SpawnBlocker, SpawnPoints[Blockerindex]);
		}
		ARunGameMode* RunGameMode =	World->GetAuthGameMode<ARunGameMode>();
		if (RunGameMode)
		{
			RunGameMode->AddFloorTile();
			SetLifeSpan(2.f);
			Blocker->SetLifeSpan(8.f);
		}
	}
}

// Called every frame
void AFloorTile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AFloorTile::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	SetSpawnPoints();
}

void AFloorTile::SetSpawnPoints()
{
	SpawnPoints.Add(SpawnPoint->GetComponentTransform());
	SpawnPoints.Add(SpawnPointL->GetComponentTransform());
	SpawnPoints.Add(SpawnPointR->GetComponentTransform());
}

FTransform AFloorTile::GetAttachTransform() const
{
	return AttachPoint->GetComponentTransform();
}

