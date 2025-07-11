#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TradeState.h"
#include "TradeWidget.generated.h"

class AMultiplayCharacter;
class UCanvasPanel;
class UUniformGridPanel;
class UButton;
class UTradeSlotWidget;
class ATradeManager;
class UTextBlock;
class UInventoryComponent;

UCLASS()
class MULTIPLAY_API UTradeWidget : public UUserWidget
{
	GENERATED_BODY()

public:
    // 초기화 함수
    UFUNCTION(BlueprintCallable, Category = "Trade")
    void InitializeWidget(AMultiplayCharacter* LocalPlayer, AMultiplayCharacter* OtherPlayer);

    // 교환 상태 업데이트
    UFUNCTION(BlueprintCallable, Category = "Trade")
    void UpdateTradeState(const FTradeState& NewState);

    // 교환 슬롯 드롭 처리
    UFUNCTION(BlueprintCallable, Category = "Trade")
    void OnTradeSlotDrop(UTradeSlotWidget* SlotWidget, UItemDragDropOperation* Operation);

    // 교환 슬롯 스왑 처리
    UFUNCTION(BlueprintCallable, Category = "Trade")
    void OnTradeSlotSwap(UTradeSlotWidget* TargetSlot, int32 SourceSlotIndex);

protected:

    virtual void NativeConstruct() override;

    // UI 요소들
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "Trade")
    UUniformGridPanel* MyTradeSlots;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "Trade")
    UUniformGridPanel* OtherTradeSlots;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "Trade")
    UButton* ConfirmButton;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "Trade")
    UButton* CancelButton;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "Trade")
    UTextBlock* MyNameText;

    UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "Trade")
    UTextBlock* OtherNameText;

    // 내 슬롯 위젯 배열
    UPROPERTY()
    TArray<UTradeSlotWidget*> MySlotWidgets;

    // 상대방 슬롯 위젯 배열
    UPROPERTY()
    TArray<UTradeSlotWidget*> OtherSlotWidgets;

    // 버튼 콜백
    UFUNCTION()
    void OnConfirmButtonClicked();

    UFUNCTION()
    void OnCancelButtonClicked();

private:

    UPROPERTY()
    AMultiplayCharacter* LocalPlayerRef;

    UPROPERTY()
    AMultiplayCharacter* OtherPlayerRef;

    UPROPERTY()
    ATradeManager* TradeManagerRef;

    UPROPERTY()
    UInventoryComponent* MyInventoryComponent;
    
};
