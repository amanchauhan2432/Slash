// Fill out your copyright notice in the Description page of Project Settings.


#include "Exp.h"
#include "../Characters/Echo.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"

void AExp::OnSphereOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    AEcho* Player = Cast<AEcho>(OtherActor);
    if (Player)
    {
        Player->UpdateExpText(FMath::FRandRange(1.f, 10.f));
        if (PickupEffect && PickupSound)
        {
            UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, PickupEffect, GetActorLocation());
            UGameplayStatics::PlaySoundAtLocation(this, PickupSound, GetActorLocation());

            Destroy();
        }
    }
}
