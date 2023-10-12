// Copyright Epic Games, Inc. All Rights Reserved.

#include "RunCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Kismet/GameplayStatics.h"
#include "MyHUD.h"
#include "Characteroverlay.h"
#include "DieWidget.h"


DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// ARunCharacter

ARunCharacter::ARunCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller
	CameraBoom->SocketOffset = FVector(0.f, 0.f, 100.f);
	
	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
	
	bUseControllerRotationYaw = true;
	GetCharacterMovement()->bOrientRotationToMovement = false;
	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void ARunCharacter::Tick(float DeltaTime)
{
	HandleMovement();
	UpdateTimeOnCharacterOverlay(DeltaTime);
}

// ĳ���Ͱ� ����� �� ȣ��Ǵ� �Լ�
void ARunCharacter::Die()
{
	// �÷��̾� ��Ʈ�ѷ��� �Է� ��Ȱ��ȭ
	DisableInput(GetWorld()->GetFirstPlayerController());

	// �ǰ� ���¸� Ȱ��ȭ
	bIsHit = true;

	// DieSound�� �����Ǿ� ������ 2D ���� ���
	if (DieSound)
	{
		UGameplayStatics::PlaySound2D(this, DieSound);
	}

	// DieWidgetClass�� �����Ǿ� ������ DieWidget�� �����Ͽ� ȭ�鿡 �߰�
	if (DieWidgetClass)
	{
		UDieWidget* DieWidget = CreateWidget<UDieWidget>(GetWorld(), DieWidgetClass);
		if (DieWidget)
		{
			// DieWidget�� �ð� ����
			DieWidget->SetGameElapsedTime(GameElapsedTime);
			DieWidget->AddToViewport();
		}
	}

	// CharacterOverlay�� �����Ǿ� ������ ȭ�鿡�� ����
	if (CharacterOverlay)
	{
		CharacterOverlay->RemoveFromParent();
	}
}

void ARunCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
	InitializeInputMappingContext();
	InitializeCharacterOverlay();
}

void ARunCharacter::PlusCoin()
{
	if (CharacterOverlay)
	{
		CharacterOverlay->SetCoin(++Coin);
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void ARunCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ARunCharacter::Move);
		EnhancedInputComponent->BindAction(AAction, ETriggerEvent::Started, this, &ARunCharacter::AInput);
		EnhancedInputComponent->BindAction(DAction, ETriggerEvent::Started, this, &ARunCharacter::DInput);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}


void ARunCharacter::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();
	if (Controller && !bCanTurn)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(RightDirection, MovementVector.X);
	}
}

// A Ű�� ������ �� ȣ��Ǵ� �Լ�
void ARunCharacter::AInput()
{
	// ȸ���� ������ ���°� �ƴϸ� �Լ� ����
	if (!bCanTurn) return;

	// ���ϴ� ȸ������ -90���� Yaw ȸ���� �߰�
	DesiredRotation += FRotator(0.f, -90.f, 0.f);

	// ȸ���� �� �� ���������Ƿ� ȸ�� ���¸� ��Ȱ��ȭ
	bCanTurn = false;
}

// D Ű�� ������ �� ȣ��Ǵ� �Լ�
void ARunCharacter::DInput()
{
	// ȸ���� ������ ���°� �ƴϸ� �Լ� ����
	if (!bCanTurn) return;

	// ���ϴ� ȸ������ 90���� Yaw ȸ���� �߰�
	DesiredRotation += FRotator(0.f, 90.f, 0.f);

	// ȸ���� �� �� ���������Ƿ� ȸ�� ���¸� ��Ȱ��ȭ
	bCanTurn = false;
}

//�ڳʿ��� ��
void ARunCharacter::TurnCorner()
{
	if (Controller)
	{
		// ���� ĳ������ ȸ���� ��������
		FRotator Rotation = GetControlRotation();

		// ��ǥ ȸ������ ���� ȸ���� ��
		if (DesiredRotation != Rotation)
		{
			// ������ ȸ���� ��� (�� �����Ӹ��� �ణ�� ȸ���� ����)
			FRotator InterpolatedRotation = FMath::RInterpTo(Rotation, DesiredRotation, GetWorld()->DeltaTimeSeconds, 45.f);

			// �÷��̾� ��Ʈ�ѷ��� �������� ȸ���� ����
			APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
			if (PlayerController)
			{
				PlayerController->SetControlRotation(InterpolatedRotation);
			}
		}
	}
}

// �ʱ� �Է� ���� ���ؽ�Ʈ ����
void ARunCharacter::InitializeInputMappingContext()
{
	APlayerController* PlayerController = Cast<APlayerController>(Controller);
	if (PlayerController)
	{
		UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
		if (Subsystem)
		{
			// �⺻ ���� ���ؽ�Ʈ �߰� (�ε��� 0 ���)
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
		else
		{
			// Enhanced Input ����ý����� ã�� �� ���� �� ���� �޽��� ���
			UE_LOG(LogTemplateCharacter, Error, TEXT("Failed to find Enhanced Input subsystem!"));
		}
	}
	else
	{
		// �÷��̾� ��Ʈ�ѷ��� ã�� �� ���� �� ���� �޽��� ���
		UE_LOG(LogTemplateCharacter, Error, TEXT("Failed to find PlayerController!"));
	}
}

// CharacterOverlay �ʱ�ȭ
void ARunCharacter::InitializeCharacterOverlay()
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		// �÷��̾� ��Ʈ�ѷ����� HUD ��������
		AMyHUD* MyHUD = Cast<AMyHUD>(PlayerController->GetHUD());
		if (MyHUD)
		{
			// CharacterOverlay ���� �� �ʱ�ȭ
			CharacterOverlay = MyHUD->GetCharacterOverlay();
			if (CharacterOverlay)
			{
				CharacterOverlay->SetCoin(Coin);
				CharacterOverlay->SetTime(GetWorld()->DeltaRealTimeSeconds);
			}
		}
	}
	else
	{
		// �÷��̾� ��Ʈ�ѷ��� ã�� �� ���� �� ���� �޽��� ���
		UE_LOG(LogTemplateCharacter, Error, TEXT("Failed to find PlayerController for initializing CharacterOverlay!"));
	}
}

// ĳ������ ������ ó��
void ARunCharacter::HandleMovement()
{
	if (!bIsHit)
	{
		// ĳ���͸� ���� �ٶ󺸴� �������� 1.0 ������ŭ �̵�
		AddMovementInput(GetActorForwardVector(), 1.f);
		
		Move(0.f);

		TurnCorner();
	}
}

void ARunCharacter::UpdateTimeOnCharacterOverlay(float DeltaTime)
{
	if (CharacterOverlay)
	{
		CharacterOverlay->SetTime(GameElapsedTime += DeltaTime);
	}
}
