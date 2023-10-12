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
	virtual void OnConstruction(const FTransform& Transform) override;

	void SetBlockerSpawnPoints();

private:

	UPROPERTY(VisibleAnywhere)
	class USceneComponent* Scene;

	/** 바닥 */
	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* Floor;

	/** 벽 */
	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* Wall;

	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* Wall2;

	/** 다음 바닥에 스폰해서 이어 붙일 포인트 */
	UPROPERTY(VisibleAnywhere)
	class UArrowComponent* AttachFloorPoint;

	/** 박스 포인트 */
	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* BoxPoint;

	/** 바위*/
	UPROPERTY(VisibleAnywhere)
	class UArrowComponent* BlockerSpawnPoint;

	UPROPERTY(VisibleAnywhere)
	class UArrowComponent* BlockerSpawnPointL;

	UPROPERTY(VisibleAnywhere)
	class UArrowComponent* BlockerSpawnPointR;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class ABlocker> SpawnBlocker;

	UPROPERTY(VisibleAnywhere)
	TArray<FTransform> BlockerSpawnPoints;

	void IsCharacterFacingWall(AActor* OtherActor, const FHitResult& Hit);
		
	/** 코인 */
	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* CoinBox;

	/** 아이템 */
	UPROPERTY(VisibleAnywhere)
	class UArrowComponent* ItemSpawnPoint;

	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<class AItem>> ItemClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class ACoin> CoinClass;

	/** 캐릭터 턴 박스 */
	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* TurnBox;

public:
	FTransform GetAttachFloorTransform() const;
	void BlockerCreate();
	void CoinCreate();
	void ItemSpawn();
};
