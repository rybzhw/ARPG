﻿// Fill out your copyright notice in the Description page of Project Settings.

#include "ARPG_EquipmentBase.h"
#include "CharacterBase.h"
#include "HumanBase.h"


#define LOCTEXT_NAMESPACE "ARPG_Item"

AARPG_EquipmentBase::AARPG_EquipmentBase()
{
	
}

void AARPG_EquipmentBase::UseItemImpl_Implementation(class UARPG_ItemCoreBase* ItemCore, class ACharacterBase* ItemOwner, EUseItemInput UseItemInput) const
{
	ItemOwner->EquipEquipment(ItemCore, UseItemInput);
}

FText AARPG_EquipmentBase::GetItemTypeDescImpl_Implementation(const class UXD_ItemCoreBase* ItemCore) const
{
	return LOCTEXT("ARPG_Equipment", "装备");
}

void AARPG_EquipmentBase::WhenUse(class ACharacterBase* ItemOwner)
{
	Super::WhenUse(ItemOwner);

	AttachToComponent(ItemOwner->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, EquipSocketName);
	if (EquipSocketName.IsNone())
	{
		if (USkeletalMeshComponent* SkeletalMeshComponent = Cast<USkeletalMeshComponent>(GetRootComponent()))
		{
			SkeletalMeshComponent->SetMasterPoseComponent(ItemOwner->GetMesh());
		}
	}
}

void AARPG_EquipmentBase::WhenNotUse(class ACharacterBase* ItemOwner)
{
	Super::WhenNotUse(ItemOwner);
}

void AARPG_EquipmentBase::WhenRemoveFromInventory_Implementation(class AActor* ItemOwner, class UXD_ItemCoreBase* ItemCore, int32 RemoveNumber, int32 ExistNumber) const
{
	if (ExistNumber <= 0)
	{
		if (AHumanBase* Human = Cast<AHumanBase>(ItemOwner))
		{
			int32 FindIndex = Human->EquipmentList.IndexOfByPredicate([ItemCore](AARPG_EquipmentBase* E_Equipment) {return E_Equipment->EqualForItemCore(ItemCore); });
			if (FindIndex != INDEX_NONE)
			{
				Human->EquipmentList.RemoveAt(FindIndex);
				Human->OnRep_EquipmentList();
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE
