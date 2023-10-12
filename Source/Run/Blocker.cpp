// Fill out your copyright notice in the Description page of Project Settings.


#include "Blocker.h"
#include <Components/SphereComponent.h>
#include "RunCharacter.h"
#include "Particles/ParticleSystem.h"
#include <Kismet/GameplayStatics.h>
// Sets default values
ABlocker::ABlocker()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	SetRootComponent(SphereCollision);
	SphereCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
	SphereCollision->bHasPerInstanceHitProxies = true;
	
	Block = CreateDefaultSubobject<UStaticMeshComponent>("Block");
	Block->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ABlocker::BeginPlay()
{
	Super::BeginPlay();

	SphereCollision->OnComponentBeginOverlap.AddDynamic(this, &ABlocker::SphereBeginOverlap);
}

void ABlocker::SphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA(ARunCharacter::StaticClass()) && !bIsHitting)
	{
		ARunCharacter* RunCharacter = Cast<ARunCharacter>(OtherActor);

		bIsHitting = true;

		if (BlockSound)
		{
			UGameplayStatics::PlaySound2D(this, BlockSound);
		}

		if (BlockParticle)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BlockParticle, SphereCollision->GetComponentLocation(), SphereCollision->GetComponentRotation(), FVector(5.f));

		}

		// RunCharacter의 'bSpeedBoom' 플래그를 확인합니다.
		if (!RunCharacter->bhasSpeedBoom)
		{
			// 입력 비활성화, Die 함수 호출, 'bHit' 플래그 설정
			RunCharacter->DisableInput(GetWorld()->GetFirstPlayerController());
			RunCharacter->Die();
			RunCharacter->SetHit(true);
			return;
		}

		Destroy();
	}
}


