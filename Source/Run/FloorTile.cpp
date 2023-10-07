// Fill out your copyright notice in the Description page of Project Settings.


#include "FloorTile.h"
#include <Components/ArrowComponent.h>
#include <Components/BoxComponent.h>
#include "RunGameMode.h"
#include "RunCharacter.h"
#include "Blocker.h"
#include "Kismet/KismetMathLibrary.h"
#include "Coin.h"

// Sets default values
AFloorTile::AFloorTile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

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

	CoinBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CoinBox"));
	CoinBox->SetupAttachment(RootComponent);

	TurnBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TurnBox"));
	TurnBox->SetupAttachment(RootComponent);
	TurnBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	TurnBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
}

// Called when the game starts or when spawned
void AFloorTile::BeginPlay()
{
	Super::BeginPlay();

	BoxPoint->OnComponentBeginOverlap.AddDynamic(this, &AFloorTile::BoxBeginOverlap);
	TurnBox->OnComponentBeginOverlap.AddDynamic(this, &AFloorTile::TurnBoxBeginOverlap);

	Wall->OnComponentHit.AddDynamic(this, &AFloorTile::WallOnHit);
	Wall2->OnComponentHit.AddDynamic(this, &AFloorTile::Wall2OnHit);
}

void AFloorTile::BoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UWorld* World = GetWorld();
	if (World && OtherActor->IsA(ARunCharacter::StaticClass()))
	{
		ARunGameMode* RunGameMode =	World->GetAuthGameMode<ARunGameMode>();
		if (RunGameMode)
		{
			RunGameMode->AddFloorTile();
			SetLifeSpan(5.f);
		}
	}
}

void AFloorTile::TurnBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA(ARunCharacter::StaticClass()))
	{
		ARunCharacter* RunCharacter = Cast<ARunCharacter>(OtherActor);
		if (RunCharacter)
		{
			RunCharacter->SetCanTurn(true);
		}
	}
}

void AFloorTile::WallOnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor->IsA(ARunCharacter::StaticClass()))
	{
		ARunCharacter* RunCharacter = Cast<ARunCharacter>(OtherActor);
		if (RunCharacter)
		{
			//hitNormal 충돌한 지점에서 수직 백터
			float a = FVector::DotProduct(Hit.Normal,RunCharacter->GetActorForwardVector());
			if (FMath::IsNearlyEqual(a, 1, .01f))
			{
				RunCharacter->Die();
			}
		}
		
	}
}

void AFloorTile::Wall2OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor->IsA(ARunCharacter::StaticClass()))
	{
		ARunCharacter* RunCharacter = Cast<ARunCharacter>(OtherActor);
		if (RunCharacter)
		{
			//hitNormal 충돌한 지점에서 수직 백터
			float a = FVector::DotProduct(Hit.Normal, RunCharacter->GetActorForwardVector());
			if (FMath::IsNearlyEqual(a, 1, .01f))
			{
				RunCharacter->Die();
			}
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

void AFloorTile::CoinCreate()
{
	UWorld* World = GetWorld();
	if (CoinClass && World)
	{
		UE_LOG(LogTemp, Display, TEXT("124"));
		
		for (int32 i = 0; i < 2; i++)
		{
			FVector CoinTrasnform = UKismetMathLibrary::RandomPointInBoundingBox(CoinBox->GetComponentLocation(), CoinBox->GetScaledBoxExtent());
			ACoin* Coin = World->SpawnActor<ACoin>(CoinClass, FTransform(CoinTrasnform));
			if (Coin)
			{
				Coin->SetLifeSpan(15.f);
			}
		}
	}
	
}

FTransform AFloorTile::GetAttachTransform() const
{
	return AttachPoint->GetComponentTransform();
}

void AFloorTile::BlockerCreate()
{
	if (SpawnPoints.Num() > 0)
	{
		const int32 Blockerindex = FMath::RandRange(0, SpawnPoints.Num() - 1);
		Blocker = GetWorld()->SpawnActor<ABlocker>(SpawnBlocker, SpawnPoints[Blockerindex]);
	}
	if(Blocker) Blocker->SetLifeSpan(15.f);
	
}

