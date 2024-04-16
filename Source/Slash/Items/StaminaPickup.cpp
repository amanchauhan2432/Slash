// Fill out your copyright notice in the Description page of Project Settings.


#include "StaminaPickup.h"
#include "../Characters/Echo.h"
#include "../Components/AttributeComponent.h"
#include "../HUD/EchoWidget.h"
#include "Kismet/GameplayStatics.h"

void AStaminaPickup::OnSphereOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    AEcho* Player = Cast<AEcho>(OtherActor);
    if (Player && Player->Attribute && Player->EchoWidget && PickupSound)
    {
        Player->Attribute->Stamina = 100.f;
        Player->EchoWidget->SetStaminaBar(1.f);

        UGameplayStatics::PlaySoundAtLocation(this, PickupSound, GetActorLocation());

        Destroy();
    }
}
