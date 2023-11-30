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
	
	// ĳ���Ͱ� ����� �� ȣ��Ǵ� �Լ�
	UFUNCTION(BlueprintCallable,BlueprintNativeEvent)
	void Die();
	virtual void Die_Implementation();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bhasSpeedBoom = false;

	UPROPERTY(BlueprintReadWrite)
	bool bHasMagnet = false;

protected:

	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// To add mapping context
	virtual void BeginPlay();

	/** Called for movement input */
	void Move(const FInputActionValue& Value);
	
	/** Ű �Է� ȣ�� �Լ� */
	void AInput();
	void DInput();
	void TurnCorner();

	// ���ϴ� ȸ����
	UPROPERTY(VisibleAnywhere)
	FRotator DesiredRotation;

	UPROPERTY(EditAnywhere)
	bool bCanTurn;

	UPROPERTY(BlueprintReadOnly)
	float GameElapsedTime;

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite)
	int32 TopScore =0;

	UPROPERTY(BlueprintReadOnly)
	int32 Coin = 0;

	UPROPERTY(BlueprintReadOnly)
	class UCharacterOverlay* CharacterOverlay;

private:
	// Input Mapping Context �ʱ�ȭ
	void InitializeInputMappingContext();

	// Character Overlay �ʱ�ȭ
	void InitializeCharacterOverlay();

	// ĳ���� �̵� ó��
	void HandleMovement();

	// Character Overlay�� �ð� ������Ʈ
	void UpdateTimeOnCharacterOverlay(float DeltaTime);

	//ĳ���Ͱ� �浹�Ǿ��°�
	bool bIsHit = false;

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

