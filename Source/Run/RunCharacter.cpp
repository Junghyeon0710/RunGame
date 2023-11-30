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
	Super::Tick(DeltaTime);
	HandleMovement();
	UpdateTimeOnCharacterOverlay(DeltaTime);
}

void ARunCharacter::Die_Implementation()
{
	// 플레이어 컨트롤러의 입력 비활성화
	DisableInput(GetWorld()->GetFirstPlayerController());

	// 피격 상태를 활성화
	bIsHit = true;

	// DieSound가 설정되어 있으면 2D 사운드 재생
	if (DieSound)
	{
		UGameplayStatics::PlaySound2D(this, DieSound);
	}

	// DieWidgetClass가 설정되어 있으면 DieWidget을 생성하여 화면에 추가
	if (DieWidgetClass)
	{
		UDieWidget* DieWidget = CreateWidget<UDieWidget>(GetWorld(), DieWidgetClass);
		if (DieWidget)
		{
			// DieWidget에 시간 설정
			TopScore = Coin + static_cast<int>(GameElapsedTime);
			DieWidget->SetGameElapsedTime(TopScore);
			DieWidget->AddToViewport();
		}
	}

	// CharacterOverlay가 설정되어 있으면 화면에서 제거
	if (CharacterOverlay)
	{
		CharacterOverlay->RemoveFromParent();
	}
}

void ARunCharacter::BeginPlay()
{


	InitializeInputMappingContext();
	InitializeCharacterOverlay();

	Super::BeginPlay();
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

// A 키를 눌렀을 때 호출되는 함수
void ARunCharacter::AInput()
{
	// 회전이 가능한 상태가 아니면 함수 종료
	if (!bCanTurn) return;

	// 원하는 회전값에 -90도의 Yaw 회전을 추가
	DesiredRotation += FRotator(0.f, -90.f, 0.f);

	// 회전을 한 번 수행했으므로 회전 상태를 비활성화
	bCanTurn = false;
}

// D 키를 눌렀을 때 호출되는 함수
void ARunCharacter::DInput()
{
	// 회전이 가능한 상태가 아니면 함수 종료
	if (!bCanTurn) return;

	// 원하는 회전값에 90도의 Yaw 회전을 추가
	DesiredRotation += FRotator(0.f, 90.f, 0.f);

	// 회전을 한 번 수행했으므로 회전 상태를 비활성화
	bCanTurn = false;
}

//코너에서 턴
void ARunCharacter::TurnCorner()
{
	if (Controller)
	{
		// 현재 캐릭터의 회전값 가져오기
		FRotator Rotation = GetControlRotation();

		// 목표 회전값과 현재 회전값 비교
		if (DesiredRotation != Rotation)
		{
			// 보간된 회전값 계산 (매 프레임마다 약간씩 회전을 조절)
			FRotator InterpolatedRotation = FMath::RInterpTo(Rotation, DesiredRotation, GetWorld()->DeltaTimeSeconds, 45.f);

			// 플레이어 컨트롤러를 가져오고 회전값 설정
			APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
			if (PlayerController)
			{
				PlayerController->SetControlRotation(InterpolatedRotation);

			}
		}
	}
	
}

// 초기 입력 매핑 컨텍스트 설정
void ARunCharacter::InitializeInputMappingContext()
{
	APlayerController* PlayerController = Cast<APlayerController>(Controller);
	if (PlayerController)
	{
		UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
		if (Subsystem)
		{
			// 기본 매핑 컨텍스트 추가 (인덱스 0 사용)
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
		else
		{
			// Enhanced Input 서브시스템을 찾을 수 없을 때 오류 메시지 출력
			UE_LOG(LogTemplateCharacter, Error, TEXT("Failed to find Enhanced Input subsystem!"));
		}
	}
	else
	{
		// 플레이어 컨트롤러를 찾을 수 없을 때 오류 메시지 출력
		UE_LOG(LogTemplateCharacter, Error, TEXT("Failed to find PlayerController!"));
	}
}

// CharacterOverlay 초기화
void ARunCharacter::InitializeCharacterOverlay()
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		// 플레이어 컨트롤러에서 HUD 가져오기
		AMyHUD* MyHUD = Cast<AMyHUD>(PlayerController->GetHUD());
		if (MyHUD)
		{
			// CharacterOverlay 설정 및 초기화
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
		// 플레이어 컨트롤러를 찾을 수 없을 때 오류 메시지 출력
		UE_LOG(LogTemplateCharacter, Error, TEXT("Failed to find PlayerController for initializing CharacterOverlay!"));
	}
}

// 캐릭터의 움직임 처리
void ARunCharacter::HandleMovement()
{
	if (!bIsHit)
	{
		// 캐릭터를 현재 바라보는 방향으로 1.0 단위만큼 이동
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
