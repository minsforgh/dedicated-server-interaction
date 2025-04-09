// Fill out your copyright notice in the Description page of Project Settings.


#include "MessageWidget.h"
#include "Components/MultiLineEditableTextBox.h"
#include "Components/EditableTextBox.h"

void UMessageWidget::InitializeMessageWidget(const FString& SenderNameText, const FString& Message)
{
	SenderName->SetText(FText::FromString(SenderNameText));
	MessageText->SetText(FText::FromString(Message));
}
