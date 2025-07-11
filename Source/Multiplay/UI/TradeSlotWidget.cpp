#include "TradeSlotWidget.h"
#include "Components/Image.h"
#include "ItemDragDropOperation.h"
#include "TradeWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

void UTradeSlotWidget::NativeConstruct()
{
    Super::NativeConstruct();

    ClearSlotData();

}

void UTradeSlotWidget::SetSlotData(const FItemData& InItemData)
{
	SlotData = InItemData;

	if (ItemIcon && SlotData.ItemIcon)
	{
        ItemIcon->SetBrushFromTexture(SlotData.ItemIcon);
        ItemIcon->SetVisibility(ESlateVisibility::Visible);
	}
}

void UTradeSlotWidget::ClearSlotData()
{
    // 슬롯 데이터 초기화
    SlotData = FItemData();

    // 아이콘 숨김
    if (ItemIcon)
    {
        ItemIcon->SetVisibility(ESlateVisibility::Hidden);
    }
}

// 왼쪽 마우스 클릭시 드래그 시작
FReply UTradeSlotWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    // 내 슬롯이며 아이템이 있고 왼쪽 마우스 버튼 클릭 시만 드래그 허용
    if (bIsMySlot && SlotData.ItemID != -1 && InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
    {
        FEventReply Reply = UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton);
        return Reply.NativeReply;
    }

    return FReply::Unhandled();
}

// 드래그 중
void UTradeSlotWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
    // 내 슬롯이며 아이템이 있을 때만 드래그 허용
    if (bIsMySlot && SlotData.ItemID != -1)
    {
        // 드래그 오퍼레이션 생성
        UItemDragDropOperation* DragDropOp = Cast<UItemDragDropOperation>(UWidgetBlueprintLibrary::CreateDragDropOperation(UItemDragDropOperation::StaticClass()));
        if (DragDropOp)
        {
            // 드래그 정보 설정
            DragDropOp->ItemData = SlotData;
            DragDropOp->SourceType = EItemDragSource::TradeSlot;
            DragDropOp->SourceTradeSlotIndex = SlotIndex;

            // 드래그 시각화 (간단히 자기 자신을 사용)
            DragDropOp->DefaultDragVisual = this;

            // OutOperation에 할당하여 드래그 시작
            OutOperation = DragDropOp;
        }
    }
}

// 드랍 시
bool UTradeSlotWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{   
    // 내 슬롯일 때만 드롭 허용
    if (bIsMySlot)
    {
        UItemDragDropOperation* ItemDragDrop = Cast<UItemDragDropOperation>(InOperation);
        if (ItemDragDrop && OwningTradeWidget)
        {   
            if (ItemDragDrop->SourceType == EItemDragSource::Inventory)
            {   
                // 인벤토리에서 교환 슬롯으로 아이템 추가
                OwningTradeWidget->OnTradeSlotDrop(this, ItemDragDrop);
                return true;
            }
            else if (ItemDragDrop->SourceType == EItemDragSource::TradeSlot && ItemDragDrop->SourceTradeSlotIndex != SlotIndex)
            {   
                // 다른 교환 슬롯에서 드래그한 경우 (교환 슬롯 간 교환)
                OwningTradeWidget->OnTradeSlotSwap(this, ItemDragDrop->SourceTradeSlotIndex);
                return true;
            }
        }
    }
    return false;
}
