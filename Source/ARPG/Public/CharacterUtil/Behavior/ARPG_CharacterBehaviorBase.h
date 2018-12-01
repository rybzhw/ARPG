﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/EngineTypes.h"
#include "ARPG_CharacterBehaviorBase.generated.h"

/**
 * 
 */
UCLASS(abstract, Blueprintable)
class ARPG_API UARPG_CharacterBehaviorBase : public UObject
{
	GENERATED_BODY()
public:
	virtual UWorld* GetWorld() const;

public:
	UPROPERTY(BlueprintReadOnly, Category = "行为")
	class ACharacterBase* Character;

	UPROPERTY(BlueprintReadOnly, Category = "行为")
	const class UARPG_CharacterBehaviorConfigBase* Config;

	virtual void ExecuteBehavior(class ACharacterBase* Executer, const FVector& Location, const FRotator& Rotation) { ReceiveExecuteBehavior(Executer, Location, Rotation); }
	UFUNCTION(BlueprintImplementableEvent, Category = "行为", meta = (DisplayName = "ExecuteBehavior"))
	void ReceiveExecuteBehavior(class ACharacterBase* Executer, const FVector& Location, const FRotator& Rotation);

	virtual void AbortBehavior(class ACharacterBase* Executer) { ReceiveAbortBehavior(Executer); }
	UFUNCTION(BlueprintNativeEvent, Category = "行为", meta = (DisplayName = "AbortBehavior"))
	void ReceiveAbortBehavior(class ACharacterBase* Executer);
	void ReceiveAbortBehavior_Implementation(class ACharacterBase* Executer) { OnBehaviorAbortFinished.ExecuteIfBound(); }

	UFUNCTION(BlueprintCallable, Category = "行为")
	void FinishExecute(bool Succeed);

	DECLARE_DELEGATE_OneParam(FOnBehaviorFinished, bool /*Succeed*/)
	FOnBehaviorFinished OnBehaviorFinished;

	UFUNCTION(BlueprintCallable, Category = "行为")
	void FinishAbort();

	DECLARE_DELEGATE(FOnBehaviorAbortFinished);
	FOnBehaviorAbortFinished OnBehaviorAbortFinished;
};

UCLASS(abstract, EditInlineNew, collapsecategories, Blueprintable)
class ARPG_API UARPG_CharacterBehaviorConfigBase : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, Category = "配置")
	UClass* BehaviorType;

	UPROPERTY()
	mutable TMap<class ACharacterBase*, class UARPG_CharacterBehaviorBase*> BehaviorMap;

	void ExecuteBehavior(class ACharacterBase* Character, const FVector& Location, const FRotator& Rotation, const UARPG_CharacterBehaviorBase::FOnBehaviorFinished& OnBehaviorFinished) const;

	void AbortBehavior(class ACharacterBase* Character, const UARPG_CharacterBehaviorBase::FOnBehaviorAbortFinished& OnBehaviorAbortFinished);

	virtual FString GetDescribe() const;
};
