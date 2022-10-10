// Fill out your copyright notice in the Description page of Project Settings.


#include "MonsterShooter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Projectile.h"
#include "Animation/AnimInstance.h"
#include "Kismet/GameplayStatics.h"

#include "MonsterShooterGameMode.h"




// Sets default values
AMonsterShooter::AMonsterShooter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->InitCapsuleSize(40.0f, 95.0f);

	TurnRate = 45.0f;
	LookUpRate = 45.0f;

	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("First Person Camera"));
	FirstPersonCamera->SetupAttachment(GetCapsuleComponent());
	FirstPersonCamera->AddRelativeLocation(FVector(-39.65f, 1.75f, 64.0f));
	FirstPersonCamera->bUsePawnControlRotation = true;

	HandMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Character Mesh"));
	HandMesh->SetOnlyOwnerSee(true);
	HandMesh->SetupAttachment(FirstPersonCamera);
	HandMesh->bCastDynamicShadow = false;
	HandMesh->CastShadow = false;
	HandMesh->AddRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	HandMesh->AddRelativeLocation(FVector(-0.5f, - 4.4f, -155.7f));



	GunMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Gun"));
	GunMesh->SetOnlyOwnerSee(true);
	GunMesh->bCastDynamicShadow = false;
	GunMesh->CastShadow = false;


	MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("Muzzle Location"));
	MuzzleLocation->SetupAttachment(GunMesh);
	MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));

	GunOffset = FVector(100.0f, 0.0f, 10.0f);

}

// Called when the game starts or when spawned
void AMonsterShooter::BeginPlay()
{
	Super::BeginPlay();


	GunMesh->AttachToComponent(HandMesh,
		FAttachmentTransformRules::SnapToTargetNotIncludingScale, 
		TEXT("GripPoint"));
	World = GetWorld();

	AnimInstance = HandMesh->GetAnimInstance();

}

// Called every frame
void AMonsterShooter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AMonsterShooter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AMonsterShooter::OnFire);
	PlayerInputComponent->BindAxis("MoveForward", this, &AMonsterShooter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMonsterShooter::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &AMonsterShooter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &AMonsterShooter::LookAtRate);
}

void AMonsterShooter::OnFire()
{
	if(World != NULL)
	{
		SpawnRotation = GetControlRotation();
		SpawnLocation = ((MuzzleLocation != nullptr) ?
			MuzzleLocation->GetComponentLocation() :
			GetActorLocation()) + SpawnRotation.RotateVector(GunOffset);

		FActorSpawnParameters ActorSpawnParams;
		ActorSpawnParams.SpawnCollisionHandlingOverride =
			ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

		World->SpawnActor<AProjectile>(Projectile, SpawnLocation, SpawnRotation, ActorSpawnParams);

		if(FireSound!= NULL)
		{
			UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
		}
		if(FireAnimation!= NULL && AnimInstance!= NULL)
		{
			AnimInstance->Montage_Play(FireAnimation, 1.0f);
		}




	}
}

void AMonsterShooter::MoveForward(float Value)
{
	if(Value!=0.0f)
	{
		AddMovementInput(GetActorForwardVector(), Value);

	}
}

void AMonsterShooter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		AddMovementInput(GetActorRightVector(), Value);

	}
}

void AMonsterShooter::TurnAtRate(float Rate)
{
	AddControllerYawInput(Rate * TurnRate * GetWorld()->GetDeltaSeconds());
}

void AMonsterShooter::LookAtRate(float Rate)
{
	AddControllerPitchInput(Rate * LookUpRate *(-1.0f)* GetWorld()->GetDeltaSeconds());
}

void AMonsterShooter::DealDamage(float DamageAmount)
{
	Health -= DamageAmount;
	if(Health<=0.0f)
	{
		//restart game
		//Restart();
		AMonsterShooterGameMode* MyGameMode = Cast<AMonsterShooterGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

		if(MyGameMode)
		{
			MyGameMode->RestartGameplay(false);
		}
		Destroy();
	}

}

