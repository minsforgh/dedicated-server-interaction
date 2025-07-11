// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InventoryTypes.h"
#include "DropItem.generated.h"

class USphereComponent;
class UStaticMeshComponent;

UCLASS()
class MULTIPLAY_API ADropItem : public AActor
{
	GENERATED_BODY()
	
public:	
	ADropItem();

	UPROPERTY(EditAnywhere, Category = "Item")
	FItemData ItemData;

	UPROPERTY(VisibleAnywhere, Category = "Collision")
	USphereComponent* CollisionComponent;

	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	UStaticMeshComponent* MeshComponent;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
						UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
						bool bFromSweep, const FHitResult& SweepResult);
};
