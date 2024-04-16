// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthPickup.h"
#include "../Characters/Echo.h"
#include "../Components/AttributeComponent.h"
#include "../HUD/EchoWidget.h"
#include "Kismet/GameplayStatics.h"

void AHealthPickup::OnSphereOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    AEcho* Player = Cast<AEcho>(OtherActor);
    if (Player && Player->Attribute && Player->EchoWidget && PickupSound)
    {
        Player->Attribute->Health = 100.f;
        Player->EchoWidget->SetHealthbar(1.f);

        UGameplayStatics::PlaySoundAtLocation(this, PickupSound, GetActorLocation());

        Destroy();
    }
}
