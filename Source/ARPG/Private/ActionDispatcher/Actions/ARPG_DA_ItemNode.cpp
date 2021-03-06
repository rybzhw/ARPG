﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "ARPG_DA_ItemNode.h"
#include "CharacterBase.h"
#include "ARPG_InventoryComponent.h"
#include "ARPG_ItemCoreBase.h"
#include "Utils/XD_ItemFunctionLibrary.h"

TSet<AActor*> UARPG_DA_MoveItem::GetAllRegistableEntities() const
{
	return { Role.Get(), Other.Get() };
}

void UARPG_DA_MoveItem::WhenActionActived()
{
	if (UARPG_InventoryComponent* Inventory = Other->FindComponentByClass<UARPG_InventoryComponent>())
	{
		Role->OpenMoveItemPanel(Inventory);
	}
	Role->OnCloseMoveItemPanelNative.AddWeakLambda(this, [=]()
		{
			Role->OnCloseMoveItemPanelNative.RemoveAll(this);
			ExecuteEventAndFinishAction(WhenMoveItemFinished);
		});
}

void UARPG_DA_MoveItem::WhenActionDeactived()
{
	Super::WhenActionDeactived();

	Role->CloseMoveItemPanel();
}

bool UARPG_DA_MoveItem::IsActionValid() const
{
	return Role.IsValid() && Other.IsValid() && Role->GetDistanceTo(Other.Get()) < ValidDistance;
}

void UARPG_DA_MoveItem_Record::WhenActionActived()
{
	Super::WhenActionActived();

	Role->Inventory->OnAddItem.AddDynamic(this, &UARPG_DA_MoveItem_Record::WhenAddItem);
	Role->Inventory->OnRemoveItem.AddDynamic(this, &UARPG_DA_MoveItem_Record::WhenRemoveItem);
}

void UARPG_DA_MoveItem_Record::WhenActionDeactived()
{
	Super::WhenActionDeactived();

	Role->Inventory->OnAddItem.RemoveAll(this);
	Role->Inventory->OnRemoveItem.RemoveAll(this);
}

void UARPG_DA_MoveItem_Record::WhenAddItem(UXD_ItemCoreBase* ItemCore, int32 AddNumber, int32 ExistNumber)
{
	UXD_ItemFunctionLibrary::AddItemNumber(RoleAddedItems, ItemCore, AddNumber, this);
	UXD_ItemFunctionLibrary::RemoveItemNumber(RoleRemovedItems, ItemCore, AddNumber);

	WhenAddedItem.ExecuteIfBound();
}

void UARPG_DA_MoveItem_Record::WhenRemoveItem(UXD_ItemCoreBase* ItemCore, int32 RemoveNumber, int32 ExistNumber)
{
	UXD_ItemFunctionLibrary::AddItemNumber(RoleRemovedItems, ItemCore, RemoveNumber, this);
	UXD_ItemFunctionLibrary::RemoveItemNumber(RoleAddedItems, ItemCore, RemoveNumber);

	WhenRemovedItem.ExecuteIfBound();
}

TSet<AActor*> UARPG_DA_TradeItem::GetAllRegistableEntities() const
{
	return { Role.Get(), Other.Get() };
}

void UARPG_DA_TradeItem::WhenActionActived()
{
	if (UARPG_InventoryComponent * Inventory = Other->FindComponentByClass<UARPG_InventoryComponent>())
	{
		Role->OpenTradeItemPanel(Inventory);
	}
	Role->OnCloseTradeItemPanelNative.AddWeakLambda(this, [=]()
		{
			Role->OnCloseTradeItemPanelNative.RemoveAll(this);
			ExecuteEventAndFinishAction(WhenTradeItemFinished);
		});
}

void UARPG_DA_TradeItem::WhenActionDeactived()
{
	Super::WhenActionDeactived();

	Role->OnCloseTradeItemPanelNative.RemoveAll(this);
	Role->CloseTradeItemPanel();
}

bool UARPG_DA_TradeItem::IsActionValid() const
{
	return Role.IsValid() && Other.IsValid() && Role->GetDistanceTo(Other.Get()) < ValidDistance;
}

void UARPG_DA_TradeItem_Record::WhenActionActived()
{
	Super::WhenActionActived();

	Role->Inventory->OnAddItem.AddDynamic(this, &UARPG_DA_TradeItem_Record::WhenBuyItem);
	Role->Inventory->OnRemoveItem.AddDynamic(this, &UARPG_DA_TradeItem_Record::WhenSellItem);
}

void UARPG_DA_TradeItem_Record::WhenActionDeactived()
{
	Super::WhenActionDeactived();

	Role->Inventory->OnAddItem.RemoveAll(this);
	Role->Inventory->OnRemoveItem.RemoveAll(this);
}

void UARPG_DA_TradeItem_Record::WhenBuyItem(UXD_ItemCoreBase* ItemCore, int32 AddNumber, int32 ExistNumber)
{
	UXD_ItemFunctionLibrary::AddItemNumber(RoleBoughtItems, ItemCore, AddNumber, this);
	UXD_ItemFunctionLibrary::RemoveItemNumber(RoleSelledItems, ItemCore, AddNumber);

	WhenBoughtItem.ExecuteIfBound();
}

void UARPG_DA_TradeItem_Record::WhenSellItem(UXD_ItemCoreBase* ItemCore, int32 RemoveNumber, int32 ExistNumber)
{
	UXD_ItemFunctionLibrary::AddItemNumber(RoleSelledItems, ItemCore, RemoveNumber, this);
	UXD_ItemFunctionLibrary::RemoveItemNumber(RoleBoughtItems, ItemCore, RemoveNumber);

	WhenSelledItem.ExecuteIfBound();
}
