// Fill out your copyright notice in the Description page of Project Settings.


#include "FloorTile.h"
#include <Components/ArrowComponent.h>
#include <Components/BoxComponent.h>
#include "RunGameMode.h"
#include "RunCharacter.h"
#include "Blocker.h"
#include "Kismet/KismetMathLibrary.h"
#include "Coin.h"
#include "Item.h"

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

	AttachFloorPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("AttachFloorPoint"));
	AttachFloorPoint->SetupAttachment(RootComponent);

	BoxPoint = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	BoxPoint->SetupAttachment(RootComponent);

	BlockerSpawnPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("BlockerSpawnPoint"));
	BlockerSpawnPoint->SetupAttachment(RootComponent);

	BlockerSpawnPointL = CreateDefaultSubobject<UArrowComponent>(TEXT("BlockerSpawnPointL"));
	BlockerSpawnPointL->SetupAttachment(RootComponent);

	BlockerSpawnPointR = CreateDefaultSubobject<UArrowComponent>(TEXT("BlocekrSpawnPointR"));
	BlockerSpawnPointR->SetupAttachment(RootComponent);

	CoinBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CoinBox"));
	CoinBox->SetupAttachment(RootComponent);

	TurnBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TurnBox"));
	TurnBox->SetupAttachment(RootComponent);
	TurnBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	TurnBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

	ItemSpawnPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("ItemSpawnPoint"));
	ItemSpawnPoint->SetupAttachment(RootComponent);
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
			SetLifeSpan(3.f);
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
	IsCharacterFacingWall(OtherActor, Hit);
}

void AFloorTile::Wall2OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	IsCharacterFacingWall(OtherActor, Hit);
}

void AFloorTile::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	SetBlockerSpawnPoints();
}

void AFloorTile::SetBlockerSpawnPoints()
{
	BlockerSpawnPoints.Add(BlockerSpawnPoint->GetComponentTransform());
	BlockerSpawnPoints.Add(BlockerSpawnPointL->GetComponentTransform());
	BlockerSpawnPoints.Add(BlockerSpawnPointR->GetComponentTransform());
}

void AFloorTile::CoinCreate()
{
	UWorld* World = GetWorld();
	if (!CoinClass || !World)
	{
		return;
	}

	for (int32 i = 0; i < 2; i++)
	{
		FVector CoinTrasnform = UKismetMathLibrary::RandomPointInBoundingBox(CoinBox->GetComponentLocation(), CoinBox->GetScaledBoxExtent());
		
		ACoin* Coin = World->SpawnActor<ACoin>(CoinClass, FTransform(CoinTrasnform));
		if (Coin)
		{
			// ������ ���� ���� (��: 20��)
			Coin->SetLifeSpan(20.f);
		}
	}
}

void AFloorTile::ItemSpawn()
{
	if (ItemClass.Num() > 0)
	{
		const int32 ItemIndex = FMath::RandRange(0, ItemClass.Num() - 1);
		AItem* Item = GetWorld()->SpawnActor<AItem>(ItemClass[ItemIndex], ItemSpawnPoint->GetComponentTransform());
		if (Item) Item->SetLifeSpan(20.f);

	}
}

void AFloorTile::IsCharacterFacingWall(AActor* OtherActor, const FHitResult& Hit)
{
	if (OtherActor && OtherActor->IsA(ARunCharacter::StaticClass()))
	{
		ARunCharacter* RunCharacter = Cast<ARunCharacter>(OtherActor);
		if (RunCharacter)
		{
			// �浹 �������� ���� ����� ĳ������ ���� ���� ������ ���� ���
			float DotProduct = FVector::DotProduct(Hit.Normal, RunCharacter->GetActorForwardVector());

			// �� ������ ������ ���� 1�� ��� (���� ĳ������ ������ ���� ��ġ)
			if (FMath::IsNearlyEqual(DotProduct, 1, .01f))
			{
				// ĳ���͸� ���� ó��
				RunCharacter->Die();
			}
		}
	}
}

FTransform AFloorTile::GetAttachFloorTransform() const
{
	return BlockerSpawnPoint->GetComponentTransform();
}

void AFloorTile::BlockerCreate()
{
	if (BlockerSpawnPoints.Num() > 0)
	{
		const int32 Blockerindex = FMath::RandRange(0, BlockerSpawnPoints.Num() - 1);
		ABlocker* Blocker = GetWorld()->SpawnActor<ABlocker>(SpawnBlocker, BlockerSpawnPoints[Blockerindex]);
		if (Blocker) Blocker->SetLifeSpan(20.f);
	}
}

