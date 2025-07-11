#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryTypes.h"
#include "InventoryWidget.generated.h"

class UUniformGridPanel;
class UInventoryComponent;
class UInventorySlotWidget;
class UTextBlock;
class UItemDragDropOperation;

UCLASS()
class MULTIPLAY_API UInventoryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
    // 인벤토리 컴포넌트 참조 설정
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void SetInventoryComponent(UInventoryComponent* InInventoryComponent);

    UFUNCTION(BlueprintCallable, Category="Inventory")
    void InitInventory();

    // 인벤토리 UI 업데이트
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void UpdateInventory();

    // 드래그 앤 드롭 처리 함수
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    bool OnInventorySlotDrop(UInventorySlotWidget* SlotWidget, UItemDragDropOperation* Operation);

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    bool OnInventorySlotSwap(UInventorySlotWidget* TargetSlot, int32 SourceSlotIndex);

protected:
    virtual void NativeConstruct() override;

    // 슬롯 그리드 패널
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "Inventory")
    UUniformGridPanel* SlotGridPanel;

    // 머니 텍스트
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "Inventory")
    UTextBlock* MoneyText;

    // 슬롯 위젯 클래스
    UPROPERTY(EditDefaultsOnly, Category = "Inventory")
    TSubclassOf<UInventorySlotWidget> SlotWidgetClass;

    // 인벤토리 컴포넌트 참조
    UPROPERTY(BlueprintReadOnly, Category = "Inventory")
    UInventoryComponent* InventoryComponent;

    // 만들어진 슬롯 위젯들을 저장
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Inventory")
    TArray<UInventorySlotWidget*> SlotWidgets;


};

