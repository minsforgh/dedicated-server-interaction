#include "TradeRequestWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "GameFramework/PlayerState.h"
#include "TradeComponent.h"
#include "MultiplayCharacter.h"
#include "MultiplayerController.h"

void UTradeRequestWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (AcceptButton)
	{
		AcceptButton->OnClicked.AddDynamic(this, &UTradeRequestWidget::OnAcceptButtonClicked);
	}
	if (RejectButton)
	{
		RejectButton->OnClicked.AddDynamic(this, &UTradeRequestWidget::OnRejectButtonClicked);
	}

}

void UTradeRequestWidget::InitializeWidget(AMultiplayCharacter* RequestingPlayer)
{	
    if (!RequestingPlayer)
    {
        return;
    }

    RqPlayer = RequestingPlayer;

    APlayerState* PlayerState = RequestingPlayer->GetPlayerState();
    if (!PlayerState)
    {
        return;
    }

    FString PlayerName = PlayerState->GetPlayerName();
    FString Message = FString::Printf(TEXT("Trade Request from\n %s"), *PlayerName);

    if (RequestTextBlock)
    {
        RequestTextBlock->SetText(FText::FromString(Message));
    }
}

void UTradeRequestWidget::OnAcceptButtonClicked()
{	
    AMultiplayerController* PC = Cast<AMultiplayerController>(GetOwningPlayer());
    if (PC)
    {
        PC->DisableUIMode();
    }

	RemoveFromParent();
    OnWidgetClosed.Broadcast();
    OnRespondToRequest.Broadcast(RqPlayer);
}

void UTradeRequestWidget::OnRejectButtonClicked()
{	
    AMultiplayerController* PC = Cast<AMultiplayerController>(GetOwningPlayer());
    if (PC)
    {
        PC->DisableUIMode();
    }
    
	RemoveFromParent();
    OnWidgetClosed.Broadcast();
    
}

