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
	DOREPLIFETIME(UInventoryComponent, Inventory);
    DOREPLIFETIME(UInventoryComponent, InventoryReplicationCounter);
    DOREPLIFETIME(UInventoryComponent, CurrentMoney);
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

    // 범위 체크
    if (SourceIndex < 0 || SourceIndex >= Inventory.Num() ||
        TargetIndex < 0 || TargetIndex >= Inventory.Num())
        return;

    // 슬롯 교환
    FItemData TempItem = Inventory[SourceIndex];
    Inventory[SourceIndex] = Inventory[TargetIndex];
    Inventory[TargetIndex] = TempItem;

    // 인벤토리 변경 이벤트 호출
    OnInventoryChanged.Broadcast();
}

