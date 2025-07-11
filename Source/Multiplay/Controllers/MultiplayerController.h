// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MultiplayerController.generated.h"

class UInputAction;
class UEditableText;
class UCanvasPanel;
class UInputMappingContext;
class APlayerChatComponent;
class UPlayerChatComponent;
class UChatWidget;
class UMessageWidget;
class UInventoryWidget;
class UInventoryComponent;

/**
 * 
 */
UCLASS()
class MULTIPLAY_API AMultiplayerController : public APlayerController
{
	GENERATED_BODY()

public:

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// Chat Widget Class
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> ChatWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> MessageWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> InventoryWidgetClass;
	
	UPROPERTY(VisibleAnyWhere, Category = "UI")
	UInventoryWidget* InventoryWidgetInstance;

	UPROPERTY(Replicated)
	UInventoryComponent* InventoryComponent;

	UPROPERTY()
	UChatWidget* ChatWidgetInstance;

	// Chat Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Chat")
	UPlayerChatComponent* ChatComponent;

	// 매핑 컨텍스트
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputMappingContext* ControllerMappingContext;

	// 채팅 입력 액션
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* EnterChatAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* ToggleInventoryAction;

	UPROPERTY()
	bool bIsInventoryVisible;

	UFUNCTION()
	UChatWidget* CreateChatWidget();

	UFUNCTION()
	UMessageWidget* CreateMessageWidget();

	UFUNCTION()
	UInventoryWidget* CreateInventoryWidget();

	//chat input 활성화, PlayerChatComponent의 Delegate FChatOpenedEvent에 바인딩
	UFUNCTION()
	void EnableChatInput();

	//chat input 비활성화, ChatWidget의 FOnChatMessageSent에 바인딩
	UFUNCTION()
	void DisableChatInput(const FString& Message);

	UFUNCTION()
	void EnableUIMode();

	UFUNCTION()
	void DisableUIMode();

	void ToggleChatInput();
	void ToggleInventory();

protected:
	virtual void BeginPlay() override;

	virtual void SetupInputComponent() override;

	virtual void OnPossess(APawn* InPawn) override;

	virtual void AcknowledgePossession(APawn* InPawn) override;

private:
	
};
