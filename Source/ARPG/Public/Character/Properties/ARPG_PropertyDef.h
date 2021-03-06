﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ARPG_PropertyDef.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct ARPG_API FARPG_PropertyChangeContext
{
	GENERATED_BODY()
public:
	FARPG_PropertyChangeContext() = default;
	FARPG_PropertyChangeContext(const TSoftObjectPtr<UObject>& Causer, const TSoftObjectPtr<UObject>& Instigator)
		:Causer(Causer), Instigator(Instigator)
	{}

	// 执行修改的实例，例如是武器、BUFF等
	TSoftObjectPtr<UObject> Causer;
	// 修改的始作俑者，一般为角色
	TSoftObjectPtr<UObject> Instigator;
};

USTRUCT(BlueprintType)
struct ARPG_API FARPG_GameplayFloatPropertyBase
{
	GENERATED_BODY()
public:
};

USTRUCT(BlueprintType)
struct FARPG_FloatProperty_ModifyConfig
{
	GENERATED_BODY()
public:
	FARPG_FloatProperty_ModifyConfig() = default;
	FARPG_FloatProperty_ModifyConfig(float Value, const FName& DescribeTag, const TSoftObjectPtr<UObject>& Causer, const TSoftObjectPtr<UObject>& Instigator)
		:Value(Value), DescribeTag(DescribeTag), ChangeContext(Causer, Instigator)
	{}

	float Value;
	FName DescribeTag;
	FARPG_PropertyChangeContext ChangeContext;
};

USTRUCT(BlueprintType)
struct ARPG_API FARPG_FloatProperty : public FARPG_GameplayFloatPropertyBase
{
	GENERATED_BODY()
public:
	FARPG_FloatProperty() = default;
	FARPG_FloatProperty(float InitValue)
		:Base(InitValue), Final(InitValue)
	{}

private:
	UPROPERTY(SaveGame)
	float Base;

	UPROPERTY(NotReplicated)
	float Additive = 0.f;

	UPROPERTY(NotReplicated)
	float Multiple = 1.f;

	UPROPERTY(VisibleAnywhere)
	float Final;

	FORCEINLINE void UpdateFinalValue() { Final = (Base + Additive) * Multiple; }
public:
	FORCEINLINE float Value() const { return Final; }

	FORCEINLINE float GetBaseValue() const { return Base; }
	void SetBaseValue(float InBaseValue)
	{
		Base = InBaseValue;
		UpdateFinalValue();
	}

	FORCEINLINE float GetAdditiveValue() const { return Additive; }
	void PushAdditiveModifier(const FARPG_FloatProperty_ModifyConfig& ModifyConfig);
	void PopAdditiveModifier(const FARPG_FloatProperty_ModifyConfig& ModifyConfig);
	void ChangePushedAdditiveModifier(const FARPG_FloatProperty_ModifyConfig& OldModifyConfig, const FARPG_FloatProperty_ModifyConfig& ChangeToModifyConfig);

	FORCEINLINE float GetMultipleValue() const { return Multiple; }
	void PushMultipleModifier(const FARPG_FloatProperty_ModifyConfig& ModifyConfig);
	void PopMultipleModifier(const FARPG_FloatProperty_ModifyConfig& ModifyConfig);
	void ChangePushedMultipleModifier(const FARPG_FloatProperty_ModifyConfig& OldModifyConfig, const FARPG_FloatProperty_ModifyConfig& ChangeToModifyConfig);
};

#define ARPG_FLOAT_PPROPERTY_ACCESSORS(Property) \
FORCEINLINE float Get##Property() const { return Property.Value(); } \
FORCEINLINE void Property##_PushAdditiveModifier(const FARPG_FloatProperty_ModifyConfig& ModifyConfig) { Property.PushAdditiveModifier(ModifyConfig); } \
FORCEINLINE void Property##_PopAdditiveModifier(const FARPG_FloatProperty_ModifyConfig& ModifyConfig) { Property.PopAdditiveModifier(ModifyConfig); } \
FORCEINLINE void Property##_PushMultipleModifier(const FARPG_FloatProperty_ModifyConfig& ModifyConfig) { Property.PushMultipleModifier(ModifyConfig); } \
FORCEINLINE void Property##_PopMultipleModifier(const FARPG_FloatProperty_ModifyConfig& ModifyConfig) { Property.PopMultipleModifier(ModifyConfig); }

UENUM()
enum class EARPG_PropertyOperatorOperand : uint8
{
	Additive,
	Multiple,
	Set
};

UENUM()
enum class EARPG_PropertyModifierOperand : uint8
{
	Additive,
	Multiple
};
