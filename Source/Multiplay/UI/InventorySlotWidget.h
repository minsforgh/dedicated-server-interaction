#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryTypes.h"
#include "InventorySlotWidget.generated.h"

class UImage;
class UTextBlock;
class UBorder;
class UInventoryWidget;

UCLASS()
class MULTIPLAY_API UInventorySlotWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
    // 슬롯 데이터 설정 함수
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void SetSlotData(const FItemData& ItemData);

    //슬롯 데이터 제거 함수
    UFUNCTION(BlueprintCallable, Category="Inventory")
    void ClearSlotData();

    // 슬롯 인덱스
    UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn = true), Category = "Inventory")
    int32 SlotIndex;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "Inventory")
    UImage* ItemIcon;

    // 현 슬롯 소유하는 인벤토리 위젯
    UPROPERTY()
    UInventoryWidget* OwningInventoryWidget;

protected:
    // 위젯 초기화
    virtual void NativeConstruct() override;

    // 드래그 앤 드롭 이벤트
    virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
    virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
    virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
    
    // 현재 슬롯 데이터
    UPROPERTY(BlueprintReadOnly, Category = "Inventory")
    FItemData SlotData;

};
