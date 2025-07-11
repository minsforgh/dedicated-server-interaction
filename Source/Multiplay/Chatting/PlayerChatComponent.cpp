#include "PlayerChatComponent.h"
#include "Components/EditableText.h"
#include "GameFramework/PlayerState.h"
#include "Net/UnrealNetwork.h"
#include "MultiplayerController.h"
#include "ChatWidget.h"
#include "MessageWidget.h"

UPlayerChatComponent::UPlayerChatComponent()
{
	SetIsReplicatedByDefault(true);
}

void UPlayerChatComponent::EnableInput()
{
	UEditableText* ChatInputText = ChatWidgetInstance->ChatInputText;

	if (!ChatInputText)
	{
		UE_LOG(LogTemp, Warning, TEXT("No Chat Input Box"));
		return;
	}

	// 채팅창 활성화
	ChatInputText->SetVisibility(ESlateVisibility::Visible);

	// 게임 입력을 UI로 전환
	OnInputEnabled.Broadcast();
}

// Server에서 각 Client에 Client RPC (ReceiveMessage)
void UPlayerChatComponent::ServerSendMessage_Implementation(const FString& Message)
{	
	if (Message.IsEmpty()) return;

	APawn* Pawn = Cast<APawn>(GetOwner());
	APlayerController* SenderPC = Cast<APlayerController>(Pawn->GetController());
	APlayerState* PlayerState = SenderPC->GetPlayerState<APlayerState>();

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		AMultiplayerController* PC = Cast<AMultiplayerController>(It->Get());
		if (PC)
		{	
			UPlayerChatComponent* ChatComponent = PC->ChatComponent;
			if (ChatComponent && PlayerState)
			{
				FString SenderName = PlayerState->GetPlayerName();
				ChatComponent->ClientReceiveMessage(SenderName, Message);
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("ChatComponent is null on PC: %s"), *PC->GetName());
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Can't Found PC"));
		}
	}
}

bool UPlayerChatComponent::ServerSendMessage_Validate(const FString& Message)
{
	return true;
}

//Client에서 Message 처리
void UPlayerChatComponent::ClientReceiveMessage_Implementation(const FString& SenderName, const FString& Message)
{
	UE_LOG(LogTemp, Warning, TEXT("Client Received  %s: %s"), *SenderName, *Message);

	AMultiplayerController* PC = Cast<AMultiplayerController>(Cast<APawn>(GetOwner())->GetController());
	if (PC && ChatWidgetInstance)
	{
		UMessageWidget* MessageWidgetInstance = PC->CreateMessageWidget();
			if (MessageWidgetInstance)
			{	
				MessageWidgetInstance->InitializeMessageWidget(SenderName, Message);
				ChatWidgetInstance->MessageBox->AddChild(MessageWidgetInstance);
				ChatWidgetInstance->MessageBox->ScrollToEnd();
			}
	}
}