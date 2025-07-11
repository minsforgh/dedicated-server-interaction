#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "InventoryTypes.h"
#include "ItemDataBase.generated.h"

// 아이템데이터베이스 정의 
UCLASS()
class MULTIPLAY_API UItemDatabase : public UObject
{
	GENERATED_BODY()

public:
    // 싱글톤 인스턴스 가져오기
    UFUNCTION(BlueprintCallable, Category = "Item Database")
    static UItemDatabase* GetInstance();

    // ID로 아이템 정보 가져오기
    UFUNCTION(BlueprintCallable, Category = "Item Database")
    FItemData GetItemData(int32 ItemID) const;

    // 모든 아이템 데이터 가져오기
    UFUNCTION(BlueprintCallable, Category = "Item Database")
    TArray<FItemData> GetAllItems() const;

    // ID로 아이템 존재 여부 확인
    UFUNCTION(BlueprintCallable, Category = "Item Database")
    bool IsValidItem(int32 ItemID) const;

private:
    // 싱글톤 인스턴스
    static UItemDatabase* Instance;

    // 아이템 정보 초기화
    void InitializeItemDatabase();

    // 아이템 정보 맵 (키: 아이템 ID, 값: 아이템 데이터)
    UPROPERTY(EditAnywhere, Category = "Item Database")
    TMap<int32, FItemData> ItemMap;
};
