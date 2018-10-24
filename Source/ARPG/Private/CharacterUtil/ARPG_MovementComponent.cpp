﻿// Fill out your copyright notice in the Description page of Project Settings.

#include "ARPG_MovementComponent.h"
#include <GameFramework/Character.h>
#include <AIController.h>
#include <Kismet/KismetMathLibrary.h>
#include "ARPG_PlayerControllerBase.h"



void UARPG_MovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//锁定时旋转输入一直朝向锁定目标，除非有移动输入
	if (GetCharacterOwing()->HasAuthority() || GetCharacterOwing()->IsLocallyControlled())
	{
		switch (ALS_MovementMode)
		{
		case EALS_MovementMode::Grounded:
			if (RootMotionRotationSpeed > 0.f && HasMovementInput() == false)
			{
				AActor* LockedTarget = nullptr;
				if (AARPG_PlayerControllerBase* PlayerController = Cast<AARPG_PlayerControllerBase>(GetCharacterOwing()->GetController()))
				{
					LockedTarget = PlayerController->GetLockedTarget();
				}
				else if (AAIController* AIController = Cast<AAIController>(GetCharacterOwing()->GetController()))
				{
					LockedTarget = AIController->GetFocusActor();
				}
				if (LockedTarget)
				{
					FRotator LockTargetRotation = UKismetMathLibrary::FindLookAtRotation(GetCharacterOwing()->GetActorLocation(), LockedTarget->GetActorLocation());
					FRotator CharacterRotation = GetCharacterRotation();
					TargetRotation = FRotator(CharacterRotation.Pitch, LockTargetRotation.Yaw, CharacterRotation.Roll);
					SetCharacterRotation(FMath::RInterpTo(CharacterRotation, TargetRotation, DeltaTime, RootMotionRotationSpeed));
				}
			}
			break;
		}
	}
}
