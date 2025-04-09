// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/EditableText.h"
#include "Components/ScrollBox.h"
#include "ChatWidget.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChatMessageSent, const FString&, Message);

UCLASS()
class MULTIPLAY_API UChatWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	virtual void NativeConstruct() override;

	// meta = (BindWidget) ���� ���� ���� ������� ������ �̸��� ���� ���ε�
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UEditableText* ChatInputText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UScrollBox* MessageBox;

	UPROPERTY(BlueprintAssignable, Category = "Chat")
	FOnChatMessageSent OnChatMessageSent;

protected:

	// ���ε��� �Լ��� UFUNCTION(), public or protected
	UFUNCTION()
	void OnChatTextCommitted(const FText& Text, ETextCommit::Type CommitType);

private:

	
	
};
