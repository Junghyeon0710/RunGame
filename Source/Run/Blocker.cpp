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
	//SphereCollision->OnComponentHit.AddDynamic(this, &ABlocker::OnHit);
}

void ABlocker::SphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA(ARunCharacter::StaticClass()) && !bHit)
	{
		ARunCharacter* RunCharacter = Cast<ARunCharacter>(OtherActor);

		bHit = true;
		UGameplayStatics::PlaySound2D(this, BlockSound);
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BlockParticle, SphereCollision->GetComponentLocation(), SphereCollision->GetComponentRotation(), FVector(5.f));

		if (!RunCharacter->bSpeedBoom)
		{
			RunCharacter->DisableInput(GetWorld()->GetFirstPlayerController());
			RunCharacter->Die();
			RunCharacter->SetHit(true);
			return;
		}
		Destroy();
	}
}

void ABlocker::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	
}

// Called every frame
void ABlocker::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

