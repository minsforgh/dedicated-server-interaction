// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MessageWidget.generated.h"


/**
 * 
 */

class UMultiLineEditableTextBox;
class UEditableTextBox;

UCLASS()
class MULTIPLAY_API UMessageWidget : public UUserWidget
{
	GENERATED_BODY()

public :

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UEditableTextBox* SenderName;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UMultiLineEditableTextBox* MessageText;

	UFUNCTION()
	void InitializeMessageWidget(const FString& SenderNameText, const FString& Message);
};
