#pragma once

#include "CoreMinimal.h"
#include "Engine/Texture2D.h"
#include "Delegates/Delegate.h"
#include "InventoryTypes.generated.h"

//������ ������ Ÿ�� ����
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

    //�� ������
    bool operator==(const FItemData& Other) const
    {
        return ItemID == Other.ItemID;
    }

    bool operator!=(const FItemData& Other) const
    {
        return !(*this == Other);
    }

    bool IsValid() const
    {
        return ItemID != -1;
    }
};

// �̺�Ʈ ��������Ʈ ���� (�κ��丮 ��ȭ)
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryChanged);
