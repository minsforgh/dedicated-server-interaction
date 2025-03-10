// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MultiplayerController.generated.h"

class UChatWidget;

/**
 * 
 */
UCLASS()
class MULTIPLAY_API AMultiplayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> ChatWidgetClass;

	UPROPERTY()
	UChatWidget* ChatWidgetInstance;
};
