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

    // 플레이어 컨트롤러에서 인벤토리 컴포넌트 가져오기
    APlayerController* PC = GetOwningPlayer();
    if (PC)
    {
        InventoryComponent = PC->FindComponentByClass<UInventoryComponent>();
        SetInventoryComponent(InventoryComponent);
    }
}

// 인벤토리 컴포넌트 설정 및 이벤트 바인딩
void UInventoryWidget::SetInventoryComponent(UInventoryComponent* InInventoryComponent)
{
    if (InventoryComponent)
    {
        // 인벤토리 변경 이벤트에 UI 업데이트 함수 바인딩
        InventoryComponent->OnInventoryChanged.AddDynamic(this, &UInventoryWidget::UpdateInventory);
        InitInventory();  // 슬롯 위젯 초기화
        UpdateInventory(); // 초기 UI 업데이트
    }
}

// 인벤토리 슬롯 위젯들 초기 설정
void UInventoryWidget::InitInventory()
{
    // 18개 슬롯 설정 (그리드 패널의 자식 위젯들)
    for (int i = 0; i < 18; i++)
    {
        UInventorySlotWidget* SlotWidget = Cast<UInventorySlotWidget>(SlotGridPanel->GetChildAt(i));
        if (SlotWidget)
        {
            SlotWidget->SlotIndex = i;  // 슬롯 인덱스 설정
            SlotWidget->OwningInventoryWidget = this;  // 부모 위젯 참조 설정
            SlotWidgets.Add(SlotWidget);  // 배열에 추가
        }
    }

    // 초기 돈 표시
    if (MoneyText && InventoryComponent)
    {
        MoneyText->SetText(FText::FromString(FString::FromInt(InventoryComponent->CurrentMoney)));
    }
}

// 인벤토리 데이터가 변경되었을 때 UI 업데이트
void UInventoryWidget::UpdateInventory()
{
    if (!InventoryComponent || !SlotWidgetClass || !SlotGridPanel)
        return;

    // 모든 슬롯 위젯 업데이트
    for (int32 i = 0; i < SlotWidgets.Num(); i++)
    {
        if (SlotWidgets[i])
        {
            // 슬롯 상태 리셋 (드래그 상태, 타이머 등 정리)
            SlotWidgets[i]->ResetSlotState();

            // 인벤토리 데이터에 맞춰 슬롯 업데이트
            if (i < InventoryComponent->Inventory.Num())
            {
                SlotWidgets[i]->SetSlotData(InventoryComponent->Inventory[i]);
            }
            else
            {
                SlotWidgets[i]->ClearSlotData();  // 빈 슬롯으로 설정
            }
        }
    }

    // 돈 표시 업데이트
    if (MoneyText)
    {
        MoneyText->SetText(FText::FromString(FString::FromInt(InventoryComponent->CurrentMoney)));
    }
}

// 인벤토리 슬롯 간 아이템 교환 처리
bool UInventoryWidget::OnInventorySlotSwap(UInventorySlotWidget* TargetSlot, int32 SourceSlotIndex)
{
    if (!TargetSlot || !InventoryComponent)
        return false;

    int32 TargetSlotIndex = TargetSlot->SlotIndex;

    // 서버에 아이템 교환 요청 (실제 데이터 변경은 서버에서 처리)
    InventoryComponent->ServerSwapItems(SourceSlotIndex, TargetSlotIndex);
    return true;
}

// 교환 슬롯에서 인벤토리로 아이템 드롭 처리
bool UInventoryWidget::OnInventorySlotDrop(UInventorySlotWidget* SlotWidget, UItemDragDropOperation* Operation)
{
    if (!SlotWidget || !Operation || !InventoryComponent)
        return false;

    // 교환 슬롯에서 온 아이템인 경우
    if (Operation->SourceType == EItemDragSource::TradeSlot)
    {
        // 플레이어 캐릭터의 TradeComponent를 통해 아이템 제거 요청
        APlayerController* PC = GetOwningPlayer();
        if (!PC)
            return false;

        APawn* Pawn = PC->GetPawn();
        AMultiplayCharacter* Character = Cast<AMultiplayCharacter>(Pawn);
        if (!Character || !Character->TradeComponent)
            return false;

        // 서버에 교환 슬롯에서 아이템 제거 요청
        Character->TradeComponent->ServerRemoveItemFromTradeSlot(Operation->SourceTradeSlotIndex);
        return true;
    }

    return false;
}