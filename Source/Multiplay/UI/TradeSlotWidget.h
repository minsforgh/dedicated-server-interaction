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

	//슬롯 데이터 설정
	UFUNCTION(BlueprintCallable, Category = "Trade")
	void SetSlotData(const FItemData& InItemData);

	//슬롯 데이터 초기화
	UFUNCTION(BlueprintCallable, Category = "Trade")
	void ClearSlotData();

	// 내 슬롯인지 여부 (true: 내 슬롯, false: 상대방 슬롯)
	UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn = true), Category = "Trade")
	bool bIsMySlot;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "Trade")
	UImage* ItemIcon;

	// 현재 슬롯 데이터
	UPROPERTY(BlueprintReadOnly, Category = "Trade")
	FItemData SlotData;

	UPROPERTY(BlueprintReadOnly, Category = "Trade")
	int32 SlotIndex;

	UPROPERTY(BlueprintReadOnly, Category = "Trade")
	UTradeWidget* OwningTradeWidget;

protected:
	// 위젯 초기화
	virtual void NativeConstruct() override;

	// 드래그 앤 드롭 이벤트
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
};
