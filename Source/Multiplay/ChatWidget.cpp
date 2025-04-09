// Fill out your copyright notice in the Description page of Project Settings.


#include "ChatWidget.h"


void UChatWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (ChatInputText)
	{
		ChatInputText->OnTextCommitted.AddDynamic(this, &UChatWidget::OnChatTextCommitted);
	}
	else UE_LOG(LogTemp, Warning, TEXT("Can't find ChatInputBox"));
}

// CommitType Enter(Enter 입력) 일 시 
void UChatWidget::OnChatTextCommitted(const FText& Text, ETextCommit::Type CommitType)
{	
	if (CommitType == ETextCommit::OnEnter)
	{	
		UE_LOG(LogTemp, Warning, TEXT("Text Committed : %s"), *Text.ToString());
		OnChatMessageSent.Broadcast(Text.ToString());
		ChatInputText->SetText(FText::GetEmpty());
	}
}