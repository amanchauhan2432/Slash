#include "Treasure.h"
#include "../Characters/Echo.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
ATreasure::ATreasure()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

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