#pragma once

#include "CoreMinimal.h"
#include "Engine/Texture2D.h"
#include "Delegates/Delegate.h"
#include "InventoryTypes.generated.h"

//아이템 데이터 타입 정의
USTRUCT(BlueprintType)
struct MULTIPLAY_API FItemData
{	
	GENERATED_BODY()

    FItemData()
        : ItemID(-1)
        , ItemName("")
        , ItemIcon(nullptr)
    {
    }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	int32 ItemID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FString ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	UTexture2D* ItemIcon;

    //비교 연산자
    bool operator==(const FItemData& Other) const
    {
        return ItemID == Other.ItemID;
    }

    bool operator!=(const FItemData& Other) const
    {
        return !(*this == Other);
    }

    bool IsValid()
    {
        return ItemID != -1;
    }
};

// 이벤트 델리게이트 선언 (인벤토리 변화)
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryChanged);
