#include "InventorySlotWidget.h"
#include "Components/Image.h"
#include "ItemDragDropOperation.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "InventoryWidget.h"

void UInventorySlotWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // 초기에 아이템이 없으면 아이콘 숨기기
    if (!SlotData.IsValid())
    {
        if (ItemIcon)
        {
            ItemIcon->SetVisibility(ESlateVisibility::Collapsed);
        }
    }
}

// 슬롯에 아이템 데이터 설정 및 UI 업데이트
void UInventorySlotWidget::SetSlotData(const FItemData& ItemData)
{
    SlotData = ItemData;

    // 아이템 아이콘 표시/숨기기
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

// 슬롯 데이터 초기화 (빈 슬롯으로 만들기)
void UInventorySlotWidget::ClearSlotData()
{
    SlotData = FItemData();

    if (ItemIcon)
    {
        ItemIcon->SetVisibility(ESlateVisibility::Collapsed);
    }
}

// 슬롯의 모든 상태를 초기화 (드래그 상태, 타이머 등 리셋)
void UInventorySlotWidget::ResetSlotState()
{
    bIsDragging = false;
    bIsWaitingForServerResponse = false;

    // 진행 중인 타이머 정리
    if (ServerResponseTimer.IsValid())
    {
        GetWorld()->GetTimerManager().ClearTimer(ServerResponseTimer);
    }

    // 위젯 상태 정상화
    SetIsEnabled(true);
    SetVisibility(ESlateVisibility::Visible);
}

// 마우스 클릭 시 드래그 시작 조건 검사
FReply UInventorySlotWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    // 이미 드래그 중이거나 서버 응답 대기 중이면 무시
    if (bIsDragging || bIsWaitingForServerResponse)
    {
        return FReply::Unhandled();
    }

    // 아이템이 있고 왼쪽 마우스 버튼 클릭 시에만 드래그 허용
    if (SlotData.IsValid() && InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
    {
        bIsDragging = true; // 드래그 상태 시작
        FEventReply Reply = UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton);
        return Reply.NativeReply;
    }

    return FReply::Unhandled();
}

// 마우스 버튼을 떼었을 때 드래그 상태 리셋 (드래그 취소된 경우)
FReply UInventorySlotWidget::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    // 드래그 중이었다면 상태 리셋
    if (bIsDragging && InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
    {
        bIsDragging = false;
    }

    return Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);
}

// 드래그가 감지되었을 때 드래그 오퍼레이션 생성
void UInventorySlotWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
    if (SlotData.IsValid())
    {
        // 커스텀 드래그 오퍼레이션 생성
        UItemDragDropOperation* DragDropOp = Cast<UItemDragDropOperation>(
            UWidgetBlueprintLibrary::CreateDragDropOperation(UItemDragDropOperation::StaticClass()));

        if (DragDropOp)
        {
            // 드래그 데이터 설정
            DragDropOp->ItemData = SlotData;
            DragDropOp->SourceType = EItemDragSource::Inventory;
            DragDropOp->SourceInventoryIndex = SlotIndex;

            // 드래그 비주얼 생성 (독립적인 이미지 위젯 사용)
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
                // 아이콘이 없으면 자기 자신 사용
                DragDropOp->DefaultDragVisual = this;
                DragDropOp->Pivot = EDragPivot::CenterCenter;
            }

            OutOperation = DragDropOp;
        }
        else
        {
            // 드래그 오퍼레이션 생성 실패 시 상태 리셋
            bIsDragging = false;
        }
    }
    else
    {
        // 슬롯 데이터가 유효하지 않으면 상태 리셋
        bIsDragging = false;
    }
}

// 드래그가 취소되었을 때 상태 리셋
void UInventorySlotWidget::NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
    Super::NativeOnDragCancelled(InDragDropEvent, InOperation);
    bIsDragging = false;
    bIsWaitingForServerResponse = false;
}

// 다른 위젯에서 이 슬롯으로 드롭했을 때 처리
bool UInventorySlotWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
    // 드래그 상태 즉시 리셋
    bIsDragging = false;

    UItemDragDropOperation* ItemDragDrop = Cast<UItemDragDropOperation>(InOperation);
    if (ItemDragDrop && OwningInventoryWidget)
    {
        bool bDropHandled = false;

        // 교환 슬롯에서 인벤토리로 드롭한 경우
        if (ItemDragDrop->SourceType == EItemDragSource::TradeSlot)
        {
            bDropHandled = OwningInventoryWidget->OnInventorySlotDrop(this, ItemDragDrop);
        }
        // 인벤토리 내에서 슬롯 간 이동한 경우
        else if (ItemDragDrop->SourceType == EItemDragSource::Inventory && ItemDragDrop->SourceInventoryIndex != SlotIndex)
        {
            bDropHandled = OwningInventoryWidget->OnInventorySlotSwap(this, ItemDragDrop->SourceInventoryIndex);
        }

        // 서버 RPC 호출이 성공한 경우 잠시 대기 상태로 전환
        if (bDropHandled)
        {
            bIsWaitingForServerResponse = true;

            // 0.5초 후 대기 상태 해제 (서버 응답 대기)
            GetWorld()->GetTimerManager().SetTimer(
                ServerResponseTimer,
                [this]()
                {
                    bIsWaitingForServerResponse = false;
                },
                0.5f,
                false
            );
        }

        return bDropHandled;
    }

    return false;
}