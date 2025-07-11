#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryTypes.h"
#include "InventoryComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MULTIPLAY_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	UInventoryComponent();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// 인벤토리 배열
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing=OnRep_Inventory, Category = "Inventory")
	TArray<FItemData> Inventory;
	
	//인벤토리 배열 복제 위한 더미 변수
	UPROPERTY(Replicated)
	int32 InventoryReplicationCounter;

	//소지금
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Inventory")
	int32 CurrentMoney;

	// 아이템 추가 함수
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Inventory")
	void ServerAddItem(const FItemData& ItemData);

	// 아이템 제거 함수
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Inventory")
	void ServerRemoveItem(const FItemData& ItemData);
	
	//인벤토리 변경 시 호출
	UFUNCTION()
	void OnRep_Inventory();

	// 아이템 교체
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Inventory")
	void ServerSwapItems(int32 SourceIndex, int32 TargetIndex);

	// 인벤토리 변경 이벤트
	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnInventoryChanged OnInventoryChanged;



};
