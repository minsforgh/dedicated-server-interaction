#include "TradeWidget.h"
#include "Components/UniformGridPanel.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "MultiplayCharacter.h"
#include "TradeSlotWidget.h"
#include "TradeManager.h"
#include "InventoryComponent.h"
#include "ItemDragDropOperation.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerState.h"
#include "TradeComponent.h"

void UTradeWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (ConfirmButton)
	{
		ConfirmButton->OnClicked.AddDynamic(this, &UTradeWidget::OnConfirmButtonClicked);
	}

	if (CancelButton)
	{
		CancelButton->OnClicked.AddDynamic(this, &UTradeWidget::OnCancelButtonClicked);
	}
}

void UTradeWidget::InitializeWidget(AMultiplayCharacter* LocalPlayer, AMultiplayCharacter* OtherPlayer)
{   
    LocalPlayerRef = LocalPlayer;
    OtherPlayerRef = OtherPlayer;

    // 교환 매니저 찾기
    if (LocalPlayer && OtherPlayer)
    {
        UWorld* World = GetWorld();
        if (World)
        {
            for (TActorIterator<ATradeManager> It(World); It; ++It)
            {
                ATradeManager* Manager = *It;
                if (Manager && Manager->HasTradeSession(LocalPlayer, OtherPlayer))
                {
                    TradeManagerRef = Manager;
                }
            }
        }
    }
    
    // 플레이어 이름 설정
    if (MyNameText && LocalPlayer)
    {   
        APlayerState* PlayerState = LocalPlayer->GetPlayerState();
        if (PlayerState)
        {   
            FString PlayerName = PlayerState->GetPlayerName();
            MyNameText->SetText(FText::FromString(PlayerName));
        }
        
    }

    if (OtherNameText && OtherPlayer)
    {
        APlayerState* PlayerState = OtherPlayer->GetPlayerState();
        if (PlayerState)
        {
            FString PlayerName = PlayerState->GetPlayerName();
            OtherNameText->SetText(FText::FromString(PlayerName));
        }
    }
   
    // 교환 슬롯 확인
    if (MyTradeSlots)
    {
        MySlotWidgets.Empty();

        // 그리드에서 슬롯 확인
        for (int32 i = 0; i < 3; i++)
        {
            UTradeSlotWidget* SlotWidget = Cast<UTradeSlotWidget>(MyTradeSlots->GetChildAt(i));
            if (SlotWidget)
            {
                SlotWidget->SlotIndex = i;
                SlotWidget->bIsMySlot = true;
                SlotWidget->OwningTradeWidget = this;

                MySlotWidgets.Add(SlotWidget);
            }
        }
    }

    if (OtherTradeSlots)
    {
        OtherSlotWidgets.Empty();

        for (int32 i = 0; i < 3; i++)
        {
            UTradeSlotWidget* SlotWidget = Cast<UTradeSlotWidget>(OtherTradeSlots->GetChildAt(i));
            if (SlotWidget)
            {
                SlotWidget->SlotIndex = i;
                SlotWidget->bIsMySlot = false;
                SlotWidget->OwningTradeWidget = this;

                OtherSlotWidgets.Add(SlotWidget);
            }
        }
    }
}

// 교환 상태 업데이트
void UTradeWidget::UpdateTradeState(const FTradeState& NewState)
{   
    if (!LocalPlayerRef || !TradeManagerRef)
        return;

    // 내 교환 슬롯 업데이트
    bool bIsPlayerA = (LocalPlayerRef == TradeManagerRef->GetPlayerA());
    TArray<FItemData> MyItems = bIsPlayerA ? NewState.PlayerAItems : NewState.PlayerBItems;

    for (int32 i = 0; i < MySlotWidgets.Num(); i++)
    {
        if (i < MyItems.Num() && MySlotWidgets[i])
        {
            if (MyItems[i].ItemID != -1)
            {
                MySlotWidgets[i]->SetSlotData(MyItems[i]);
            }
            else
            {
                MySlotWidgets[i]->ClearSlotData();
            }
        }
    }

    // 상대방 교환 슬롯 업데이트
    TArray<FItemData> OtherItems = bIsPlayerA ? NewState.PlayerBItems : NewState.PlayerAItems;

    for (int32 i = 0; i < OtherSlotWidgets.Num(); i++)
    {
        if (i < OtherItems.Num() && OtherSlotWidgets[i])
        {
            if (OtherItems[i].ItemID != -1)
            {
                OtherSlotWidgets[i]->SetSlotData(OtherItems[i]);
            }
            else
            {
                OtherSlotWidgets[i]->ClearSlotData();
            }
        }
    }

    // 확인 상태 업데이트
    bool bLocalConfirmed = bIsPlayerA ? NewState.bPlayerAConfirmed : NewState.bPlayerBConfirmed;
    bool bOtherConfirmed = bIsPlayerA ? NewState.bPlayerBConfirmed : NewState.bPlayerAConfirmed;

    // 버튼 상태 업데이트
    if (ConfirmButton)
    {
        ConfirmButton->SetIsEnabled(!bLocalConfirmed);
    }
}

// 교환 슬롯에 드랍 처리
void UTradeWidget::OnTradeSlotDrop(UTradeSlotWidget* SlotWidget, UItemDragDropOperation* Operation)
{   
    if (!SlotWidget || !Operation || !LocalPlayerRef)
    {
        return;
    }
      
    UTradeComponent* TradeComp = LocalPlayerRef->TradeComponent;
    if (!TradeComp)
    {
        return;
    }

    if (Operation->SourceType == EItemDragSource::Inventory)
    {
        // 인벤토리에서 TradeSlot으로 아이템 추가
        int32 InventoryIndex = Operation->SourceInventoryIndex;
        int32 TradeSlotIndex = SlotWidget->SlotIndex;

        // 서버에 TradeSlot에 아이템 추가 요청
        TradeComp->ServerAddItemToTradeSlot(InventoryIndex, TradeSlotIndex);
    }
}

// 교환 슬롯간 교환 처리
void UTradeWidget::OnTradeSlotSwap(UTradeSlotWidget* TargetSlot, int32 SourceSlotIndex)
{   
    if (!TargetSlot || !LocalPlayerRef)
        return;

    UTradeComponent* TradeComp = LocalPlayerRef->TradeComponent;
    if (!TradeComp)
    {
        return;
    }
    TradeComp->ServerSwapTradeSlots(SourceSlotIndex, TargetSlot->SlotIndex);
}

//확인 버튼 누를 시
void UTradeWidget::OnConfirmButtonClicked()
{
    if(!LocalPlayerRef)
        return;

    UTradeComponent* TradeComp = LocalPlayerRef->TradeComponent;
    if (!TradeComp)
    {
        return;
    }

    TradeComp->ServerSetTradeConfirmation(true);
}

//취소 버튼 누를 시
void UTradeWidget::OnCancelButtonClicked()
{   
    if(!LocalPlayerRef)
        return;

    UTradeComponent* TradeComp = LocalPlayerRef->TradeComponent;
    if (!TradeComp)
    {
        return;
    }

    TradeComp->ServerCancelTradeSession();
}
