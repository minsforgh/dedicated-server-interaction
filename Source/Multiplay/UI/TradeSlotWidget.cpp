#include "TradeSlotWidget.h"
#include "Components/Image.h"
#include "ItemDragDropOperation.h"
#include "TradeWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

void UTradeSlotWidget::NativeConstruct()
{
    Super::NativeConstruct();
    ClearSlotData(); // 초기에 빈 슬롯으로 설정
}

// 교환 슬롯에 아이템 데이터 설정
void UTradeSlotWidget::SetSlotData(const FItemData& InItemData)
{
    SlotData = InItemData;

    if (ItemIcon && SlotData.ItemIcon)
    {
        ItemIcon->SetBrushFromTexture(SlotData.ItemIcon);
        ItemIcon->SetVisibility(ESlateVisibility::Visible);
    }
}

// 교환 슬롯 초기화 (빈 슬롯으로 만들기)
void UTradeSlotWidget::ClearSlotData()
{
    SlotData = FItemData(); // 빈 아이템 데이터로 초기화

    if (ItemIcon)
    {
        ItemIcon->SetVisibility(ESlateVisibility::Hidden);
    }
}

// 마우스 클릭 시 드래그 시작 조건 검사 (내 슬롯이고 아이템이 있을 때만)
FReply UTradeSlotWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    // 내 슬롯이고, 아이템이 있고, 왼쪽 마우스 버튼 클릭 시에만 드래그 허용
    if (bIsMySlot && SlotData.ItemID != -1 && InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
    {
        FEventReply Reply = UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton);
        return Reply.NativeReply;
    }

    return FReply::Unhandled();
}

// 드래그 감지 시 교환 슬롯용 드래그 오퍼레이션 생성
void UTradeSlotWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
    // 내 슬롯이고 아이템이 있을 때만 드래그 허용
    if (bIsMySlot && SlotData.ItemID != -1)
    {
        UItemDragDropOperation* DragDropOp = Cast<UItemDragDropOperation>(
            UWidgetBlueprintLibrary::CreateDragDropOperation(UItemDragDropOperation::StaticClass()));

        if (DragDropOp)
        {
            // 교환 슬롯 드래그 데이터 설정
            DragDropOp->ItemData = SlotData;
            DragDropOp->SourceType = EItemDragSource::TradeSlot;
            DragDropOp->SourceTradeSlotIndex = SlotIndex;

            // 독립적인 드래그 비주얼 생성
            if (ItemIcon && SlotData.ItemIcon)
            {
                UImage* DragVisualWidget = NewObject<UImage>(this);
                DragVisualWidget->SetBrushFromTexture(SlotData.ItemIcon);
                // 원래 슬롯과 유사한 크기로 설정 (더 크게)
                DragVisualWidget->SetDesiredSizeOverride(FVector2D(100.0f, 100.0f));
                DragDropOp->DefaultDragVisual = DragVisualWidget;
                DragDropOp->Pivot = EDragPivot::CenterCenter;
            }
            else
            {
                DragDropOp->DefaultDragVisual = this;
                DragDropOp->Pivot = EDragPivot::CenterCenter;
            }

            OutOperation = DragDropOp;
        }
    }
}

// 다른 위젯에서 이 교환 슬롯으로 드롭했을 때 처리
bool UTradeSlotWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
    // 내 슬롯일 때만 드롭 허용 (상대방 슬롯에는 드롭 불가)
    if (bIsMySlot)
    {
        UItemDragDropOperation* ItemDragDrop = Cast<UItemDragDropOperation>(InOperation);
        if (ItemDragDrop && OwningTradeWidget)
        {
            // 인벤토리에서 교환 슬롯으로 아이템 추가
            if (ItemDragDrop->SourceType == EItemDragSource::Inventory)
            {
                OwningTradeWidget->OnTradeSlotDrop(this, ItemDragDrop);
                return true;
            }
            // 다른 교환 슬롯에서 드래그한 경우 (교환 슬롯 간 교환)
            else if (ItemDragDrop->SourceType == EItemDragSource::TradeSlot && ItemDragDrop->SourceTradeSlotIndex != SlotIndex)
            {
                OwningTradeWidget->OnTradeSlotSwap(this, ItemDragDrop->SourceTradeSlotIndex);
                return true;
            }
        }
    }
    return false;
}