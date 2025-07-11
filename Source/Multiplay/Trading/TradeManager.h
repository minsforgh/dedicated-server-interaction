#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameFramework/Actor.h"
#include "TradeState.h"
#include "TradeManager.generated.h"

class AMultiplayCharacter;

UCLASS()
class MULTIPLAY_API ATradeManager : public AActor
{
	GENERATED_BODY()

public:

    ATradeManager();

    // 복제 속성 설정
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    // 초기화
    void Initialize(AMultiplayCharacter* A, AMultiplayCharacter* B);

    // 교환 세션 확인
    UFUNCTION(BlueprintCallable, Category = "Trade")
    bool HasTradeSession(AMultiplayCharacter* PlayerA, AMultiplayCharacter* PlayerB);

    // 플레이어 참조 얻기
    UFUNCTION(BlueprintCallable, Category = "Trade")
    AMultiplayCharacter* GetPlayerA() const { return PlayerA; }

    UFUNCTION(BlueprintCallable, Category = "Trade")
    AMultiplayCharacter* GetPlayerB() const { return PlayerB; }

    // 교환 슬롯에 아이템 추가
    void AddItemToTradeSlot(AMultiplayCharacter* RequestingPlayer, int32 InventoryIndex, int32 TradeSlotIndex);
    
    // 교환 슬롯 아이템 제거
    void RemoveItemFromTradeSlot(AMultiplayCharacter* RequestingPlayer, int32 TradeSlotIndex);

    // 교환 슬롯 간 아이템 교환
    void SwapTradeSlots(AMultiplayCharacter* RequestingPlayer, int32 SourceSlotIndex, int32 TargetSlotIndex);

    // 교환 확인 상태 설정
    void SetTradeConfirmation(AMultiplayCharacter* RequestingPlayer, bool bIsConfirmed);

    // 교환 실행
    void ExecuteTrade();

    // 교환 취소
    void CancelTrade();

    // 교환 상태 업데이트
    UFUNCTION(NetMulticast, Reliable, Category = "Trade")
    void MulticastUpdateTradeState(const FTradeState& NewState);

private:
    // 교환 참가자 두 명
    UPROPERTY(Replicated)
    AMultiplayCharacter* PlayerA;

    UPROPERTY(Replicated)
    AMultiplayCharacter* PlayerB;

    // 교환 상태 구조체
    UPROPERTY(Replicated)
    FTradeState TradeState;
};
