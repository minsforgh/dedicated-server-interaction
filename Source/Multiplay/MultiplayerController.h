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

/**
 * 
 */
UCLASS()
class MULTIPLAY_API AMultiplayerController : public APlayerController
{
	GENERATED_BODY()

public:
	// Chat Widget Class
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> ChatWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> MessageWidgetClass;

	// Chat Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Chat")
	UPlayerChatComponent* ChatComponent;

	// ���� ���ؽ�Ʈ
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputMappingContext* ControllerMappingContext;

	// ä�� �Է� �׼�
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* EnterChatAction;

	void ToggleChatInput();

	UFUNCTION()
	UChatWidget* CreateChatWidget();

	UFUNCTION()
	UMessageWidget* CreateMessageWidget();

	//chat input Ȱ��ȭ, PlayerChatComponent�� Delegate FChatOpenedEvent�� ���ε�
	UFUNCTION()
	void EnableChatInput();

	//chat input ��Ȱ��ȭ, ChatWidget�� FOnChatMessageSent�� ���ε�
	UFUNCTION()
	void DisableChatInput(const FString& Message);

	/*UFUNCTION(Client, Reliable)
	void ClientReceiveMessage(const FString& SenderName, const FString& Message);*/

	UPROPERTY()
	UChatWidget* ChatWidgetInstance;

protected:
	virtual void BeginPlay() override;

	virtual void SetupInputComponent() override;

	virtual void OnPossess(APawn* InPawn) override;

	virtual void AcknowledgePossession(APawn* InPawn) override;

	//virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	
};
