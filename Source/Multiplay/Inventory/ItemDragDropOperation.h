#pragma once

#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "InventoryTypes.h"
#include "ItemDragDropOperation.generated.h"

//슬롯 타입 열거형
UENUM(BlueprintType)
enum class EItemDragSource : uint8
{
	Inventory,
	TradeSlot
};

//드래그&드롭 중 전달할 데이터와 드래그 상태 관리
UCLASS()
class MULTIPLAY_API UItemDragDropOperation : public UDragDropOperation
{
	GENERATED_BODY()

public:
    // 아이템 데이터
    UPROPERTY(BlueprintReadWrite, Category = "Drag Drop")
    FItemData ItemData;

    // 드래그 소스 타입
    UPROPERTY(BlueprintReadWrite, Category = "Drag Drop")
    EItemDragSource SourceType;

    // 인벤토리 인덱스 (인벤토리에서 드래그한 경우)
    UPROPERTY(BlueprintReadWrite, Category = "Drag Drop")
    int32 SourceInventoryIndex;

    // 교환 슬롯 인덱스 (교환 슬롯에서 드래그한 경우)
    UPROPERTY(BlueprintReadWrite, Category = "Drag Drop")
    int32 SourceTradeSlotIndex;
};

