﻿// Fill out your copyright notice in the Description page of Project Settings.

#include "ARPG_AnimNotify.h"
#include "CharacterBase.h"
#include "HumanBase.h"
#include "Item/Weapon/ARPG_WeaponBase.h"




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
			Weapon->SetEnableNearAttackTrace(false);
		}
	}
}