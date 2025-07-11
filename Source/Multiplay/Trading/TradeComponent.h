#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TradeComponent.generated.h"

class AMultiplayCharacter;
class UTradeRequestWidget;
class UTradeWidget;
class ATradeManager;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MULTIPLAY_API UTradeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	
	UTradeComponent();

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    // 교환 요청 보내기
    UFUNCTION(Server, Reliable)
    void ServerRequestTrade(AMultiplayCharacter* TargetPlayer);

    // 교환 요청 받기
    UFUNCTION(Client, Reliable)
    void ClientReceiveTradeRequest(AMultiplayCharacter* RequestingPlayer);

    // 교환 요청 응답
    UFUNCTION(Server, Reliable)
    void ServerRespondToTradeRequest(AMultiplayCharacter* RequestingPlayer, bool bAccepted);

    // 교환 세션 시작
    UFUNCTION(Client, Reliable)
    void ClientStartTradeSession(AMultiplayCharacter* OtherPlayer);

    // 교환 UI 열기
    UFUNCTION(Client, Reliable)
    void ClientOpenTradeUI(AMultiplayCharacter* OtherPlayer);

    //교환 UI 닫기
    UFUNCTION(Client, Reliable)
    void ClientCloseTradeUI();

    // 교환 슬롯에 아이템 추가 요청
    UFUNCTION(Server, Reliable)
    void ServerAddItemToTradeSlot(int32 InventoryIndex, int32 TradeSlotIndex);
    
    //교환 슬롯에 아이템 제거 요청
    UFUNCTION(Server, Reliable)
    void ServerRemoveItemFromTradeSlot(int32 TradeSlotIndex);

    // 교환 슬롯 교환 요청 
    UFUNCTION(Server, Reliable)
    void ServerSwapTradeSlots(int32 SourceSlotIndex, int32 TargetSlotIndex);

    // 교환 확인 상태 설정 요청
    UFUNCTION(Server, Reliable)
    void ServerSetTradeConfirmation(bool bIsConfirmed);

    //교환 취소
    UFUNCTION(Server, Reliable)
    void ServerCancelTradeSession();

    // 현재 교환 중인 플레이어
    UPROPERTY(Replicated)
    AMultiplayCharacter* CurrentTradePartner;

    //교환 신청 UI 위젯 클래스
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UUserWidget> TradeRequestWidgetClass;

    // 교환 신청 UI 위젯 인스턴스
    UTradeRequestWidget* TradeRequestWidgetInstance;

    //교환 UI 위젯 클래스
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UUserWidget> TradeWidgetClass;
    
    //교환 UI 위젯 인스턴스
    UTradeWidget* TradeWidgetInstance;

    // 현재 교환 중인지 여부
    UPROPERTY(Replicated)
    bool bIsTrading;

    //교환 매니저
    UPROPERTY()
    ATradeManager* CurrentTradeManager;

private:

    // 교환 신청 UI 위젯 닫을 시 이벤트
    UFUNCTION()
    void OnTradeRequestWidgetClosed();

    //교환 신청 수락 시 이벤트
    UFUNCTION()
    void OnAcceptRequest(AMultiplayCharacter* RequestingPlayer);

};
