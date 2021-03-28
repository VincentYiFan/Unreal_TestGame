// Fill out your copyright notice in the Description page of Project Settings.


#include "MPFinalProjectile.h"
#include "MPFinalPawn.h"

// Sets default values
AMPFinalProjectile::AMPFinalProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Tell the game that this actor should replicate 
	bReplicates = true;

	//Definition for the SphereComponent that will serve as the Root component for the projectile and its collision.
	SphereComponent = CreateDefaultSubobject<USphereComponent>( TEXT( "RootComponent" ) );
	SphereComponent->InitSphereRadius( 30.f );
	SphereComponent->SetCollisionProfileName( TEXT( "BlockAllDynamic" ) );
	RootComponent = SphereComponent;

	//Registering the Projectile Impact function on a Hit event.
	if( Role == ROLE_Authority )
	{
		SphereComponent->OnComponentHit.AddDynamic( this, &AMPFinalProjectile::OnProjectileImpact );
	}

	//Definition for the Mesh that will serve as our visual representation.
	static ConstructorHelpers::FObjectFinder<UStaticMesh> DefaultMesh( TEXT( "/Game/StarterContent/Shapes/Shape_Sphere.Shape_Sphere" ) );
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>( TEXT( "Mesh" ) );
	StaticMesh->SetupAttachment( RootComponent );

	//Set the Static Mesh and its position/scale if we successfully found a mesh asset to use.
	if( DefaultMesh.Succeeded() )
	{
		StaticMesh->SetStaticMesh( DefaultMesh.Object );
		StaticMesh->RelativeLocation = FVector( 0.0f, 0.0f, -37.5f );
		StaticMesh->RelativeScale3D = FVector( 0.75f, 0.75f, 0.75f );
	}

	static ConstructorHelpers::FObjectFinder<UParticleSystem> DefaultExplosionEffect( TEXT( "/Game/StarterContent/Particles/P_Explosion.P_Explosion" ) );
	if( DefaultExplosionEffect.Succeeded() )
	{
		ExplosionEffect = DefaultExplosionEffect.Object;
	}

	//Definition for the Projectile Movement Component.
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>( TEXT( "ProjectileMovement" ) );
	ProjectileMovementComponent->SetUpdatedComponent( SphereComponent );
	ProjectileMovementComponent->InitialSpeed = 1500.0f;
	ProjectileMovementComponent->MaxSpeed = 1500.0f;
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->ProjectileGravityScale = 0.0f;

	DamageType = UDamageType::StaticClass();
	Damage = 20.0f;
}

// Called when the game starts or when spawned
void AMPFinalProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

void AMPFinalProjectile::Destroyed()
{
	FVector spawnLocation = GetActorLocation();
	UGameplayStatics::SpawnEmitterAtLocation( this, ExplosionEffect, spawnLocation, FRotator::ZeroRotator, true, EPSCPoolMethod::AutoRelease );
}

void AMPFinalProjectile::OnProjectileImpact( UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit )
{
	if( OtherActor )
	{
		UGameplayStatics::ApplyPointDamage( OtherActor, Damage, NormalImpulse, Hit, Instigator->Controller, this, DamageType );

		AMPFinalPawn* BatteryCharacter = Cast<AMPFinalPawn>(OtherActor);
		if( BatteryCharacter != NULL )
		{
			BatteryCharacter->UpdatePower( -Damage * 5.f );
		}
	}

	Destroy();
}

// Called every frame
void AMPFinalProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

