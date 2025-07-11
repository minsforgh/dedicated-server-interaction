#include "DropItem.h"
#include "Net/UnrealNetwork.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "MultiplayCharacter.h"
#include "MultiplayerController.h"
#include "InventoryComponent.h"
#include "InventoryTypes.h"

ADropItem::ADropItem()
{
    PrimaryActorTick.bCanEverTick = false;

    bReplicates = true;
    SetReplicateMovement(true);

    CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
    CollisionComponent->InitSphereRadius(100.f);
    CollisionComponent->SetCollisionProfileName("Trigger");
    RootComponent = CollisionComponent;

    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
    MeshComponent->SetupAttachment(RootComponent);

    CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ADropItem::OnOverlapBegin);

}

//Overlap 시 호출 (아이템 획득)
void ADropItem::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
                                bool bFromSweep, const FHitResult& SweepResult)
{
    if (HasAuthority())
    {
        AMultiplayCharacter* Player = Cast<AMultiplayCharacter>(OtherActor);
        if (Player)
        {
            APlayerController* RawController = Cast<APlayerController>(Player->GetController());
            AMultiplayerController* PC = Cast<AMultiplayerController>(RawController);
            if (PC && PC->InventoryComponent)
            {   
                UE_LOG(LogTemp, Warning, TEXT("Server Add Item Called"));
                PC->InventoryComponent->ServerAddItem(ItemData);
                Destroy(); // 아이템 제거
            }
    }     
    }
}

