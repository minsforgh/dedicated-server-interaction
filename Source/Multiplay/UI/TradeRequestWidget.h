#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TradeRequestWidget.generated.h"

class AMultiplayCharacter;
class UTextBlock;
class UButton;

//TradeRequestWidgetInstance를 제거하기 위함
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWidgetClosed);

//ServerRespondToTradeRequest 위함
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRespondToRequest, AMultiplayCharacter*, RequestingPlayer);


UCLASS()
class MULTIPLAY_API UTradeRequestWidget : public UUserWidget
{
	GENERATED_BODY()

public:
    
    virtual void NativeConstruct() override;
	void InitializeWidget(AMultiplayCharacter* RequestingPlayer);
    FOnWidgetClosed OnWidgetClosed;
    FOnRespondToRequest OnRespondToRequest;

protected:
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UTextBlock* RequestTextBlock;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UButton* AcceptButton;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UButton* RejectButton;

    UFUNCTION()
    void OnAcceptButtonClicked();

    UFUNCTION()
    void OnRejectButtonClicked();

    UPROPERTY()
    AMultiplayCharacter* RqPlayer;
};
