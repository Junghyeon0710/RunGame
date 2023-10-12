// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "RunCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class ARunCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	/** AKey Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* AAction;

	/** DKey Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* DAction;

public:

	ARunCharacter();

	virtual void Tick(float DeltaTime)override;
	
	// 캐릭터가 사망할 때 호출되는 함수
	void Die();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bhasSpeedBoom = false;

protected:

	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// To add mapping context
	virtual void BeginPlay();

	/** Called for movement input */
	void Move(const FInputActionValue& Value);
	
	/** 키 입력 호출 함수 */
	void AInput();
	void DInput();
	void TurnCorner();

	// 원하는 회전값
	UPROPERTY(VisibleAnywhere)
	FRotator DesiredRotation;

	UPROPERTY(EditAnywhere)
	bool bCanTurn;

	UPROPERTY(BlueprintReadOnly)
	float GameElapsedTime;

private:
	// Input Mapping Context 초기화
	void InitializeInputMappingContext();

	// Character Overlay 초기화
	void InitializeCharacterOverlay();

	// 캐릭터 이동 처리
	void HandleMovement();

	// Character Overlay에 시간 업데이트
	void UpdateTimeOnCharacterOverlay(float DeltaTime);

	//캐릭터가 충돌되었는가
	bool bIsHit = false;

	int32 Coin = 0;

	UPROPERTY()
	class UCharacterOverlay* CharacterOverlay;

	UPROPERTY(EditAnywhere)
	class USoundBase* DieSound;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UDieWidget> DieWidgetClass;

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	FORCEINLINE void SetCanTurn(bool CanTurn) { bCanTurn = CanTurn; }
	FORCEINLINE void SetHit(bool Hit) { bIsHit = Hit; }
	void PlusCoin();
};

