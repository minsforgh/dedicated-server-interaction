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

	// �κ��丮 �迭
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing=OnRep_Inventory, Category = "Inventory")
	TArray<FItemData> Inventory;
	
	//�κ��丮 �迭 ���� ���� ���� ����
	UPROPERTY(Replicated)
	int32 InventoryReplicationCounter;

	//������
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Inventory")
	int32 CurrentMoney;

	// ������ �߰� �Լ�
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "Inventory")
	void ServerAddItem(const FItemData& ItemData);

	// ������ ���� �Լ�
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "Inventory")
	void ServerRemoveItem(const FItemData& ItemData);
	
	//�κ��丮 ���� �� ȣ��
	UFUNCTION()
	void OnRep_Inventory();

	// ������ ��ü
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "Inventory")
	void ServerSwapItems(int32 SourceIndex, int32 TargetIndex);

	// �κ��丮 ���� �̺�Ʈ
	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnInventoryChanged OnInventoryChanged;



};
