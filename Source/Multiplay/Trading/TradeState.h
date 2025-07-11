#pragma once

#include "CoreMinimal.h"
#include "InventoryTypes.h"
#include "ItemDatabase.h"
#include "TradeState.generated.h"

// 교환 상태 구조체
USTRUCT(BlueprintType)
struct MULTIPLAY_API FTradeState
{
    GENERATED_BODY()

    // 플레이어 A의 교환 슬롯
    UPROPERTY()
    TArray<FItemData> PlayerAItems;

    // 플레이어 B의 교환 슬롯
    UPROPERTY()
    TArray<FItemData> PlayerBItems;

    // 플레이어 A의 확인 상태
    UPROPERTY()
    bool bPlayerAConfirmed;

    // 플레이어 B의 확인 상태
    UPROPERTY()
    bool bPlayerBConfirmed;


    FTradeState()
    {
        // 각 플레이어당 3개의 교환 슬롯 초기화
        PlayerAItems.SetNum(3);
        PlayerBItems.SetNum(3);
        bPlayerAConfirmed = false;
        bPlayerBConfirmed = false;
    }

    // 네트워크 직렬화 함수 (효율적인 네트워크 통신 위함)
    bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
    {
        bOutSuccess = true;

        // 플레이어 A 아이템
        uint8 NumItemsA = PlayerAItems.Num();
        Ar << NumItemsA;
        if (Ar.IsLoading())
        {
            PlayerAItems.SetNum(NumItemsA);
        }

        for (int32 i = 0; i < NumItemsA; i++)
        {
            // FItemData 직렬화 (ItemID만 저장)
            Ar << PlayerAItems[i].ItemID;

            // 로딩 시 ID가 유효하면 데이터베이스에서 모든 정보 가져오기
            if (Ar.IsLoading())
            {
                if (PlayerAItems[i].ItemID != -1)
                {
                    // 데이터베이스에서 해당 ID의 아이템 정보 전체 가져오기
                    PlayerAItems[i] = UItemDatabase::GetInstance()->GetItemData(PlayerAItems[i].ItemID);
                }
                else
                {
                    // 빈 슬롯은 기본 FItemData로 설정
                    PlayerAItems[i] = FItemData();
                }
            }
        }

        // PlayerB 아이템 배열도 동일한 방식으로 직렬화
        uint8 NumItemsB = PlayerBItems.Num();
        Ar << NumItemsB;

        if (Ar.IsLoading())
        {
            PlayerBItems.SetNum(NumItemsB);
        }

        for (int32 i = 0; i < NumItemsB; i++)
        {
            // 아이템 ID만 직렬화
            Ar << PlayerBItems[i].ItemID;

            // 로딩 시 ID가 유효하면 데이터베이스에서 모든 정보 가져오기
            if (Ar.IsLoading())
            {
                if (PlayerBItems[i].ItemID != -1)
                {
                    // 데이터베이스에서 해당 ID의 아이템 정보 전체 가져오기
                    PlayerBItems[i] = UItemDatabase::GetInstance()->GetItemData(PlayerBItems[i].ItemID);
                }
                else
                {
                    // 빈 슬롯은 기본 FItemData로 설정
                    PlayerBItems[i] = FItemData();
                }
            }
        }

        // 확인 상태
        Ar << bPlayerAConfirmed;
        Ar << bPlayerBConfirmed;

        return bOutSuccess;
    }
};

// NetSerialize 구현을 위한 템플릿 특수화
template<>
struct TStructOpsTypeTraits<FTradeState> : public TStructOpsTypeTraitsBase2<FTradeState>
{
    enum
    {
        WithNetSerializer = true
    };
};

