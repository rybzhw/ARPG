﻿// Fill out your copyright notice in the Description page of Project Settings.

#include "ARPG_AnimNotify.h"
#include <Components/SkeletalMeshComponent.h>
#include <Animation/AnimMontage.h>
#include <Animation/AnimInstance.h>
#include "CharacterBase.h"
#include "HumanBase.h"
#include "ARPG_WeaponBase.h"



void UARPG_PlayMontageByState::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (ACharacterBase* Character = Cast<ACharacterBase>(MeshComp->GetOwner()))
	{
		if (bClientMaster || Character->HasAuthority())
		{
			Character->PlayMontage(Montage, 1.f, StartSectionName, bClientMaster);
		}
	}
}

FString UARPG_PlayMontageByState::GetNotifyName_Implementation() const
{
	return FString::Printf(TEXT("动画跳转[%s]"), Montage ? *Montage->GetName() : TEXT("None"));
}

void UARPG_PlayMontageByInput::NotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration)
{

}

void UARPG_PlayMontageByInput::NotifyTick(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float FrameDeltaTime)
{
	if (ACharacterBase* Character = Cast<ACharacterBase>(MeshComp->GetOwner()))
	{
		if (Character->IsLocallyControlled())
		{
			if (bIsReleased ? Character->ARPG_InputIsReleased(InputType) : Character->ARPG_InputIsPressed(InputType))
			{
				if (UAnimMontage* CurPlayingMontage = Cast<UAnimMontage>(Animation))
				{
					if (MeshComp->GetAnimInstance()->Montage_IsPlaying(CurPlayingMontage))
					{
						Character->PlayMontage(Montage, 1.f, StartSectionName, bClientMaster);
					}
				}
			}
		}
	}
}

void UARPG_PlayMontageByInput::NotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation)
{

}

FString UARPG_PlayMontageByInput::GetNotifyName_Implementation() const
{
	return FString::Printf(TEXT("输入_动画跳转[%s]"), Montage ? *Montage->GetName() : TEXT("None"));
}

void UARPG_DefenseState::NotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration)
{
	if (ACharacterBase* Character = Cast<ACharacterBase>(MeshComp->GetOwner()))
	{
		Character->bIsDefense = true;
	}
}

void UARPG_DefenseState::NotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation)
{
	if (ACharacterBase* Character = Cast<ACharacterBase>(MeshComp->GetOwner()))
	{
		Character->bIsDefense = false;
	}
}

FString UARPG_DefenseState::GetNotifyName_Implementation() const
{
	return TEXT("防御状态");
}

void UARPG_DefenseSwipeState::NotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration)
{
	if (ACharacterBase* Character = Cast<ACharacterBase>(MeshComp->GetOwner()))
	{
		Character->bIsDefenseSwipe = true;
	}
}

void UARPG_DefenseSwipeState::NotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation)
{
	if (ACharacterBase* Character = Cast<ACharacterBase>(MeshComp->GetOwner()))
	{
		Character->bIsDefenseSwipe = false;
	}
}

FString UARPG_DefenseSwipeState::GetNotifyName_Implementation() const
{
	return TEXT("防御反击状态");
}

void UARPG_DodgeState::NotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration)
{
	if (ACharacterBase* Character = Cast<ACharacterBase>(MeshComp->GetOwner()))
	{
		Character->bIsDodging = true;
	}
}

void UARPG_DodgeState::NotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation)
{
	if (ACharacterBase* Character = Cast<ACharacterBase>(MeshComp->GetOwner()))
	{
		Character->bIsDodging = false;
	}
}

FString UARPG_DodgeState::GetNotifyName_Implementation() const
{
	return TEXT("闪避状态");
}

void UARPG_AddToughnessValue::NotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration)
{
	if (ACharacterBase* Character = Cast<ACharacterBase>(MeshComp->GetOwner()))
	{
		Character->ToughnessValue += AddToughnessValue;
	}
}

void UARPG_AddToughnessValue::NotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation)
{
	if (ACharacterBase* Character = Cast<ACharacterBase>(MeshComp->GetOwner()))
	{
		Character->ToughnessValue -= AddToughnessValue;
	}
}

FString UARPG_AddToughnessValue::GetNotifyName_Implementation() const
{
	return FString::Printf(TEXT("增加强韧度[%s]"), *FString::SanitizeFloat(AddToughnessValue, 0));
}

void UARPG_Human_TakeWeaponPos::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (AHumanBase* Human = Cast<AHumanBase>(MeshComp->GetOwner()))
	{
		if (bPullOutWeapon)
		{
			Human->LetTheWeaponInHand();
		}
		else
		{
			Human->LetTheWeaponInWeaponBack();
		}
	}
}

void UARPG_Human_WeaponTrace::NotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration)
{
	if (AHumanBase* Human = Cast<AHumanBase>(MeshComp->GetOwner()))
	{
		if (AARPG_WeaponBase* Weapon = bIsLeftWeapon ? Human->LeftWeapon : Human->RightWeapon)
		{
			Weapon->ReceiveDamageAction = ReceiveDamageAction;
			Weapon->SetEnableNearAttackTrace(true);
		}
	}
}

void UARPG_Human_WeaponTrace::NotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation)
{
	if (AHumanBase* Human = Cast<AHumanBase>(MeshComp->GetOwner()))
	{
		if (AARPG_WeaponBase* Weapon = bIsLeftWeapon ? Human->LeftWeapon : Human->RightWeapon)
		{
			Weapon->ReceiveDamageAction = nullptr;
			Weapon->SetEnableNearAttackTrace(false);
		}
	}
}
