// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FloorTile.generated.h"

UCLASS()
class RUN_API AFloorTile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFloorTile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void BoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void TurnBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void WallOnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	void Wall2OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void OnConstruction(const FTransform& Transform) override;

	void SetSpawnPoints();
	
private:

	UPROPERTY(VisibleAnywhere)
	class USceneComponent* Scene;

	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* Floor;

	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* Wall;

	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* Wall2;

	UPROPERTY(VisibleAnywhere)
	class UArrowComponent* AttachPoint;

	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* BoxPoint;

	UPROPERTY(VisibleAnywhere)
	class UArrowComponent* SpawnPoint;

	UPROPERTY(VisibleAnywhere)
	class UArrowComponent* SpawnPointL;

	UPROPERTY(VisibleAnywhere)
	class UArrowComponent* SpawnPointR;

	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* CoinBox;

	UPROPERTY(VisibleAnywhere)
	TArray<FTransform> SpawnPoints;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class ABlocker> SpawnBlocker;

	UPROPERTY()
	class ABlocker* Blocker;



	UPROPERTY(EditAnywhere)
	TSubclassOf<class ACoin> CoinClass;

	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* TurnBox;

public:
	FTransform GetAttachTransform() const;
	void BlockerCreate();
	void CoinCreate();
};
