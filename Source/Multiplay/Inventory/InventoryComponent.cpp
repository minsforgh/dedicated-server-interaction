#include "InventoryComponent.h"
#include "Net/UnrealNetwork.h"
#include <Net/Core/PushModel/PushModel.h>

UInventoryComponent::UInventoryComponent()
{
	SetIsReplicatedByDefault(true);
}

void UInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(UInventoryComponent, Inventory, COND_OwnerOnly);
    DOREPLIFETIME_CONDITION(UInventoryComponent, InventoryReplicationCounter, COND_OwnerOnly);
    DOREPLIFETIME_CONDITION(UInventoryComponent, CurrentMoney, COND_OwnerOnly);
}

void UInventoryComponent::OnRep_Inventory()
{   
    OnInventoryChanged.Broadcast();
}

//인벤토리 내부 변경 시 더미 변수에도 변화 (Replication이 TArray 내부 변화는 단독으로 감지 못함)
void UInventoryComponent::ServerAddItem_Implementation(const FItemData& ItemData)
{
    Inventory.Add(ItemData);
    InventoryReplicationCounter++;
}

void UInventoryComponent::ServerRemoveItem_Implementation(const FItemData& ItemData)
{   
    Inventory.Remove(ItemData);
    InventoryReplicationCounter++;
}

void UInventoryComponent::ServerSwapItems_Implementation(int32 SourceIndex, int32 TargetIndex)
{

    // 서버에서만 실행
    if (!GetOwner()->HasAuthority())
        return;

    // [OK] 이 로그는 서버에서 출력됨 (보임!)
    UE_LOG(LogTemp, Error, TEXT("[OK][OK][OK] SERVER SWAP ITEMS CALLED! Source=%d, Target=%d [OK][OK][OK]"),
        SourceIndex, TargetIndex);

    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red,
            FString::Printf(TEXT("[OK] SERVER: SwapItems %d->%d"), SourceIndex, TargetIndex));
    }

    // 범위 체크
    if (SourceIndex < 0 || SourceIndex >= Inventory.Num() ||
        TargetIndex < 0 || TargetIndex >= Inventory.Num())
    {
        UE_LOG(LogTemp, Error, TEXT("[ERROR] INVALID INDICES! Source=%d, Target=%d, Inventory.Num()=%d"),
            SourceIndex, TargetIndex, Inventory.Num());
        return;
    }

    // 슬롯 교환
    FItemData TempItem = Inventory[SourceIndex];
    Inventory[SourceIndex] = Inventory[TargetIndex];
    Inventory[TargetIndex] = TempItem;

    // 인벤토리 변경 이벤트 호출
    OnInventoryChanged.Broadcast();

    UE_LOG(LogTemp, Warning, TEXT("[OK] SWAP COMPLETED SUCCESSFULLY"));
}

bool UInventoryComponent::ServerAddItem_Validate(const FItemData& ItemData)
{
    if (!ItemData.IsValid()) return false;
    if (ItemData.ItemID < 0 || ItemData.ItemID > 1000) return false;
    if (Inventory.Num() >= 20) return false;
    return true;
}

bool UInventoryComponent::ServerRemoveItem_Validate(const FItemData& ItemData)
{
    if (!Inventory.Contains(ItemData)) return false;
    return true;
}

bool UInventoryComponent::ServerSwapItems_Validate(int32 SourceIndex, int32 TargetIndex)
{
    if (SourceIndex < 0 || SourceIndex >= Inventory.Num()) return false;
    if (TargetIndex < 0 || TargetIndex >= Inventory.Num()) return false;
    return true;
}
