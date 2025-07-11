#include "MultiplayerController.h"
#include "Net/UnrealNetwork.h"
#include "Blueprint/UserWidget.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "PlayerChatComponent.h"
#include "ChatWidget.h"
#include "MessageWidget.h"
#include "InventoryWidget.h"
#include "InventoryComponent.h"

void AMultiplayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMultiplayerController, InventoryComponent);
	DOREPLIFETIME(AMultiplayerController, ChatComponent);
}

void AMultiplayerController::BeginPlay()
{
	Super::BeginPlay();

	// 컨트롤러의 매핑 컨텍스트 추가 (우선순위 0로 설정)
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		if (ControllerMappingContext)
		{
			Subsystem->AddMappingContext(ControllerMappingContext, 0);
		}
	}

	InventoryComponent = GetComponentByClass<UInventoryComponent>();
}

void AMultiplayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	// Server의 ChatComponent 초기화
	ChatComponent = InPawn->GetComponentByClass<UPlayerChatComponent>();
}

// Server로부터 Pawn 소유권 확인 시 호출
void AMultiplayerController::AcknowledgePossession(APawn* InPawn)
{	
	Super::AcknowledgePossession(InPawn);

	ChatComponent = InPawn->GetComponentByClass<UPlayerChatComponent>();

	if (!ChatComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("ChatComp is NULL!"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ChatComp Found: %s"), *ChatComponent->GetFullName());
	}

	if (ChatComponent)
	{
		ChatComponent->ChatWidgetInstance = CreateChatWidget();
		ChatWidgetInstance = ChatComponent->ChatWidgetInstance;

		ChatComponent->OnInputEnabled.AddDynamic(this, &AMultiplayerController::EnableChatInput);
		ChatComponent->ChatWidgetInstance->OnChatMessageSent.AddDynamic(this, &AMultiplayerController::DisableChatInput);
		ChatComponent->ChatWidgetInstance->OnChatMessageSent.AddDynamic(ChatComponent, &UPlayerChatComponent::ServerSendMessage);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Cannot Get ChatComponent"));
	}

	InventoryWidgetInstance = CreateInventoryWidget();
	bIsInventoryVisible = false;
	InventoryWidgetInstance->AddToViewport();
	InventoryWidgetInstance->SetVisibility(ESlateVisibility::Hidden);
}

void AMultiplayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* EnhancedInputComp = CastChecked<UEnhancedInputComponent>(InputComponent))
	{
		if (EnterChatAction)
		{
			EnhancedInputComp->BindAction(EnterChatAction, ETriggerEvent::Started, this, &AMultiplayerController::ToggleChatInput);
		}
		if (ToggleInventoryAction)
		{
			EnhancedInputComp->BindAction(ToggleInventoryAction, ETriggerEvent::Started, this, &AMultiplayerController::ToggleInventory);
		}
	}
}

void AMultiplayerController::ToggleChatInput()
{   
    if (ChatComponent)
    {
        ChatComponent->EnableInput();
    }
}

// 인벤토리 활성/비활성 토글
void AMultiplayerController::ToggleInventory()
{	
	if (HasAuthority())
	{
		if (InventoryComponent)
		{
			UE_LOG(LogTemp, Warning, TEXT("Inventory Size : %d"), InventoryComponent->Inventory.Num());
		}
	}
	if (InventoryWidgetInstance)
	{	
		
		if (bIsInventoryVisible)
		{
			// 인벤토리 숨기기
			InventoryWidgetInstance->SetVisibility(ESlateVisibility::Hidden);
			bIsInventoryVisible = false;

			// 마우스 커서 숨기고 게임 입력 모드로 설정
			SetShowMouseCursor(false);
			FInputModeGameOnly InputMode;
			SetInputMode(InputMode);

			SetIgnoreMoveInput(false);
			SetIgnoreLookInput(false);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("ToggleInventory Called"));
			// 인벤토리 표시
			InventoryWidgetInstance->SetVisibility(ESlateVisibility::Visible);
			bIsInventoryVisible = true;

			// 마우스 커서 표시하고 UI 입력 모드로 설정
			SetShowMouseCursor(true);
			FInputModeGameAndUI InputMode;
			SetInputMode(InputMode);

			SetIgnoreMoveInput(true);
			SetIgnoreLookInput(true);
			
		}
	}
	
}

// 채팅창 UI 위젯 생성
UChatWidget* AMultiplayerController::CreateChatWidget()
{	
	UChatWidget* ChatWidget = nullptr;

	// ChatWidget을 viewport에 추가 (client 한정)
	if (IsLocalPlayerController())
	{
		if (ChatWidgetClass)
		{
			ChatWidget = CreateWidget<UChatWidget>(this, ChatWidgetClass);

			if (ChatWidget)
			{
				ChatWidget->AddToViewport();
				ChatWidget->SetVisibility(ESlateVisibility::Visible);
			}
		}
	}
	return ChatWidget;
}

// 메세지 UI 위젯 생성
UMessageWidget* AMultiplayerController::CreateMessageWidget()
{
	UMessageWidget* MsgWidget = nullptr;

	if (IsLocalPlayerController())
	{
		if (MessageWidgetClass)
		{
			MsgWidget = CreateWidget<UMessageWidget>(this, MessageWidgetClass);
		}
	}
	return MsgWidget;
}

// 인벤토리 UI 위젯 생성
UInventoryWidget* AMultiplayerController::CreateInventoryWidget()
{
	UInventoryWidget* InventoryWidget = nullptr;

	if (IsLocalPlayerController())
	{
		if (InventoryWidgetClass)
		{
			InventoryWidget = CreateWidget<UInventoryWidget>(this, InventoryWidgetClass);
		}
	}
	return InventoryWidget;
}

// 채팅창 입력 활성화
void AMultiplayerController::EnableChatInput()
{	
	FInputModeUIOnly InputMode;
	InputMode.SetWidgetToFocus(ChatComponent->ChatWidgetInstance->ChatInputText->TakeWidget());
	SetInputMode(InputMode);
	bShowMouseCursor = true;;
}

//채팅창 입력 비활성화
void AMultiplayerController::DisableChatInput(const FString& Message)
{
	bShowMouseCursor = false;
	SetInputMode(FInputModeGameOnly());
}

// UI 모드 활성화 (인벤토리, 교환)
void AMultiplayerController::EnableUIMode()
{
	FInputModeUIOnly InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

	SetInputMode(InputMode);
	bShowMouseCursor = true;

	SetIgnoreMoveInput(true);
	SetIgnoreLookInput(true);
	
}

// UI모드 비활성화
void AMultiplayerController::DisableUIMode()
{
	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);

	bShowMouseCursor = false;

	SetIgnoreMoveInput(false);
	SetIgnoreLookInput(false);
}
