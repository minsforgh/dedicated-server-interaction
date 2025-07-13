#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TradeComponent.generated.h"

class AMultiplayCharacter;
class UTradeRequestWidget;
class UTradeWidget;
class ATradeManager;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class MULTIPLAY_API UTradeComponent : public UActorComponent
{
    GENERATED_BODY()

public:

    UTradeComponent();

    // 네트워크 리플리케이션 설정
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    // === 교환 요청 관련 RPC ===

    // [Server RPC] 다른 플레이어에게 교환 신청 보내기
    UFUNCTION(Server, Reliable)
    void ServerRequestTrade(AMultiplayCharacter* TargetPlayer);

    // [Client RPC] 교환 요청을 받았을 때 UI 표시
    UFUNCTION(Client, Reliable)
    void ClientReceiveTradeRequest(AMultiplayCharacter* RequestingPlayer);

    // [Server RPC] 교환 요청에 대한 응답 처리 (수락/거절)
    UFUNCTION(Server, Reliable)
    void ServerRespondToTradeRequest(AMultiplayCharacter* RequestingPlayer, bool bAccepted);

    // === 교환 세션 관리 RPC ===

    // [Client RPC] 교환 세션 시작 (로컬 플레이어인 경우에만 UI 열기)
    UFUNCTION(Client, Reliable)
    void ClientStartTradeSession(AMultiplayCharacter* OtherPlayer);

    // [Client RPC] 교환 UI 열기
    UFUNCTION(Client, Reliable)
    void ClientOpenTradeUI(AMultiplayCharacter* OtherPlayer);

    // [Client RPC] 교환 UI 닫기
    UFUNCTION(Client, Reliable)
    void ClientCloseTradeUI();

    // [Server RPC] 교환 세션 취소
    UFUNCTION(Server, Reliable)
    void ServerCancelTradeSession();

    // === 교환 아이템 관리 RPC ===

    // [Server RPC] 인벤토리에서 교환 슬롯으로 아이템 추가
    UFUNCTION(Server, Reliable)
    void ServerAddItemToTradeSlot(int32 InventoryIndex, int32 TradeSlotIndex);

    // [Server RPC] 교환 슬롯에서 아이템 제거 (인벤토리로 복귀)
    UFUNCTION(Server, Reliable)
    void ServerRemoveItemFromTradeSlot(int32 TradeSlotIndex);

    // [Server RPC] 교환 슬롯 간 아이템 교환
    UFUNCTION(Server, Reliable)
    void ServerSwapTradeSlots(int32 SourceSlotIndex, int32 TargetSlotIndex);

    // [Server RPC] 교환 확인 상태 설정 (확인/취소)
    UFUNCTION(Server, Reliable)
    void ServerSetTradeConfirmation(bool bIsConfirmed);

    // === 리플리케이션 변수들 ===

    // 현재 교환 중인 플레이어 (서버에서 관리, 클라이언트에 복제)
    UPROPERTY(Replicated)
    AMultiplayCharacter* CurrentTradePartner;

    // 현재 교환 중인지 여부 (서버에서 관리, 클라이언트에 복제)
    UPROPERTY(Replicated)
    bool bIsTrading;

    // === UI 위젯 관련 ===

    // 교환 신청 UI 위젯 클래스 (에디터에서 설정)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UUserWidget> TradeRequestWidgetClass;

    // 교환 신청 UI 위젯 인스턴스 (런타임에 생성)
    UTradeRequestWidget* TradeRequestWidgetInstance;

    // 교환 UI 위젯 클래스 (에디터에서 설정)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UUserWidget> TradeWidgetClass;

    // 교환 UI 위젯 인스턴스 (런타임에 생성)
    UTradeWidget* TradeWidgetInstance;

    // === 교환 매니저 ===

    // 현재 교환 세션을 관리하는 TradeManager (서버에서 생성)
    UPROPERTY()
    ATradeManager* CurrentTradeManager;

private:

    // === 위젯 이벤트 핸들러 ===

    // 교환 신청 UI 위젯이 닫혔을 때 정리
    UFUNCTION()
    void OnTradeRequestWidgetClosed();

    // 교환 요청 수락 시 서버에 응답 전송
    UFUNCTION()
    void OnAcceptRequest(AMultiplayCharacter* RequestingPlayer);
};