#include "ItemDatabase.h"

// 싱글톤 인스턴스 초기화
UItemDatabase* UItemDatabase::Instance = nullptr;

UItemDatabase* UItemDatabase::GetInstance()
{
    if (!Instance)
    {
        Instance = NewObject<UItemDatabase>();
        Instance->InitializeItemDatabase();
        Instance->AddToRoot(); // GC 방지
    }

    return Instance;
}

void UItemDatabase::InitializeItemDatabase()
{   
    //테스트 용 아이템 추가
    {
        FItemData Item;
        Item.ItemID = 1;
        Item.ItemName = "RedGem";
        Item.ItemIcon = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Textures/RedGem"));
        ItemMap.Add(Item.ItemID, Item);
    }

    {
        FItemData Item;
        Item.ItemID = 2;
        Item.ItemName = "BlueGem";
        Item.ItemIcon = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Textures/BlueGem"));
        ItemMap.Add(Item.ItemID, Item);
    }

    {
        FItemData Item;
        Item.ItemID = 3;
        Item.ItemName = "GreenGem";
        Item.ItemIcon = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Textures/GreenGem"));
        ItemMap.Add(Item.ItemID, Item);
    }
}

FItemData UItemDatabase::GetItemData(int32 ItemID) const
{
    if (ItemMap.Contains(ItemID))
    {
        return ItemMap[ItemID];
    }

    // 유효하지 않은 ID면 빈 아이템 데이터 반환
    return FItemData();
}

TArray<FItemData> UItemDatabase::GetAllItems() const
{
    TArray<FItemData> AllItems;

    for (const auto& Pair : ItemMap)
    {
        AllItems.Add(Pair.Value);
    }

    return AllItems;
}

bool UItemDatabase::IsValidItem(int32 ItemID) const
{
    return ItemMap.Contains(ItemID);
}

