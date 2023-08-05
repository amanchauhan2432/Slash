#include "Treasure.h"
#include "../Characters/Echo.h"
#include "Kismet/GameplayStatics.h"

ATreasure::ATreasure()
{
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void ATreasure::BeginPlay()
{
	Super::BeginPlay();
	
}

void ATreasure::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATreasure::OnSphereOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Player = Cast<AEcho>(OtherActor);
    if (Player)
    {
        UGameplayStatics::SpawnSoundAtLocation(this, PickUpSound, GetActorLocation());

		Destroy();
    }
}