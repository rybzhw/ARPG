﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/EngineTypes.h"
#include "ARPG_CharacterBehaviorType.h"
#include "ARPG_CharacterBehaviorBase.generated.h"

class UARPG_CharacterBehaviorConfigBase;

/**
 * 
 */
UCLASS(abstract)
class ARPG_API UARPG_CharacterBehaviorBase : public UObject
{
	GENERATED_BODY()
public:
	virtual UWorld* GetWorld() const;

public:
	UPROPERTY(BlueprintReadOnly, Category = "行为")
	class ACharacterBase* Character;

	void AbortBehavior(class ACharacterBase* Executer, const FOnCharacterBehaviorAbortFinished& OnBehaviorAbortFinished);
protected:
	virtual void ExecuteBehavior(class ACharacterBase* Executer) { ReceiveExecuteBehavior(Executer); }
	UFUNCTION(BlueprintImplementableEvent, Category = "行为", meta = (DisplayName = "ExecuteBehavior"))
	void ReceiveExecuteBehavior(class ACharacterBase* Executer);

	virtual void AbortBehavior(class ACharacterBase* Executer) { ReceiveAbortBehavior(Executer); }
	UFUNCTION(BlueprintNativeEvent, Category = "行为", meta = (DisplayName = "AbortBehavior"))
	void ReceiveAbortBehavior(class ACharacterBase* Executer);
	void ReceiveAbortBehavior_Implementation(class ACharacterBase* Executer) { OnBehaviorAbortFinished.ExecuteIfBound(); }

public:
	UFUNCTION(BlueprintCallable, Category = "行为")
	void FinishExecute(bool Succeed);

	FOnCharacterBehaviorFinished OnBehaviorFinished;

	UFUNCTION(BlueprintCallable, Category = "行为")
	void FinishAbort();

	FOnCharacterBehaviorAbortFinished OnBehaviorAbortFinished;
};

UCLASS(abstract, Blueprintable)
class ARPG_API UARPG_CharacterBehaviorConfigurable : public UARPG_CharacterBehaviorBase
{
	GENERATED_BODY()
		
	friend class UARPG_CharacterBehaviorConfigBase;
public:
	UPROPERTY(BlueprintReadOnly, Category = "行为")
	const UARPG_CharacterBehaviorConfigBase* Config;

public:
	template<typename T>
	const T* GetConfig() const { return Cast<const T>(Config); }

	UFUNCTION(BlueprintCallable, Category = "行为", meta = (DeterminesOutputType = "ConfigType"))
	UARPG_CharacterBehaviorConfigBase* GetConfig(TSubclassOf<UARPG_CharacterBehaviorConfigBase> ConfigType) const { return const_cast<UARPG_CharacterBehaviorConfigBase*>(Config); }
};

UCLASS(abstract, EditInlineNew, collapsecategories, Blueprintable)
class ARPG_API UARPG_CharacterBehaviorConfigBase : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, Category = "配置", Transient)
	TSubclassOf<UARPG_CharacterBehaviorConfigurable> BehaviorType;

	UPROPERTY()
	mutable TMap<class ACharacterBase*, class UARPG_CharacterBehaviorConfigurable*> BehaviorMap;

	UARPG_CharacterBehaviorConfigurable* ExecuteBehavior(class ACharacterBase* Character, const FOnCharacterBehaviorFinished& OnBehaviorFinished) const;

	void AbortBehavior(class ACharacterBase* Character, const FOnCharacterBehaviorAbortFinished& OnBehaviorAbortFinished);

	virtual FString GetDescribe() const;
};