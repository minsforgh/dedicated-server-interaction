// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiplayerController.h"
#include "Blueprint/UserWidget.h"
#include "ChatWidget.h"

void AMultiplayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalPlayerController())
	{
		if (ChatWidgetClass)
		{
			ChatWidgetInstance = CreateWidget<UChatWidget>(this, ChatWidgetClass);

			if (ChatWidgetInstance)
			{
				ChatWidgetInstance->AddToViewport();
			}
		}
	}
	

}
