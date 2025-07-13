#include "TradeComponent.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/Actor.h"
#include "MultiplayCharacter.h"
#include "TradeRequestWidget.h"
#include "MultiplayerController.h"
#include "TradeManager.h"
#include "TradeWidget.h"
#include "GameFramework/GameStateBase.h"
#include "EngineUtils.h"

UTradeComponent::UTradeComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
}

// 네트워크 리플리케이션 설정
void UTradeComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UTradeComponent, bIsTrading);
	DOREPLIFETIME(UTradeComponent, CurrentTradePartner);
}

// [Server RPC] 다른 플레이어에게 교환 신청 보내기
void UTradeComponent::ServerRequestTrade_Implementation(AMultiplayCharacter* TargetPlayer)
{
	// 대상 플레이어 유효성 검사
	if (!TargetPlayer || !TargetPlayer->TradeComponent || TargetPlayer->TradeComponent->bIsTrading)
	{
		return;
	}

	AMultiplayCharacter* ThisOwner = Cast<AMultiplayCharacter>(GetOwner());
	if (!ThisOwner)
	{
		return;
	}

	// 대상 플레이어에게 교환 요청 UI 표시
	TargetPlayer->TradeComponent->ClientReceiveTradeRequest(ThisOwner);
}

// [Client RPC] 교환 요청을 받았을 때 UI 표시
void UTradeComponent::ClientReceiveTradeRequest_Implementation(AMultiplayCharacter* RequestingPlayer)
{
	// 이미 교환 요청 UI가 있거나 요청자가 유효하지 않으면 무시
	if (TradeRequestWidgetInstance || !RequestingPlayer || !RequestingPlayer->TradeComponent)
	{
		return;
	}

	// 플레이어 컨트롤러 가져오기
	AActor* OwnerActor = GetOwner();
	if (!OwnerActor)
	{
		return;
	}

	AController* OwnerController = OwnerActor->GetInstigatorController();
	if (!OwnerController)
	{
		return;
	}

	AMultiplayerController* PC = Cast<AMultiplayerController>(OwnerController);
	if (!PC || !TradeRequestWidgetClass)
	{
		return;
	}

	// 교환 요청 위젯 생성 및 설정
	TradeRequestWidgetInstance = CreateWidget<UTradeRequestWidget>(PC, TradeRequestWidgetClass);
	if (!TradeRequestWidgetInstance)
	{
		return;
	}

	// 위젯 이벤트 바인딩
	TradeRequestWidgetInstance->OnWidgetClosed.AddDynamic(this, &UTradeComponent::OnTradeRequestWidgetClosed);
	TradeRequestWidgetInstance->OnRespondToRequest.AddDynamic(this, &UTradeComponent::OnAcceptRequest);

	PC->EnableUIMode(); // UI 모드 활성화

	TradeRequestWidgetInstance->InitializeWidget(RequestingPlayer);
	TradeRequestWidgetInstance->AddToViewport();
}

// [Server RPC] 교환 요청에 대한 응답 처리 (수락/거절)
void UTradeComponent::ServerRespondToTradeRequest_Implementation(AMultiplayCharacter* RequestingPlayer, bool bAccepted)
{
	// 거절하거나 조건이 맞지 않으면 무시
	if (!bAccepted || !RequestingPlayer || RequestingPlayer->TradeComponent->bIsTrading || bIsTrading)
		return;

	AMultiplayCharacter* CurrentPlayer = Cast<AMultiplayCharacter>(GetOwner());
	if (!CurrentPlayer)
		return;

	// 양쪽 플레이어 모두 교환 상태로 설정
	bIsTrading = true;
	RequestingPlayer->TradeComponent->bIsTrading = true;
	CurrentTradePartner = RequestingPlayer;
	RequestingPlayer->TradeComponent->CurrentTradePartner = CurrentPlayer;

	// 서버에서 TradeManager 생성 (교환 세션 관리)
	UWorld* World = GetWorld();
	if (World)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = World->GetGameState();
		ATradeManager* TradeManager = World->SpawnActor<ATradeManager>(ATradeManager::StaticClass(), SpawnParams);

		if (TradeManager)
		{
			// TradeManager 초기화 및 참조 저장
			TradeManager->Initialize(CurrentPlayer, RequestingPlayer);

			CurrentTradeManager = TradeManager;
			if (RequestingPlayer->TradeComponent)
			{
				RequestingPlayer->TradeComponent->CurrentTradeManager = TradeManager;
			}
		}
	}

	// 양쪽 클라이언트에서 교환 세션 시작
	ClientStartTradeSession(RequestingPlayer);
	RequestingPlayer->TradeComponent->ClientStartTradeSession(CurrentPlayer);
}

// [Client RPC] 교환 세션 시작 (로컬 플레이어인 경우에만 UI 열기)
void UTradeComponent::ClientStartTradeSession_Implementation(AMultiplayCharacter* OtherPlayer)
{
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (OwnerPawn && OwnerPawn->IsLocallyControlled())
	{
		ClientOpenTradeUI(OtherPlayer);
	}
}

// [Client RPC] 교환 UI 열기
void UTradeComponent::ClientOpenTradeUI_Implementation(AMultiplayCharacter* Otherplayer)
{
	AActor* OwnerActor = GetOwner();
	if (!OwnerActor)
	{
		return;
	}

	AController* OwnerController = OwnerActor->GetInstigatorController();
	if (!OwnerController)
	{
		return;
	}

	AMultiplayerController* PC = Cast<AMultiplayerController>(OwnerController);
	if (!PC || !TradeWidgetClass)
	{
		return;
	}

	// 교환 UI 위젯 생성
	TradeWidgetInstance = CreateWidget<UTradeWidget>(PC, TradeWidgetClass);
	if (!TradeWidgetInstance)
	{
		return;
	}

	PC->EnableUIMode(); // UI 모드 활성화

	// 위젯 초기화 및 화면에 표시
	TradeWidgetInstance->InitializeWidget(Cast<AMultiplayCharacter>(OwnerActor), Otherplayer);
	TradeWidgetInstance->AddToViewport();

	PC->ToggleInventory(); // 인벤토리도 함께 열기
}

// [Client RPC] 교환 UI 닫기
void UTradeComponent::ClientCloseTradeUI_Implementation()
{
	AActor* OwnerActor = GetOwner();
	if (!OwnerActor)
	{
		return;
	}

	AController* OwnerController = OwnerActor->GetInstigatorController();
	if (!OwnerController)
	{
		return;
	}

	AMultiplayerController* PC = Cast<AMultiplayerController>(OwnerController);
	if (!PC)
	{
		return;
	}

	PC->DisableUIMode(); // UI 모드 비활성화
	TradeWidgetInstance->RemoveFromParent();
	TradeWidgetInstance = nullptr;

	PC->ToggleInventory(); // 인벤토리 닫기
}

// [Server RPC] 교환 세션 취소
void UTradeComponent::ServerCancelTradeSession_Implementation()
{
	// 현재 TradeManager가 있으면 취소 실행
	if (CurrentTradeManager)
	{
		CurrentTradeManager->CancelTrade();
	}
	else
	{
		// TradeManager 참조가 없으면 월드에서 찾아서 취소
		UWorld* World = GetWorld();
		if (World)
		{
			for (TActorIterator<ATradeManager> It(World); It; ++It)
			{
				ATradeManager* Manager = *It;
				if (Manager && Manager->HasTradeSession(Cast<AMultiplayCharacter>(GetOwner()), CurrentTradePartner))
				{
					Manager->CancelTrade();
					break;
				}
			}
		}
	}
}

// [Server RPC] 인벤토리에서 교환 슬롯으로 아이템 추가
void UTradeComponent::ServerAddItemToTradeSlot_Implementation(int32 InventoryIndex, int32 TradeSlotIndex)
{
	if (CurrentTradeManager)
	{
		AMultiplayCharacter* Character = Cast<AMultiplayCharacter>(GetOwner());
		if (Character)
		{
			// TradeManager에게 아이템 추가 요청
			CurrentTradeManager->AddItemToTradeSlot(Character, InventoryIndex, TradeSlotIndex);
		}
	}
}

// [Server RPC] 교환 슬롯에서 아이템 제거 (인벤토리로 복귀)
void UTradeComponent::ServerRemoveItemFromTradeSlot_Implementation(int32 TradeSlotIndex)
{
	if (CurrentTradeManager)
	{
		AMultiplayCharacter* Character = Cast<AMultiplayCharacter>(GetOwner());
		if (Character)
		{
			// TradeManager에게 아이템 제거 요청
			CurrentTradeManager->RemoveItemFromTradeSlot(Character, TradeSlotIndex);
		}
	}
}

// [Server RPC] 교환 슬롯 간 아이템 교환
void UTradeComponent::ServerSwapTradeSlots_Implementation(int32 SourceSlotIndex, int32 TargetSlotIndex)
{
	if (CurrentTradeManager)
	{
		AMultiplayCharacter* Character = Cast<AMultiplayCharacter>(GetOwner());
		if (Character)
		{
			// TradeManager에게 슬롯 간 교환 요청
			CurrentTradeManager->SwapTradeSlots(Character, SourceSlotIndex, TargetSlotIndex);
		}
	}
}

// [Server RPC] 교환 확인 상태 설정 (확인/취소)
void UTradeComponent::ServerSetTradeConfirmation_Implementation(bool bIsConfirmed)
{
	if (CurrentTradeManager)
	{
		AMultiplayCharacter* Character = Cast<AMultiplayCharacter>(GetOwner());
		if (Character)
		{
			// TradeManager에게 확인 상태 전달
			CurrentTradeManager->SetTradeConfirmation(Character, bIsConfirmed);
		}
	}
}

// 교환 요청 위젯이 닫혔을 때 정리
void UTradeComponent::OnTradeRequestWidgetClosed()
{
	TradeRequestWidgetInstance = nullptr;
}

// 교환 요청 수락 시 서버에 응답 전송
void UTradeComponent::OnAcceptRequest(AMultiplayCharacter* ReqeustingPlayer)
{
	ServerRespondToTradeRequest(ReqeustingPlayer, true);
}