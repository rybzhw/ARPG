﻿// Fill out your copyright notice in the Description page of Project Settings.

#include "Human_AnimNotify.h"
#include <Components/SkeletalMeshComponent.h>
#include "HumanBase.h"
#include "ARPG_WeaponBase.h"
#include "ARPG_BowBase.h"
#include "ARPG_ArrowBase.h"
#include "Animation/AnimInstance.h"


void UARPG_DodgeAttack::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{
	if (AHumanBase* Human = Cast<AHumanBase>(MeshComp->GetOwner()))
	{
		if (Human->IsLocallyControlled())
		{
			if (UAnimMontage* CurPlayingMontage = Cast<UAnimMontage>(Animation))
			{
				if (MeshComp->GetAnimInstance()->Montage_IsPlaying(CurPlayingMontage))
				{
					if (Human->ARPG_AnyInputIsPressed(ARPG_InputType::ToBitMask(EARPG_InputType::LeftLightAttack) | ARPG_InputType::ToBitMask(EARPG_InputType::LeftHeavyAttack)))
					{
						if (Human->LeftWeapon)
						{
							Human->LeftWeapon->AttackAnimSet->InvokePlayDodgeAnim(Human, DodgeDirection);
						}
					}
					else if (Human->ARPG_AnyInputIsPressed(ARPG_InputType::ToBitMask(EARPG_InputType::RightLightAttack) | ARPG_InputType::ToBitMask(EARPG_InputType::RightHeavyAttack)))
					{
						if (Human->RightWeapon)
						{
							Human->RightWeapon->AttackAnimSet->InvokePlayDodgeAnim(Human, DodgeDirection);
						}
					}
				}
			}
		}
	}
}

FString UARPG_DodgeAttack::GetNotifyName_Implementation() const
{
	return TEXT("闪避攻击");
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

FString UARPG_Human_TakeWeaponPos::GetNotifyName_Implementation() const
{
	return TEXT("武器位置");
}

void UARPG_Human_WeaponTrace::NotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration)
{
	if (AHumanBase* Human = Cast<AHumanBase>(MeshComp->GetOwner()))
	{
		if (AARPG_WeaponBase* Weapon = bIsLeftWeapon ? Human->LeftWeapon : Human->RightWeapon)
		{
			Weapon->ReceiveDamageAction = ReceiveDamageAction;
			Weapon->AnimAddHitStunValue = AddHitStunValue;
			Weapon->BeakBackDistance = BeakBackDistance;
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

FString UARPG_Human_WeaponTrace::GetNotifyName_Implementation() const
{
	return TEXT("武器伤害检测");
}

void UARPG_Human_FallingAttackTrace::NotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration)
{
	if (AHumanBase* Human = Cast<AHumanBase>(MeshComp->GetOwner()))
	{
		if (AARPG_WeaponBase* Weapon = bIsLeftWeapon ? Human->LeftWeapon : Human->RightWeapon)
		{
			Weapon->ReceiveDamageAction = ReceiveDamageAction;
			Weapon->AnimAddHitStunValue = AddHitStunValue;
			Weapon->SetEnableFallingAttackTrace(true, bClearIgnoreList);
		}
	}
}

void UARPG_Human_FallingAttackTrace::NotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation)
{
	if (AHumanBase* Human = Cast<AHumanBase>(MeshComp->GetOwner()))
	{
		if (AARPG_WeaponBase* Weapon = bIsLeftWeapon ? Human->LeftWeapon : Human->RightWeapon)
		{
			Weapon->SetEnableFallingAttackTrace(false, bClearIgnoreList);
		}
	}
}

void UARPG_Human_PullOutArrow::NotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration)
{
	if (AHumanBase* Human = Cast<AHumanBase>(MeshComp->GetOwner()))
	{
		if (AARPG_WeaponBase* Weapon = bIsLeftWeapon ? Human->LeftWeapon : Human->RightWeapon)
		{
			if (AARPG_BowBase* Bow = Cast<AARPG_BowBase>(Weapon))
			{
				Bow->SpawnArrowInHand();
				Bow->HoldingTime = 0.f;
			}
		}
	}
}

void UARPG_Human_PullOutArrow::NotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation)
{
	if (AHumanBase* Human = Cast<AHumanBase>(MeshComp->GetOwner()))
	{
		if (AARPG_WeaponBase* Weapon = bIsLeftWeapon ? Human->LeftWeapon : Human->RightWeapon)
		{
			if (AARPG_BowBase* Bow = Cast<AARPG_BowBase>(Weapon))
			{
				if (Bow->HoldingArrow)
				{
					Bow->HoldingArrow->Destroy();
					Bow->HoldingArrow = nullptr;
				}
			}
		}
	}
}

void UARPG_Human_PullBow::NotifyTick(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float FrameDeltaTime)
{
	if (AHumanBase* Human = Cast<AHumanBase>(MeshComp->GetOwner()))
	{
		if (AARPG_WeaponBase* Weapon = bIsLeftWeapon ? Human->LeftWeapon : Human->RightWeapon)
		{
			if (AARPG_BowBase* Bow = Cast<AARPG_BowBase>(Weapon))
			{
				Bow->HoldingTime += FrameDeltaTime;
			}
		}
	}
}

void UARPG_Human_LaunchArrow::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (AHumanBase* Human = Cast<AHumanBase>(MeshComp->GetOwner()))
	{
		if (AARPG_WeaponBase* Weapon = bIsLeftWeapon ? Human->LeftWeapon : Human->RightWeapon)
		{
			if (AARPG_BowBase* Bow = Cast<AARPG_BowBase>(Weapon))
			{
				Bow->LaunchArrow(FullBowTime);
			}
		}
	}
}

void USRDAF_LeftWeapon::SetReceiveDamageAction(USkeletalMeshComponent* MeshComp, TSubclassOf<class UReceiveDamageActionBase> ReceiveDamageAction) const
{
	if (AHumanBase* Human = Cast<AHumanBase>(MeshComp->GetOwner()))
	{
		if (Human->LeftWeapon)
		{
			Human->LeftWeapon->ReceiveDamageAction = ReceiveDamageAction;
		}
	}
}

void USRDAF_RightWeapon::SetReceiveDamageAction(USkeletalMeshComponent* MeshComp, TSubclassOf<class UReceiveDamageActionBase> ReceiveDamageAction) const
{
	if (AHumanBase* Human = Cast<AHumanBase>(MeshComp->GetOwner()))
	{
		if (Human->RightWeapon)
		{
			Human->RightWeapon->ReceiveDamageAction = ReceiveDamageAction;
		}
	}
}

void USAHSVF_LeftWeapon::SetAddHitStunValue(USkeletalMeshComponent* MeshComp, float AddHitStunValue) const
{
	if (AHumanBase* Human = Cast<AHumanBase>(MeshComp->GetOwner()))
	{
		if (Human->LeftWeapon)
		{
			Human->LeftWeapon->AnimAddHitStunValue += AddHitStunValue;
		}
	}
}

void USAHSVF_LeftWeapon::Reset(USkeletalMeshComponent* MeshComp, float AddHitStunValue) const
{
	if (AHumanBase* Human = Cast<AHumanBase>(MeshComp->GetOwner()))
	{
		if (Human->LeftWeapon)
		{
			Human->LeftWeapon->AnimAddHitStunValue -= AddHitStunValue;
		}
	}
}

void USAHSVF_RightWeapon::SetAddHitStunValue(USkeletalMeshComponent* MeshComp, float AddHitStunValue) const
{
	if (AHumanBase* Human = Cast<AHumanBase>(MeshComp->GetOwner()))
	{
		if (Human->RightWeapon)
		{
			Human->RightWeapon->AnimAddHitStunValue += AddHitStunValue;
		}
	}
}

void USAHSVF_RightWeapon::Reset(USkeletalMeshComponent* MeshComp, float AddHitStunValue) const
{
	if (AHumanBase* Human = Cast<AHumanBase>(MeshComp->GetOwner()))
	{
		if (Human->RightWeapon)
		{
			Human->RightWeapon->AnimAddHitStunValue -= AddHitStunValue;
		}
	}
}

void USBBDF_LeftWeapon::SetBeakBackDistance(USkeletalMeshComponent* MeshComp, float BeakBackDistance) const
{
	if (AHumanBase* Human = Cast<AHumanBase>(MeshComp->GetOwner()))
	{
		if (Human->LeftWeapon)
		{
			Human->LeftWeapon->BeakBackDistance = BeakBackDistance;
		}
	}
}

void USBBDF_RightWeapon::SetBeakBackDistance(USkeletalMeshComponent* MeshComp, float BeakBackDistance) const
{
	if (AHumanBase* Human = Cast<AHumanBase>(MeshComp->GetOwner()))
	{
		if (Human->RightWeapon)
		{
			Human->RightWeapon->BeakBackDistance = BeakBackDistance;
		}
	}
}
