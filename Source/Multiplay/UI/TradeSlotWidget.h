#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryTypes.h"
#include "TradeSlotWidget.generated.h"

class UImage;
class UTradeWidget;

UCLASS()
class MULTIPLAY_API UTradeSlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	// === 슬롯 데이터 관리 ===

	// 교환 슬롯에 아이템 데이터 설정
	UFUNCTION(BlueprintCallable, Category = "Trade")
	void SetSlotData(const FItemData& InItemData);

	// 교환 슬롯 초기화 (빈 슬롯으로 만들기)
	UFUNCTION(BlueprintCallable, Category = "Trade")
	void ClearSlotData();

	// === 슬롯 속성 ===

	// 내 슬롯인지 여부 (true: 내 슬롯, false: 상대방 슬롯)
	// 내 슬롯만 드래그 가능하고 드롭 가능
	UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn = true), Category = "Trade")
	bool bIsMySlot;

	// 아이템 아이콘을 표시할 이미지 위젯 (블루프린트에서 바인딩)
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "Trade")
	UImage* ItemIcon;

	// === 슬롯 데이터 및 식별자 ===

	// 현재 슬롯에 저장된 아이템 데이터
	UPROPERTY(BlueprintReadOnly, Category = "Trade")
	FItemData SlotData;

	// 교환 슬롯 인덱스 (0~2, 각 플레이어마다 3개 슬롯)
	UPROPERTY(BlueprintReadOnly, Category = "Trade")
	int32 SlotIndex;

	// 이 슬롯을 소유하는 교환 위젯 (드롭 이벤트 전달용)
	UPROPERTY(BlueprintReadOnly, Category = "Trade")
	UTradeWidget* OwningTradeWidget;

protected:
	// === 위젯 생명주기 ===

	// 위젯 초기화
	virtual void NativeConstruct() override;

	// === 드래그 앤 드롭 이벤트 처리 ===

	// 마우스 클릭 시 드래그 시작 조건 검사 (내 슬롯이고 아이템이 있을 때만)
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	// 드래그 감지 시 교환 슬롯용 드래그 오퍼레이션 생성
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;

	// 다른 위젯에서 이 교환 슬롯으로 드롭했을 때 처리
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
};