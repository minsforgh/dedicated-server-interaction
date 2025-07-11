#include "MultiplayCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/Actor.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Net/UnrealNetwork.h"
#include "PlayerChatComponent.h"
#include "TradeComponent.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

AMultiplayCharacter::AMultiplayCharacter()
{
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

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	ChatComponent = CreateDefaultSubobject<UPlayerChatComponent>(TEXT("ChatComponent"));
	TradeComponent = CreateDefaultSubobject<UTradeComponent>(TEXT("TradeComponent"));

}

void AMultiplayCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 1);
		}
	}

}

//////////////////////////////////////////////////////////////////////////
// Input

void AMultiplayCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMultiplayCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMultiplayCharacter::Look);

		//상호작용 바인딩
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &AMultiplayCharacter::Interact);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AMultiplayCharacter::Interact()
{
	// 상호작용 가능한 플레이어 찾기
	AMultiplayCharacter* TargetPlayer = FindInteractablePlayer();

	if (!TargetPlayer)
	{
		UE_LOG(LogTemp, Error, TEXT("TargetPlayer is NULL!"));
		return;
	}

	if (!TargetPlayer->TradeComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("TargetPlayer->TradeComponent is NULL!"));
		return;
	}

	if (!TradeComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("Own TradeComponent is NULL!"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Target Found!!!!"));

		// 서버에 교환 요청 전송
	TradeComponent->ServerRequestTrade(TargetPlayer);
}

AMultiplayCharacter* AMultiplayCharacter::FindInteractablePlayer()
{
	UWorld* World = GetWorld();
	if (!World) return nullptr;

	FVector Center = GetActorLocation();
	float Radius = 200.0f; // 감지 반경

	// 감지 대상 (Pawn 채널)
	FCollisionShape Sphere = FCollisionShape::MakeSphere(Radius);

	TArray<FOverlapResult> Overlaps;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this); // 본인 제외

	bool bHasOverlap = World->OverlapMultiByChannel(
		Overlaps,
		Center,
		FQuat::Identity,
		ECC_Pawn,
		Sphere,
		Params
	);

	if (bHasOverlap)
	{
		for (auto& Result : Overlaps)
		{
			AMultiplayCharacter* OtherPlayer = Cast<AMultiplayCharacter>(Result.GetActor());
			if (OtherPlayer)
			{
				UE_LOG(LogTemp, Log, TEXT("감지된 캐릭터: %s"), *OtherPlayer->GetName());
				return OtherPlayer;
			}
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("반경 내 캐릭터 없음"));
	return nullptr;

}

void AMultiplayCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AMultiplayCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

