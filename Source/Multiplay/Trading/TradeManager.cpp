#include "TradeManager.h"
#include "MultiplayCharacter.h"
#include "InventoryComponent.h"
#include "TradeComponent.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "TradeWidget.h"

ATradeManager::ATradeManager()
{	
	PrimaryActorTick.bCanEverTick = false;

	//Replicate 설정위해서는 RootComponent가 존재해야함
	USceneComponent* SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("TradeManagerRoot"));
	RootComponent = SceneComp;
	
	// 복제 설정 (SetReplicates()는 완전히 초기화 된 후에 호출하는게 안전, 생정자 단계에서는 bReplicates 사용)
	bReplicates = true;

	// 기본 교환 상태 초기화
	TradeState = FTradeState();
}

void ATradeManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATradeManager, PlayerA);
	DOREPLIFETIME(ATradeManager, PlayerB);
	DOREPLIFETIME(ATradeManager, TradeState);
}

void ATradeManager::Initialize(AMultiplayCharacter* A, AMultiplayCharacter* B)
{	
	PlayerA = A;
	PlayerB = B;

	// 교환 상태 초기화
	TradeState = FTradeState();

	// 초기 상태 업데이트
	MulticastUpdateTradeState(TradeState);
}

bool ATradeManager::HasTradeSession(AMultiplayCharacter* A, AMultiplayCharacter* B)
{
    return (PlayerA == A && PlayerB == B) ||
        (PlayerA == B && PlayerB == A);
}

// 교환 슬롯에 아이템 추가
void ATradeManager::AddItemToTradeSlot(AMultiplayCharacter* RequestingPlayer, int32 InventoryIndex, int32 TradeSlotIndex)
{
    if (!HasAuthority())
    {
        return;
    }
    
    if (!RequestingPlayer || TradeSlotIndex < 0 || TradeSlotIndex >= 3)
    {
        return;
    }
        
    // 어느 플레이어가 요청했는지 확인
    bool bIsPlayerA = (RequestingPlayer == PlayerA);
    AMultiplayCharacter* Player = bIsPlayerA ? PlayerA : PlayerB;

    if (!Player)
    {
        return;
    }
    AController* PlayerController = Player->GetController();
    if (!PlayerController)
    {
        return;
    }

    // 인벤토리 컴포넌트에서 아이템 가져오기
    UInventoryComponent* Inventory = PlayerController->FindComponentByClass<UInventoryComponent>();
    if (!Inventory)
    {
        return;
    }
     
    TArray<FItemData> Items = Inventory->Inventory;
    if (!Items.IsValidIndex(InventoryIndex))
    {
        return;
    }
       
    FItemData Item = Items[InventoryIndex];

    // 교환 상태 업데이트
    if (bIsPlayerA)
    {   

        TradeState.PlayerAItems[TradeSlotIndex] = Item;
    }
    else
    {   
        TradeState.PlayerBItems[TradeSlotIndex] = Item;
    }

    //인벤토리에서 item 제거
    Inventory->ServerRemoveItem(Item);

    // 확인 상태 리셋
    TradeState.bPlayerAConfirmed = false;
    TradeState.bPlayerBConfirmed = false;

    // 양쪽 클라이언트에 교환 상태 업데이트
    MulticastUpdateTradeState(TradeState);
}

// 교환 슬롯에서 아이템 제거
void ATradeManager::RemoveItemFromTradeSlot(AMultiplayCharacter* RequestingPlayer, int32 TradeSlotIndex)
{
    if (!HasAuthority())
        return;

    if (!RequestingPlayer || TradeSlotIndex < 0 || TradeSlotIndex >= 3)
        return;

    bool bIsPlayerA = (RequestingPlayer == PlayerA);

    AController* PlayerController = RequestingPlayer->GetController();
    if (!PlayerController)
    {
        return;
    }

    // 컨트롤러에서 인벤토리 컴포넌트 찾기
    UInventoryComponent* Inventory = PlayerController->FindComponentByClass<UInventoryComponent>();
    if (!Inventory)
    {
        return;
    }

    // 아이템을 인벤토리로 반환
    FItemData ItemToReturn;
    if (bIsPlayerA)
    {
        ItemToReturn = TradeState.PlayerAItems[TradeSlotIndex];
        if (ItemToReturn.IsValid())
        {
            Inventory->ServerAddItem(ItemToReturn);
        }

        // 교환 상태 업데이트
        TradeState.PlayerAItems[TradeSlotIndex] = FItemData();
    }
    else
    {
        ItemToReturn = TradeState.PlayerBItems[TradeSlotIndex];
        if (ItemToReturn.IsValid())
        {
            Inventory->ServerAddItem(ItemToReturn);
        }
        // 교환 상태 업데이트
        TradeState.PlayerBItems[TradeSlotIndex] = FItemData();
    }

    // 확인 상태 리셋
    TradeState.bPlayerAConfirmed = false;
    TradeState.bPlayerBConfirmed = false;

    // 양쪽 클라이언트에 교환 상태 업데이트
    MulticastUpdateTradeState(TradeState);
}

// 교환 슬롯간 아이템 교환
void ATradeManager::SwapTradeSlots(AMultiplayCharacter* RequestingPlayer, int32 SourceSlotIndex, int32 TargetSlotIndex)
{ 
    if (!HasAuthority())
        return;

    if (!RequestingPlayer ||
        SourceSlotIndex < 0 || SourceSlotIndex >= 3 ||
        TargetSlotIndex < 0 || TargetSlotIndex >= 3)
        return;

    // 어느 플레이어가 요청했는지 확인
    bool bIsPlayerA = (RequestingPlayer == PlayerA);

    // 교환 상태 업데이트
    if (bIsPlayerA)
    {
        // 플레이어 A의 슬롯 교환
        FItemData TempItem = TradeState.PlayerAItems[SourceSlotIndex];
        TradeState.PlayerAItems[SourceSlotIndex] = TradeState.PlayerAItems[TargetSlotIndex];
        TradeState.PlayerAItems[TargetSlotIndex] = TempItem;
    }
    else
    {
        // 플레이어 B의 슬롯 교환
        FItemData TempItem = TradeState.PlayerBItems[SourceSlotIndex];
        TradeState.PlayerBItems[SourceSlotIndex] = TradeState.PlayerBItems[TargetSlotIndex];
        TradeState.PlayerBItems[TargetSlotIndex] = TempItem;
    }

    // 확인 상태 리셋
    TradeState.bPlayerAConfirmed = false;
    TradeState.bPlayerBConfirmed = false;

    // 양쪽 클라이언트에 교환 상태 업데이트
    MulticastUpdateTradeState(TradeState);
}

// 교환 승인 확인
void ATradeManager::SetTradeConfirmation(AMultiplayCharacter* RequestingPlayer, bool bIsConfirmed)
{
    if (!HasAuthority())
        return;

    if (!RequestingPlayer)
        return;

    // 어느 플레이어가 요청했는지 확인
    bool bIsPlayerA = (RequestingPlayer == PlayerA);

    // 교환 상태 업데이트
    if (bIsPlayerA)
    {
        TradeState.bPlayerAConfirmed = bIsConfirmed;
    }
    else
    {
        TradeState.bPlayerBConfirmed = bIsConfirmed;
    }

    // 양쪽 클라이언트에 교환 상태 업데이트
    MulticastUpdateTradeState(TradeState);

    // 양쪽이 모두 확인한 경우 교환 실행
    if (TradeState.bPlayerAConfirmed && TradeState.bPlayerBConfirmed)
    {
        ExecuteTrade();
    }
}

// 교환 실행
void ATradeManager::ExecuteTrade()
{
    if (!HasAuthority())
    {
        return;
    }

    // 양쪽 플레이어가 모두 확인했는지 체크
    if (!TradeState.bPlayerAConfirmed || !TradeState.bPlayerBConfirmed)
    {
        return;
    }

    // 플레이어 A와 B의 컨트롤러 및 인벤토리 컴포넌트
    AController* ControllerA = PlayerA ? PlayerA->GetController() : nullptr;
    AController* ControllerB = PlayerB ? PlayerB->GetController() : nullptr;

    if (!ControllerA || !ControllerB)
    {
        return;
    }

    UInventoryComponent* InventoryA = ControllerA->FindComponentByClass<UInventoryComponent>();
    UInventoryComponent* InventoryB = ControllerB->FindComponentByClass<UInventoryComponent>();

    if (!InventoryA || !InventoryB)
    {
        return;
    }

    // 각 슬롯의 아이템 교환
    for (int32 i = 0; i < 3; i++)
    {
        // A -> B 교환
        if (TradeState.PlayerAItems[i].ItemID != -1)
        {   
            //이미 교환 슬롯에 아이템 추가할 때 인벤토리에서는 제거함
            InventoryB->ServerAddItem(TradeState.PlayerAItems[i]);
        }

        // B -> A 교환
        if (TradeState.PlayerBItems[i].ItemID != -1)
        {
            InventoryA->ServerAddItem(TradeState.PlayerBItems[i]);
        }
    }

    // 교환 종료 및 UI 닫기
    if (PlayerA && PlayerA->TradeComponent)
    {
        PlayerA->TradeComponent->ClientCloseTradeUI();
        PlayerA->TradeComponent->bIsTrading = false;
        PlayerA->TradeComponent->CurrentTradePartner = nullptr;
    }

    if (PlayerB && PlayerB->TradeComponent)
    {
        PlayerB->TradeComponent->ClientCloseTradeUI();
        PlayerB->TradeComponent->bIsTrading = false;
        PlayerB->TradeComponent->CurrentTradePartner = nullptr;
    }

    // TradeManager 파괴
    Destroy();
}

// 교환 취소
void ATradeManager::CancelTrade()
{   
    if (!HasAuthority())
        return;

    // 플레이어 A와 B의 컨트롤러 및 인벤토리 컴포넌트
    AController* ControllerA = PlayerA ? PlayerA->GetController() : nullptr;
    AController* ControllerB = PlayerB ? PlayerB->GetController() : nullptr;

    if (ControllerA && ControllerB)
    {
        UInventoryComponent* InventoryA = ControllerA->FindComponentByClass<UInventoryComponent>();
        UInventoryComponent* InventoryB = ControllerB->FindComponentByClass<UInventoryComponent>();

        if (InventoryA && InventoryB)
        {
            // 각 플레이어의 거래 슬롯에 있는 아이템을 원래 주인에게 반환
            for (int32 i = 0; i < 3; i++)
            {
                // 플레이어 A의 아이템 반환
                if (TradeState.PlayerAItems[i].ItemID != -1)
                {
                    InventoryA->ServerAddItem(TradeState.PlayerAItems[i]);
                }

                // 플레이어 B의 아이템 반환
                if (TradeState.PlayerBItems[i].ItemID != -1)
                {
                    InventoryB->ServerAddItem(TradeState.PlayerBItems[i]);
                }
            }
        }
    }
 
    // 교환 종료 및 UI 닫기
    if (PlayerA && PlayerA->TradeComponent)
    {
        PlayerA->TradeComponent->ClientCloseTradeUI();
        PlayerA->TradeComponent->bIsTrading = false;
        PlayerA->TradeComponent->CurrentTradePartner = nullptr;
    }

    if (PlayerB && PlayerB->TradeComponent)
    {
        PlayerB->TradeComponent->ClientCloseTradeUI();
        PlayerB->TradeComponent->bIsTrading = false;
        PlayerB->TradeComponent->CurrentTradePartner = nullptr;
    }

    // TradeManager 파괴
    Destroy();
}

void ATradeManager::MulticastUpdateTradeState_Implementation(const FTradeState& NewState)
{
    // TradeState 업데이트
    TradeState = NewState;

    // UI 업데이트가 필요하다면 TradeWidget 찾아서 UpdateTradeState 호출
    APawn* LocalPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    AMultiplayCharacter* LocalCharacter = Cast<AMultiplayCharacter>(LocalPawn);

    if (LocalCharacter && LocalCharacter->TradeComponent && LocalCharacter->TradeComponent->TradeWidgetInstance)
    {
        LocalCharacter->TradeComponent->TradeWidgetInstance->UpdateTradeState(TradeState);
    }
}
