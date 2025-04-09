// Fill out your copyright notice in the Description page of Project Settings.

#include "Net/UnrealNetwork.h"
#include "MultiplayerController.h"
#include "Blueprint/UserWidget.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "PlayerChatComponent.h"
#include "ChatWidget.h"
#include "MessageWidget.h"

void AMultiplayerController::BeginPlay()
{
	Super::BeginPlay();

	// ��Ʈ�ѷ��� ���� ���ؽ�Ʈ �߰� (�켱���� 0�� ����)
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		if (ControllerMappingContext)
		{
			Subsystem->AddMappingContext(ControllerMappingContext, 0);
		}
	}
}

void AMultiplayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	// Server�� ChatComponent �ʱ�ȭ
	ChatComponent = InPawn->GetComponentByClass<UPlayerChatComponent>();
}

// Server�κ��� Pawn ������ Ȯ�� �� ȣ��
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
	}
}

void AMultiplayerController::ToggleChatInput()
{   
    if (ChatComponent)
    {
        ChatComponent->EnableInput();
    }
}

UChatWidget* AMultiplayerController::CreateChatWidget()
{	
	UChatWidget* ChatWidget = nullptr;

	// ChatWidget�� viewport�� �߰� (client ����)
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

void AMultiplayerController::EnableChatInput()
{	
	FInputModeUIOnly InputMode;
	InputMode.SetWidgetToFocus(ChatComponent->ChatWidgetInstance->ChatInputText->TakeWidget());
	SetInputMode(InputMode);
	bShowMouseCursor = true;;
}

void AMultiplayerController::DisableChatInput(const FString& Message)
{
	bShowMouseCursor = false;
	SetInputMode(FInputModeGameOnly());
}
