#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryTypes.h"
#include "InventorySlotWidget.generated.h"

class UImage;
class UInventoryWidget;

UCLASS()
class MULTIPLAY_API UInventorySlotWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    // === 슬롯 데이터 관리 ===

    // 슬롯에 아이템 데이터 설정 및 UI 업데이트
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void SetSlotData(const FItemData& ItemData);

    // 슬롯 데이터 초기화 (빈 슬롯으로 만들기)
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void ClearSlotData();

    // 슬롯의 모든 상태를 초기화 (드래그 상태, 타이머 등 리셋)
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void ResetSlotState();

    // === 슬롯 식별 정보 ===

    // 슬롯 인덱스 (0~17, 에디터에서 자동 설정)
    UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn = true), Category = "Inventory")
    int32 SlotIndex;

    // 아이템 아이콘을 표시할 이미지 위젯 (블루프린트에서 바인딩)
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "Inventory")
    UImage* ItemIcon;

    // 이 슬롯을 소유하는 인벤토리 위젯 (드롭 이벤트 전달용)
    UPROPERTY()
    UInventoryWidget* OwningInventoryWidget;

protected:
    // === 위젯 생명주기 ===

    // 위젯 초기화
    virtual void NativeConstruct() override;

    // === 드래그 앤 드롭 이벤트 처리 ===

    // 마우스 클릭 시 드래그 시작 조건 검사
    virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

    // 마우스 버튼을 떼었을 때 드래그 상태 리셋 (드래그 취소된 경우)
    virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

    // 드래그가 감지되었을 때 드래그 오퍼레이션 생성
    virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;

    // 드래그가 취소되었을 때 상태 리셋
    virtual void NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

    // 다른 위젯에서 이 슬롯으로 드롭했을 때 처리
    virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

    // === 슬롯 데이터 ===

    // 현재 슬롯에 저장된 아이템 데이터
    UPROPERTY(BlueprintReadOnly, Category = "Inventory")
    FItemData SlotData;

private:
    // === 드래그 상태 관리 ===

    // 현재 드래그 중인지 여부 (클라이언트 로컬 상태)
    bool bIsDragging = false;

    // 서버 응답 대기 중인지 여부 (UI 잠금용)
    bool bIsWaitingForServerResponse = false;

    // 서버 응답 대기 타이머 (일정 시간 후 자동 해제)
    FTimerHandle ServerResponseTimer;
};