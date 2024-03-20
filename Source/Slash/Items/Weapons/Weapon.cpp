// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "../../Characters/Echo.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "../../Interface/HItInterface.h"
#include "NiagaraComponent.h"

AWeapon::AWeapon()
{
    WeaponBox = CreateDefaultSubobject<UBoxComponent>(TEXT("WeaponBox"));
    WeaponBox->SetupAttachment(GetRootComponent());
    WeaponBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    WeaponBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
    WeaponBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

    BoxTraceStart = CreateDefaultSubobject<USceneComponent>(TEXT("Box Trace Start"));
    BoxTraceStart->SetupAttachment(GetRootComponent());

    BoxTraceEnd = CreateDefaultSubobject<USceneComponent>(TEXT("Box Trace End"));
    BoxTraceEnd->SetupAttachment(GetRootComponent());
}

void AWeapon::BeginPlay()
{
    Super::BeginPlay();

    WeaponBox->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnBoxOverlapBegin);
}

void AWeapon::OnSphereOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    Super::OnSphereOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

    Player = Cast<AEcho>(OtherActor);
    if (Player)
    {
        Player->OverlappingWeapon = this;
    }
}

void AWeapon::OnSphereOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    Super::OnSphereOverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);

    if (Player)
    {
        Player->OverlappingWeapon = nullptr;
    }
}

void AWeapon::OnBoxOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (GetOwner()->ActorHasTag(TEXT("Enemy")) && OtherActor->ActorHasTag(TEXT("Enemy"))) return;

    FHitResult BoxHit;
    BoxTrace(BoxHit);

    if (BoxHit.GetActor())
    {
        if (GetOwner()->ActorHasTag(TEXT("Enemy")) && BoxHit.GetActor()->ActorHasTag(TEXT("Enemy"))) return;

        UGameplayStatics::ApplyDamage(BoxHit.GetActor(), Damage, GetInstigator()->GetController(), this, UDamageType::StaticClass());

        IHitInterface* HitInterface = Cast<IHitInterface>(BoxHit.GetActor());
        if (HitInterface)
        {
            HitInterface->Execute_GetHit(BoxHit.GetActor(), BoxHit.ImpactPoint, GetOwner());
        }
        
        CreateFields(BoxHit.ImpactPoint);
    }
}

void AWeapon::BoxTrace(FHitResult& BoxHit)
{
    FVector Start = BoxTraceStart->GetComponentLocation();
    FVector End = BoxTraceEnd->GetComponentLocation();

    TArray<AActor*> ActorsToIgnore;
    ActorsToIgnore.Add(this);
    ActorsToIgnore.AddUnique(GetOwner());
    for (AActor* Actor : IgnoreActors)
    {
        ActorsToIgnore.AddUnique(Actor);
    }
    UKismetSystemLibrary::BoxTraceSingle(this, Start, End, BoxTraceExtent, BoxTraceStart->GetComponentRotation(), ETraceTypeQuery::TraceTypeQuery1, false, ActorsToIgnore, bShowBoxDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None, BoxHit, true);
    IgnoreActors.AddUnique(BoxHit.GetActor());
}

void AWeapon::Equip(USceneComponent* InParent, FName InSocketName, AActor* NewOwner, APawn* NewInstigator)
{
    AttachMeshToSocket(InParent, InSocketName);
    ItemState = EItemState::EIS_Equipped;

    SetOwner(NewOwner);
    SetInstigator(NewInstigator);

    if (EquipSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, EquipSound, GetActorLocation());
    }

    if (Sphere)
    {
        Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }
    if (EmbersEffect)
    {
        EmbersEffect->Deactivate();
    }
}

void AWeapon::AttachMeshToSocket(USceneComponent* InParent, FName InSocketName)
{
    FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget, true);
    ItemMesh->AttachToComponent(InParent, TransformRules, InSocketName);
}

