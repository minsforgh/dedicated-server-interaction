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

void UTradeComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UTradeComponent, bIsTrading);
	DOREPLIFETIME(UTradeComponent, CurrentTradePartner);
}

//서버 통해 교환 신청
void UTradeComponent::ServerRequestTrade_Implementation(AMultiplayCharacter* TargetPlayer)
{	
	UE_LOG(LogTemp, Warning, TEXT("ServerRequestTrade Called"));
	
	if (!TargetPlayer && !TargetPlayer->TradeComponent)
	{
		return;
	}
	if (TargetPlayer->TradeComponent->bIsTrading)
	{
		return;
	}

	AMultiplayCharacter* ThisOwner = Cast<AMultiplayCharacter>(GetOwner());
	if (!ThisOwner)
	{
		UE_LOG(LogTemp, Error, TEXT("TradeComponent Owner is not a valid AMultiplayCharacter!"));
		return;
	}

	TargetPlayer->TradeComponent->ClientReceiveTradeRequest(ThisOwner);
}

// 신청 받은 Client에서 교환 신청 처리
void UTradeComponent::ClientReceiveTradeRequest_Implementation(AMultiplayCharacter* RequestingPlayer)
{	
	
	if (TradeRequestWidgetInstance)
	{
		return;
	}
	if (!RequestingPlayer && !RequestingPlayer->TradeComponent)
	{
		return;
	}

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
	if (!TradeRequestWidgetClass)
	{
		return;
	}

	TradeRequestWidgetInstance = CreateWidget<UTradeRequestWidget>(PC, TradeRequestWidgetClass);
	if (!TradeRequestWidgetInstance)
	{
		return;
	}

	TradeRequestWidgetInstance->OnWidgetClosed.AddDynamic(this, &UTradeComponent::OnTradeRequestWidgetClosed);
	TradeRequestWidgetInstance->OnRespondToRequest.AddDynamic(this, &UTradeComponent::OnAcceptRequest);

	PC->EnableUIMode();

	TradeRequestWidgetInstance->InitializeWidget(RequestingPlayer);
	TradeRequestWidgetInstance->AddToViewport();
}

// 교환 신청 응답 (서버로)
void UTradeComponent::ServerRespondToTradeRequest_Implementation(AMultiplayCharacter* RequestingPlayer, bool bAccepted)
{
	if (!bAccepted || !RequestingPlayer ||
		RequestingPlayer->TradeComponent->bIsTrading || bIsTrading)
		return;

	AMultiplayCharacter* CurrentPlayer = Cast<AMultiplayCharacter>(GetOwner());
	if (!CurrentPlayer)
		return;

	// 양쪽 플레이어 모두 교환 상태 설정
	bIsTrading = true;
	RequestingPlayer->TradeComponent->bIsTrading = true;
	CurrentTradePartner = RequestingPlayer;
	RequestingPlayer->TradeComponent->CurrentTradePartner = CurrentPlayer;

	// TradeManager 생성
	UWorld* World = GetWorld();
	if (World)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = World->GetGameState();
		ATradeManager* TradeManager = World->SpawnActor<ATradeManager>(ATradeManager::StaticClass(), SpawnParams);

		if (TradeManager)
		{
			// TradeManager 초기화
			TradeManager->Initialize(CurrentPlayer, RequestingPlayer);

			// TradeManager 참조 저장
			CurrentTradeManager = TradeManager;
			if (RequestingPlayer->TradeComponent)
			{
				RequestingPlayer->TradeComponent->CurrentTradeManager = TradeManager;
			}
		}
	}

	// 교환 세션 시작
	ClientStartTradeSession(RequestingPlayer);
	RequestingPlayer->TradeComponent->ClientStartTradeSession(CurrentPlayer);
}

// 교환 세션 시작
void UTradeComponent::ClientStartTradeSession_Implementation(AMultiplayCharacter* OtherPlayer)
{	
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (OwnerPawn && OwnerPawn->IsLocallyControlled())
	{
		ClientOpenTradeUI(OtherPlayer);
	}
}

// 각 Client에서 교환 UI 생성
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
	if (!PC)
	{
		return;
	}
	if (!TradeWidgetClass)
	{
		return;
	}
	TradeWidgetInstance = CreateWidget<UTradeWidget>(PC, TradeWidgetClass);
	if (!TradeWidgetInstance)
	{
		return;
	}

	PC->EnableUIMode();

	TradeWidgetInstance->InitializeWidget(Cast<AMultiplayCharacter>(OwnerActor), Otherplayer);
	TradeWidgetInstance->AddToViewport();

	PC->ToggleInventory();
}

// 각 Clien에서 교환 UI 제거
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

	PC->DisableUIMode();
	TradeWidgetInstance->RemoveFromParent();
	TradeWidgetInstance = nullptr;

	PC->ToggleInventory();
}

// 교환 세션 취소
void UTradeComponent::ServerCancelTradeSession_Implementation()
{
	if (CurrentTradeManager)
	{
		// 교환 취소 실행
		CurrentTradeManager->CancelTrade();
	}
	else
	{
		// TradeManager 참조가 없으면 월드에서 찾기
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

// 교환 슬롯에 아이템 추가 요청
void UTradeComponent::ServerAddItemToTradeSlot_Implementation(int32 InventoryIndex, int32 TradeSlotIndex)
{
	if (CurrentTradeManager)
	{
		AMultiplayCharacter* Character = Cast<AMultiplayCharacter>(GetOwner());
		if (Character)
		{
			CurrentTradeManager->AddItemToTradeSlot(Character, InventoryIndex, TradeSlotIndex);
		}
	}
}

// 교환  슬롯 아이템 제거 요청
void UTradeComponent::ServerRemoveItemFromTradeSlot_Implementation(int32 TradeSlotIndex)
{
	// 현재 교환 매니저 찾기
	if (CurrentTradeManager)
	{
		// 본인 확인
		AMultiplayCharacter* Character = Cast<AMultiplayCharacter>(GetOwner());
		if (Character)
		{
			// TradeManager에 명시적으로 요청한 플레이어 전달
			CurrentTradeManager->RemoveItemFromTradeSlot(Character, TradeSlotIndex);
		}
	}
}

// 교환 슬롯 간 아이템 교환
void UTradeComponent::ServerSwapTradeSlots_Implementation(int32 SourceSlotIndex, int32 TargetSlotIndex)
{
	if (CurrentTradeManager)
	{
		AMultiplayCharacter* Character = Cast<AMultiplayCharacter>(GetOwner());
		if (Character)
		{
			CurrentTradeManager->SwapTradeSlots(Character, SourceSlotIndex, TargetSlotIndex);
		}
	}
}

// 교환 승인 확인 요청
void UTradeComponent::ServerSetTradeConfirmation_Implementation(bool bIsConfirmed)
{
	if (CurrentTradeManager)
	{
		AMultiplayCharacter* Character = Cast<AMultiplayCharacter>(GetOwner());
		if (Character)
		{
			CurrentTradeManager->SetTradeConfirmation(Character, bIsConfirmed);
		}
	}
}

void UTradeComponent::OnTradeRequestWidgetClosed()
{	
	//교환 신청 위젯 인스턴스 제거 (GC)
	TradeRequestWidgetInstance = nullptr;
}

void UTradeComponent::OnAcceptRequest(AMultiplayCharacter* ReqeustingPlayer)
{
	ServerRespondToTradeRequest(ReqeustingPlayer, true);
}

