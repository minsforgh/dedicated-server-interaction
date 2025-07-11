// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerChatComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FChatOpenedEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FChatClosedEvent);

class UChatWidget;
class UEditableText;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MULTIPLAY_API UPlayerChatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	UPlayerChatComponent();

	UPROPERTY(BlueprintAssignable, Category = "Chat")
	FChatOpenedEvent OnInputEnabled;

	UPROPERTY(BlueprintAssignable, Category = "Chat")
	FChatClosedEvent OnInputDisabled;

	// 메세지 UI 위젯 클래스
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> MessageWidgetClass;

	// 채팅창 UI 위젯 레퍼런스
	UPROPERTY(EditAnywhere)
	UChatWidget* ChatWidgetInstance;

	UFUNCTION()
	void EnableInput();
	
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSendMessage(const FString& Message);

	UFUNCTION(Client, Reliable)
	void ClientReceiveMessage(const FString& SenderName, const FString& Message);

};
