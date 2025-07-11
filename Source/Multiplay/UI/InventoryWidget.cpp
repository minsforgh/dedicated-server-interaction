#include "InventoryWidget.h"
#include "InventoryTypes.h"
#include "Components/UniformGridPanel.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "InventorySlotWidget.h"
#include "InventoryComponent.h"
#include "ItemDragDropOperation.h"
#include "TradeComponent.h"
#include "MultiplayCharacter.h"

void UInventoryWidget::NativeConstruct()
{
    Super::NativeConstruct();

    APlayerController* PC = GetOwningPlayer();
    if (PC)
    {
        // 인벤토리 컴포넌트 참조 가져오기
        InventoryComponent = PC->FindComponentByClass<UInventoryComponent>();
        SetInventoryComponent(InventoryComponent);
    }
}

void UInventoryWidget::SetInventoryComponent(UInventoryComponent* InInventoryComponent)
{
    if (InventoryComponent)
    {
        // 이벤트 바인딩
        InventoryComponent->OnInventoryChanged.AddDynamic(this, &UInventoryWidget::UpdateInventory);
        
        // 초기 인벤토리 업데이트
        InitInventory();
        UpdateInventory();
    }
}

// 초기화
void UInventoryWidget::InitInventory()
{   
    for (int i = 0; i < 18; i++)
    {   
        UInventorySlotWidget* SlotWidget = Cast<UInventorySlotWidget>(SlotGridPanel->GetChildAt(i));
        if (SlotWidget)
        {
            SlotWidget->SlotIndex = i;
            SlotWidget->OwningInventoryWidget = this; // 슬롯 위젯에 소유 인벤토리 위젯 참조 설정
            SlotWidgets.Add(SlotWidget);
        }
    }

    if (MoneyText && InventoryComponent)
    {
        MoneyText->SetText(FText::FromString(FString::FromInt(InventoryComponent->CurrentMoney)));
    }
}

void UInventoryWidget::UpdateInventory()
{
    if (!InventoryComponent || !SlotWidgetClass || !SlotGridPanel)
        return;

    // 모든 슬롯 업데이트
    for (int32 i = 0; i < SlotWidgets.Num(); i++)
    {   
        if (SlotWidgets[i])
        {   
            if (i < InventoryComponent->Inventory.Num())
            {
                SlotWidgets[i]->SetSlotData(InventoryComponent->Inventory[i]);
            }
            else
            {   
                SlotWidgets[i]->ClearSlotData();

            }
        }
    }

    if (MoneyText)
    {
        MoneyText->SetText(FText::FromString(FString::FromInt(InventoryComponent->CurrentMoney)));
    }
}

// 인벤토리 내 슬롯 끼리 교환
bool UInventoryWidget::OnInventorySlotSwap(UInventorySlotWidget* TargetSlot, int32 SourceSlotIndex)
{
    if (!TargetSlot || !InventoryComponent)
        return false;

    // 인벤토리 내에서 슬롯 간 아이템 교환
    int32 TargetSlotIndex = TargetSlot->SlotIndex;

    // 서버에 아이템 교환 요청
    InventoryComponent->ServerSwapItems(SourceSlotIndex, TargetSlotIndex);
    return true; // void 함수 호출 후 성공 반환
}

// 거래 슬롯에서 인벤토리 슬롯에 드랍
bool UInventoryWidget::OnInventorySlotDrop(UInventorySlotWidget* SlotWidget, UItemDragDropOperation* Operation)
{
    if (!SlotWidget || !Operation || !InventoryComponent)
        return false;

    // 거래 슬롯에서 인벤토리로 아이템을 드롭하는 경우
    if (Operation->SourceType == EItemDragSource::TradeSlot)
    {
        // 로컬 플레이어의 캐릭터 찾기
        APlayerController* PC = GetOwningPlayer();
        if (!PC)
            return false;

        APawn* Pawn = PC->GetPawn();
        AMultiplayCharacter* Character = Cast<AMultiplayCharacter>(Pawn);
        if (!Character || !Character->TradeComponent)
            return false;

        // TradeComponent를 통해 서버에 아이템 제거 요청
        Character->TradeComponent->ServerRemoveItemFromTradeSlot(Operation->SourceTradeSlotIndex);
        return true;
    }

    return false;
}
