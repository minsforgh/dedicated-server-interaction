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
	// Sets default values for this component's properties
	UPlayerChatComponent();

	// 복제 기능 활성화를 위한 오버라이드
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(BlueprintAssignable, Category = "Chat")
	FChatOpenedEvent OnInputEnabled;

	UPROPERTY(BlueprintAssignable, Category = "Chat")
	FChatClosedEvent OnInputDisabled;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;


public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Message Widget Class
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

	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void MulticastSetMessage(const FString& SenderName, const FString& Message);
};
