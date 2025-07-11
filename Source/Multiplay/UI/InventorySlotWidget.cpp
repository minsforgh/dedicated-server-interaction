#include "InventorySlotWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "ItemDragDropOperation.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "InventoryWidget.h"

void UInventorySlotWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // 초기에 아이템이 없으면 아이콘 숨김
    if (!SlotData.IsValid())
    {
        if (ItemIcon)
        {
            ItemIcon->SetVisibility(ESlateVisibility::Collapsed);
        }
    }
}

// 아이템 데이터 설정
void UInventorySlotWidget::SetSlotData(const FItemData& ItemData)
{   
    SlotData = ItemData;

    if (ItemIcon && SlotData.ItemIcon)
    {
        ItemIcon->SetVisibility(ESlateVisibility::Visible);
        ItemIcon->SetBrushFromTexture(SlotData.ItemIcon);
    }
    else if (ItemIcon)
    {
        ItemIcon->SetVisibility(ESlateVisibility::Collapsed);
    }
}

// 아이템 데이터 제거 (빈 아이템)
void UInventorySlotWidget::ClearSlotData()
{   
    SlotData = FItemData();

    if (ItemIcon)
    {
        ItemIcon->SetVisibility(ESlateVisibility::Collapsed);
    }
}

// 왼쪽 마우스 버튼 클릭 시 (드래그 시작)
FReply UInventorySlotWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    // 아이템이 있고 왼쪽 마우스 버튼 클릭 시 드래그 감지 시작
    if (SlotData.IsValid() && InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
    {
        FEventReply Reply = UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton);
        return Reply.NativeReply;
    }

    return FReply::Unhandled();
}

// 드래그 중
void UInventorySlotWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
    // 아이템이 있을 때만 드래그 허용
    if (SlotData.IsValid())
    {   
        // 드래그 오퍼레이션 생성
        UItemDragDropOperation* DragDropOp = Cast<UItemDragDropOperation>(UWidgetBlueprintLibrary::CreateDragDropOperation(UItemDragDropOperation::StaticClass()));

        if (DragDropOp)
        {
            // 드래그 정보 설정
            DragDropOp->ItemData = SlotData;
            DragDropOp->SourceType = EItemDragSource::Inventory;
            DragDropOp->SourceInventoryIndex = SlotIndex;

            // 드래그 시각화 (간단히 자기 자신을 사용)
            DragDropOp->DefaultDragVisual = this;

            // OutOperation에 할당하여 드래그 시작
            OutOperation = DragDropOp;
        }
    }
}

// 드래그 해제 (드랍) 시
bool UInventorySlotWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{   
    // 아이템 드래그 드롭 정보 확인
    UItemDragDropOperation* ItemDragDrop = Cast<UItemDragDropOperation>(InOperation);
    if (ItemDragDrop && OwningInventoryWidget)
    {
        // 거래 슬롯에서 인벤토리로 드래그한 경우
        if (ItemDragDrop->SourceType == EItemDragSource::TradeSlot)
        {
            // 상위 인벤토리 위젯에 드롭 이벤트 전달
            return OwningInventoryWidget->OnInventorySlotDrop(this, ItemDragDrop);
        }
        // 인벤토리 내에서 슬롯 간 교환
        else if (ItemDragDrop->SourceType == EItemDragSource::Inventory && ItemDragDrop->SourceInventoryIndex != SlotIndex)
        {
            // 상위 인벤토리 위젯에 슬롯 교환 이벤트 전달
            return OwningInventoryWidget->OnInventorySlotSwap(this, ItemDragDrop->SourceInventoryIndex);
        }
    }

    return false;
}
