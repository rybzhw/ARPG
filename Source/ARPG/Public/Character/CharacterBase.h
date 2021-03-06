﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include <Perception/AISightTargetInterface.h>
#include <GenericTeamAgentInterface.h>
#include <Animation/AnimInstance.h>
#include <ScriptInterface.h>
#include "XD_SaveGameInterface.h"
#include "ARPG_InputBuffer.h"
#include "ARPG_LockOnTargetSystem.h"
#include "ARPG_CharacterAnimType.h"
#include "ItemTypeUtils.h"
#include "CharacterDamageType.h"
#include "ARPG_InteractInterface.h"
#include "ARPG_CollisionType.h"
#include "ARPG_CampType.h"
#include "ARPG_BattleType.h"
#include "BehaviorTreeEx.h"
#include "ARPG_CharacterBehaviorType.h"
#include "ARPG_AI_BattleInterface.h"
#include "XD_DispatchableEntityInterface.h"
#include "ARPG_NavAreaType.h"
#include "ARPG_PropertyDef.h"
#include "CharacterBase.generated.h"

class UCA_EnterReleaseStateBase;
class UARPG_AD_CharacterInteract;
class UARPG_ActionDispatcherBase;
class UCA_TurnConfigBase;
class UCA_CharacterTurnBase;
class UARPG_ReceiveDamageActionBase;
class UARPG_InventoryComponent;
class AARPG_ItemBase;
class UARPG_MovementComponent;
class UARPG_ItemCoreBase;
class USoundAttenuation;
class UAudioComponent;
class UPathFollowingComponent;
class AARPG_WeaponBase;
class AARPG_ArrowBase;
class AARPG_EquipmentBase;
class ACharacterBase;
class UXD_ItemCoreBase;

UENUM()
enum class EInteractEndResult : uint8
{
	CanNotInteract,
	Succeed,
	Failed
};

DECLARE_DELEGATE_OneParam(FOnInteractEnd, EInteractEndResult);

DECLARE_DELEGATE(FOnInteractAbortEnd);

UCLASS()
class ARPG_API ACharacterBase : public ACharacter,
	public IXD_SaveGameInterface,
	public IXD_DispatchableEntityInterface,
	public IARPG_LockOnTargetInterface,
	public IAISightTargetInterface,
	public IARPG_AI_BattleInterface,
	public IARPG_InteractInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACharacterBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	// Called when the game starts or when spawned
	void BeginPlay() override;

	void Destroyed() override;
	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void PreInitializeComponents() override;
public:
	// Called every frame
	void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void GetLifetimeReplicatedProps(TArray< class FLifetimeProperty > & OutLifetimeProps) const override;

	void OnConstruction(const FTransform& Transform) override;

	float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
public:
	bool NeedSave_Implementation() const override;
	void WhenGameInit_Implementation() override;

	void Reset() override;

	TArray<UARPG_ItemCoreBase*> GetReInitItemList() const;

	virtual TArray<UARPG_ItemCoreBase*> GetInitItemList() const;
	UFUNCTION(BlueprintImplementableEvent, Category = "角色|初始化", meta = (DisplayName = "GetInitItemList"))
	TArray<UARPG_ItemCoreBase*> ReceivedGetInitItemList() const;

	// 属性
public:
	// 生命

	UPROPERTY(EditAnywhere, Category = "角色|属性", Replicated, SaveGame, meta = (DisplayName = "当前生命"))
	float Health = 1000.f;
	UPROPERTY(EditAnywhere, Category = "角色|属性", Replicated, meta = (DisplayName = "生命上限"))
	FARPG_FloatProperty MaxHelath = 1000.f;

	UFUNCTION(BlueprintCallable, Category = "角色|属性", meta = (CompactNodeTitle = "Health"))
	FORCEINLINE float GetHealth() const { return Health; }
	UFUNCTION(BlueprintCallable, Category = "角色|属性")
	void SetHealth(float InHelath, const FARPG_PropertyChangeContext& ChangeContext);
	UFUNCTION(BlueprintCallable, Category = "角色|属性", meta = (CompactNodeTitle = "Health"))
	FORCEINLINE float GetMaxHealth() const { return MaxHelath.Value(); }

	void MaxHealth_AdjustForMaxChange(float InMaxHealth, const FARPG_PropertyChangeContext& ChangeContext);
	void MaxHealth_PushAdditiveModifier(const FARPG_FloatProperty_ModifyConfig& ModifyConfig);
	void MaxHealth_PopAdditiveModifier(const FARPG_FloatProperty_ModifyConfig& ModifyConfig);
	void MaxHealth_PushMultipleModifier(const FARPG_FloatProperty_ModifyConfig& ModifyConfig);
	void MaxHealth_PopMultipleModifier(const FARPG_FloatProperty_ModifyConfig& ModifyConfig);

	UFUNCTION(BlueprintCallable, Category = "角色|状态")
	bool IsAlive() const { return Health > 0.f; }
	UFUNCTION(BlueprintCallable, Category = "角色|状态")
	bool IsDead() const { return Health <= 0.f; }

	virtual void WhenDead();

	// 精力
	UPROPERTY(EditAnywhere, Category = "角色|属性", Replicated, SaveGame, meta = (DisplayName = "当前精力"))
	float Stamina = 300.f;
	UPROPERTY(EditAnywhere, Category = "角色|属性", Replicated, meta = (DisplayName = "精力上限"))
	FARPG_FloatProperty MaxStamina = 300.f;
	UFUNCTION(BlueprintCallable, Category = "角色|属性", meta = (CompactNodeTitle = "Stamina"))
	FORCEINLINE float GetStamina() const { return Stamina; }
	UFUNCTION(BlueprintCallable, Category = "角色|属性")
	void SetStamina(float InStamina, const FARPG_PropertyChangeContext& ChangeContext);
	UFUNCTION(BlueprintCallable, Category = "角色|属性", meta = (CompactNodeTitle = "MaxStamina"))
	FORCEINLINE float GetMaxStamina() const { return MaxStamina.Value(); }

	void MaxStamina_AdjustForMaxChange(float InMaxStamina, const FARPG_PropertyChangeContext& ChangeContext);
	void MaxStamina_PushAdditiveModifier(const FARPG_FloatProperty_ModifyConfig& ModifyConfig);
	void MaxStamina_PopAdditiveModifier(const FARPG_FloatProperty_ModifyConfig& ModifyConfig);
	void MaxStamina_PushMultipleModifier(const FARPG_FloatProperty_ModifyConfig& ModifyConfig);
	void MaxStamina_PopMultipleModifier(const FARPG_FloatProperty_ModifyConfig& ModifyConfig);

	// 精力恢复相关
	float StaminaCoolDownRemainTime = 0.f;
	UPROPERTY(EditAnywhere, Category = "角色|属性", Replicated, meta = (DisplayName = "精力恢复速度"))
	FARPG_FloatProperty StaminaRestoreSpeed = 60.f;
	ARPG_FLOAT_PPROPERTY_ACCESSORS(StaminaRestoreSpeed);
	UFUNCTION(BlueprintCallable, Category = "角色|属性", meta = (CompactNodeTitle = "StaminaRestoreSpeed", DisplayName = "GetStaminaRestoreSpeed"))
	float K2_GetStaminaRestoreSpeed() const { return StaminaRestoreSpeed.Value(); }

	UPROPERTY(EditAnywhere, Category = "角色|属性", Replicated, meta = (DisplayName = "精力再恢复冷却时间"))
	FARPG_FloatProperty StaminaRestoreCoolDownTime = 2.f;
	ARPG_FLOAT_PPROPERTY_ACCESSORS(StaminaRestoreCoolDownTime);
	UFUNCTION(BlueprintCallable, Category = "角色|属性", meta = (CompactNodeTitle = "StaminaRestoreCoolDownTime", DisplayName = "GetStaminaRestoreCoolDownTime"))
	float K2_GetStaminaRestoreCoolDownTime() const { return StaminaRestoreCoolDownTime.Value(); }

	UPROPERTY(EditAnywhere, Category = "角色|属性", Replicated, meta = (DisplayName = "冲刺精力消耗速度"))
	FARPG_FloatProperty SprintStaminaReduceSpeed = 30.f;
	ARPG_FLOAT_PPROPERTY_ACCESSORS(SprintStaminaReduceSpeed);
	UFUNCTION(BlueprintCallable, Category = "角色|属性", meta = (CompactNodeTitle = "SprintStaminaReduceSpeed", DisplayName = "GetSprintStaminaReduceSpeed"))
	float K2_GetSprintStaminaReduceSpeed() const { return SprintStaminaReduceSpeed.Value(); }

	// 总负重
	UPROPERTY(EditAnywhere, Category = "角色|属性", Replicated, meta = (DisplayName = "当前总计负重"))
	FARPG_FloatProperty Bearload;
	ARPG_FLOAT_PPROPERTY_ACCESSORS(Bearload);
	UFUNCTION(BlueprintCallable, Category = "角色|属性", meta = (CompactNodeTitle = "Bearload", DisplayName = "GetBearload"))
	FORCEINLINE float K2_GetBearload() const { return Bearload.Value(); }
	void SetBearload(float InBearload, const FARPG_PropertyChangeContext& ChangeContext);
	UPROPERTY(EditAnywhere, Category = "角色|属性", Replicated, meta = (DisplayName = "总计负重上限"))
	FARPG_FloatProperty MaxBearload = 100.f;
	ARPG_FLOAT_PPROPERTY_ACCESSORS(MaxBearload);
	UFUNCTION(BlueprintCallable, Category = "角色|属性", meta = (CompactNodeTitle = "MaxBearload", DisplayName = "GetMaxBearload"))
	float K2_GetMaxBearload() const { return MaxBearload.Value(); }

	// 装备负重
	UPROPERTY(EditAnywhere, Category = "角色|属性", Replicated, meta = (DisplayName = "当前装备负重"))
	FARPG_FloatProperty EquipLoad;
	ARPG_FLOAT_PPROPERTY_ACCESSORS(EquipLoad);
	UFUNCTION(BlueprintCallable, Category = "角色|属性", meta = (CompactNodeTitle = "EquipLoad", DisplayName = "GetEquipLoad"))
	FORCEINLINE float K2_GetEquipLoad() const { return EquipLoad.Value(); }
	void SetEquipLoad(float InEquipLoad, const FARPG_PropertyChangeContext& ChangeContext);
	UPROPERTY(EditAnywhere, Category = "角色|属性", Replicated, meta = (DisplayName = "装备负重上限"))
	FARPG_FloatProperty MaxEquipLoad = 100.f;
	ARPG_FLOAT_PPROPERTY_ACCESSORS(MaxEquipLoad);
	UFUNCTION(BlueprintCallable, Category = "角色|属性", meta = (CompactNodeTitle = "MaxEquipLoad", DisplayName = "GetMaxEquipLoad"))
	float K2_GetMaxEquipLoad() const { return MaxEquipLoad.Value(); }
	
	// 削韧积累值
	UPROPERTY(BlueprintReadOnly, Category = "角色|战斗", meta = (DisplayName = "削韧积累值"))
	float HitStunValue;
	UFUNCTION(BlueprintCallable, Category = "角色|属性", meta = (CompactNodeTitle = "HitStunValue", DisplayName = "GetHitStunValue"))
	float GetHitStunValue() const { return HitStunValue; }
	// 削韧积累值清空时间
	UPROPERTY(EditDefaultsOnly, Category = "角色|配置")
	float HitStunClearTime = 30.f;

	// 强韧度，当硬直积累量高于该值则播放硬直动画
	UPROPERTY(EditAnywhere, Category = "角色|属性", Replicated, meta = (DisplayName = "强韧度"))
	FARPG_FloatProperty Toughness = 50.f;
	ARPG_FLOAT_PPROPERTY_ACCESSORS(Toughness);
	UFUNCTION(BlueprintCallable, Category = "角色|属性", meta = (CompactNodeTitle = "Toughness", DisplayName = "GetToughness"))
	float K2_GetToughness() const { return Toughness.Value(); }

	// 返回削韧溢出数值，若没溢出则返回-1
	float AddHitStun(float Value);
	FTimerHandle ClearHitStun_TimeHandle;
	void ClearHitStun();

	// 状态
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "角色|状态")
	class UARPG_CharacterStateComponent* CharacterStateComponent;

	bool CanRun() const;
	bool CanSprint() const;
	bool IsSprinting() const;
	//DispatchableEntityInterface
public:
	FXD_DispatchableActionList GetCurrentDispatchableActions_Implementation() override;
	UPROPERTY(BlueprintReadOnly, Category = "角色|行为")
	TArray<UXD_DispatchableActionBase*> CurrentActions;

	UARPG_ActionDispatcherBase* GetCurrentMainDispatcher_Implementation() const override;
	void SetCurrentMainDispatcher_Implementation(UXD_ActionDispatcherBase* Dispatcher) override;
	UPROPERTY(BlueprintReadOnly, Category = "角色|行为")
	UARPG_ActionDispatcherBase* CurrentMainDispatcher;

	bool CanExecuteDispatcher_Implementation() const override;
	UFUNCTION(BlueprintCallable, Category = "角色|行为")
	bool IsInRoleSelection() const;
	UFUNCTION(BlueprintCallable, Category = "角色|行为")
	void AbortRoleSelection();

	//重生用
public:
	UPROPERTY(SaveGame, BlueprintReadOnly, Category = "角色|重生")
	FIntVector BornWorldOrigin;

	UPROPERTY(SaveGame, BlueprintGetter = GetRebornLocation, BlueprintSetter = SetRebornLocation, Category = "角色|重生")
	FVector BornLocation;

	UFUNCTION(BlueprintSetter)
	void SetRebornLocation(const FVector& RebornLocation);

	UFUNCTION(BlueprintGetter)
	FVector GetRebornLocation();

	//输入
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "角色|行为")
	FARPG_InputBuffer InputBuffer;

	UFUNCTION(BlueprintCallable, Category = "角色|行为")
	void ARPG_InputPressed(EARPG_InputType InputType);

	UFUNCTION(BlueprintCallable, Category = "角色|行为")
	void ARPG_InputReleased(EARPG_InputType InputType);

	UFUNCTION(BlueprintCallable, Category = "角色|行为")
	bool ARPG_InputIsPressed(UPARAM(meta = (Bitmask, BitmaskEnum = "EARPG_InputType")) int32 InputType) const;

	UFUNCTION(BlueprintCallable, Category = "角色|行为")
	bool ARPG_InputIsReleased(UPARAM(meta = (Bitmask, BitmaskEnum = "EARPG_InputType")) int32 InputType) const;

	UFUNCTION(BlueprintCallable, Category = "角色|行为")
	bool ARPG_AnyInputIsPressed(UPARAM(meta = (Bitmask, BitmaskEnum = "EARPG_InputType")) int32 InputType) const;

	UFUNCTION(BlueprintCallable, Category = "角色|行为")
	bool ARPG_AllInputIsReleased(UPARAM(meta = (Bitmask, BitmaskEnum = "EARPG_InputType")) int32 InputType) const;
	//移动行为
public:
	ECharacterGait InvokedGaitState;

	UFUNCTION(BlueprintCallable, Category = "角色|行为")
	void InvokeChangeMoveGait(ECharacterGait Gait);

	UFUNCTION(Reliable, WithValidation, Server)
	void InvokeChangeMoveGaitToServer(const ECharacterGait& Gait);
	virtual void InvokeChangeMoveGaitToServer_Implementation(const ECharacterGait& Gait);
	bool InvokeChangeMoveGaitToServer_Validate(const ECharacterGait& Gait) { return true; }

	//停止移动，也会停止转身行为
	UFUNCTION(BlueprintCallable, Category = "角色|行为")
	void StopMovement();

	UFUNCTION(BlueprintCallable, Category = "角色|行为")
	FVector GetMovementInputVector() const;

	//IARPG_LockOnTargetInterface
public:
	UPROPERTY(EditDefaultsOnly, Category = "角色|配置")
	TArray<FName> CanLockedSocketNames;

	UPROPERTY(EditDefaultsOnly, Category = "角色|配置")
	float MaxLockingDistance = 2000.f;

	virtual FVector GetTargetLocation_Implementation(const FName& CurLockSocketName) const override;

	virtual bool CanLockedOnTarget_Implementation(AController* Invoker, const FName& InvokeLockedSocketName) const override;

	virtual bool CanLockingOnTarget_Implementation(AController* Invoker, const FName& CurLockSocketName, bool& TryLockAgain) const override;

	UPROPERTY(Replicated)
	uint8 bIsLockingOther : 1;

	virtual void SetLockedTarget(AActor* NewLockedTarget);
	//动画
public:
	UFUNCTION(BlueprintCallable, Category = "角色|动作", meta = (AdvancedDisplay = Config, AutoCreateRefTerm = Config))
	float PlayMontage(UAnimMontage* MontageToPlay, const FARPG_MontagePlayConfig& Config, float InPlayRate = 1.f, FName StartSectionName = NAME_None, bool ClientMaster = true);

protected:
	float PlayMontageImpl(UAnimMontage* MontageToPlay, const FARPG_MontagePlayConfig& Config, float InPlayRate = 1.f, FName StartSectionName = NAME_None);

	UFUNCTION(Unreliable, WithValidation, NetMulticast)
	void MulticastPlayMontage(UAnimMontage* MontageToPlay, const FARPG_MontagePlayConfig& Config, float InPlayRate = 1.f, FName StartSectionName = NAME_None);
	virtual void MulticastPlayMontage_Implementation(UAnimMontage* MontageToPlay, const FARPG_MontagePlayConfig& Config, float InPlayRate = 1.f, FName StartSectionName = NAME_None);
	bool MulticastPlayMontage_Validate(UAnimMontage* MontageToPlay, const FARPG_MontagePlayConfig& Config, float InPlayRate = 1.f, FName StartSectionName = NAME_None) { return true; }

	UFUNCTION(Unreliable, WithValidation, NetMulticast)
	void MulticastPlayMontageSkipOwner(UAnimMontage* MontageToPlay, const FARPG_MontagePlayConfig& Config, float InPlayRate = 1.f, FName StartSectionName = NAME_None);
	virtual void MulticastPlayMontageSkipOwner_Implementation(UAnimMontage* MontageToPlay, const FARPG_MontagePlayConfig& Config, float InPlayRate = 1.f, FName StartSectionName = NAME_None);
	bool MulticastPlayMontageSkipOwner_Validate(UAnimMontage* MontageToPlay, const FARPG_MontagePlayConfig& Config, float InPlayRate = 1.f, FName StartSectionName = NAME_None) { return true; }

	UFUNCTION(Reliable, WithValidation, Server)
	void PlayMontageToServer(UAnimMontage* MontageToPlay, const FARPG_MontagePlayConfig& Config, float InPlayRate = 1.f, FName StartSectionName = NAME_None);
	virtual void PlayMontageToServer_Implementation(UAnimMontage* MontageToPlay, const FARPG_MontagePlayConfig& Config, float InPlayRate = 1.f, FName StartSectionName = NAME_None);
	bool PlayMontageToServer_Validate(UAnimMontage* MontageToPlay, const FARPG_MontagePlayConfig& Config, float InPlayRate = 1.f, FName StartSectionName = NAME_None) { return true; }
public:
	UFUNCTION(BlueprintCallable, Category = "角色|动作", Reliable, WithValidation, NetMulticast)
	void StopMontage(UAnimMontage* MontageToStop = nullptr);
	virtual void StopMontage_Implementation(UAnimMontage* MontageToStop = nullptr);
	bool StopMontage_Validate(UAnimMontage* MontageToStop = nullptr) { return true; }

	UFUNCTION(BlueprintCallable, Category = "角色|动作")
	void TryPlayMontage(const FARPG_MontageParameter& Montage, const FARPG_MontagePlayConfig& Config);

	void PlayMontageWithBlendingOutDelegate(UAnimMontage* Montage, const FOnMontageBlendingOutStarted& OnMontageBlendingOutStarted, const FARPG_MontagePlayConfig& Config = FARPG_MontagePlayConfig(), float InPlayRate = 1.f, FName StartSectionName = NAME_None, bool ClientMaster = false);
	void AddMontageWithBlendingOutDelegate(UAnimMontage* Montage, const FOnMontageBlendingOutStarted& OnMontageBlendingOutStarted);
	void ClearMontageBlendingOutDelegate(UAnimMontage* Montage);

	UPROPERTY(BlueprintReadOnly, Category = "角色|动作", Replicated)
	uint8 bMirrorFullBodyMontage : 1;
	uint8 MirrorFullBodyMontageCounter : 2;
	void SetMirrorFullBodyMontage(bool IsMirror);

	UPROPERTY(EditDefaultsOnly, Category = "角色|动画配置")
	FName FullBodySlotName = TEXT("DefaultSlot");

	UFUNCTION(BlueprintCallable, Category = "角色|动作")
	bool CanPlayFullBodyMontage() const;

	UFUNCTION(BlueprintCallable, Category = "角色|动作")
	void EnableAutoUpdateLookAtRotation(bool Enable);
	UFUNCTION(BlueprintCallable, Category = "角色|动作")
	void SetLookAtRotation(const FRotator& LookAtRotation);
	UFUNCTION(BlueprintCallable, Category = "角色|动作")
	void SetLookAtLocation(const FVector& WorldLocation);
	UFUNCTION(BlueprintCallable, Category = "角色|动作")
	FRotator GetLookAtRotation() const;

	//闪避
	UPROPERTY(EditDefaultsOnly, Category = "角色|动画配置")
	class UARPG_DodgeAnimSetBase* DodgeAnimSet;

	UFUNCTION(BlueprintCallable, Category = "角色|动作")
	void InvokeDodge();

	void DodgeByControlRotation(float Direction);

	UFUNCTION(BlueprintCallable, Category = "角色|动作")
	void InvokeDodgeByDirection(EDodgeDirection Direction);

	UFUNCTION(BlueprintCallable, Category = "角色|动作")
	bool CanDodge() const;

	// TODO:RootMotion的距离比例,（现在还无效，需要改MovementComponent）
	UPROPERTY(Replicated)
	FVector RootMotionScale = FVector::OneVector;
	UFUNCTION(BlueprintCallable, Category = "角色|动作")
	void SetRootMotionTranslationScale(const FVector& Scale);
	UFUNCTION(BlueprintCallable, Category = "角色|动作")
	FVector GetRootMotionTranslationScale() const;

	//动画配置
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "角色|动作")
	uint8 bEnableAimOffset : 1;
	//动画瞄准偏移覆盖
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "角色|动作", AdvancedDisplay)
	class UAimOffsetBlendSpace* AimOffsetOverride;

	//考虑用动画曲线替代
	UPROPERTY(BlueprintReadOnly, Category = "角色|动画配置")
	uint8 bEnableFootIk : 1;

	UPROPERTY(EditDefaultsOnly, Category = "角色|动画配置")
	UARPG_ReceiveDamageActionBase* ReceiveDamageActionConfig;
	UPROPERTY()
	UARPG_ReceiveDamageActionBase* ReceiveDamageActionConfigOverride;
	UARPG_ReceiveDamageActionBase* GetReceiveDamageAction() const { return ReceiveDamageActionConfigOverride ? ReceiveDamageActionConfigOverride : ReceiveDamageActionConfig; }
	DECLARE_MULTICAST_DELEGATE(FOnDamageInterrupt);
	//当伤害中断角色动作时
	FOnDamageInterrupt OnDamageInterrupt;

	//背包相关
public:
	UFUNCTION(BlueprintCallable, Category = "角色|物品", Reliable, WithValidation, Server)
	void InvokeMoveItem(UARPG_InventoryComponent* SourceInventory, UARPG_InventoryComponent* TargetInventory, UARPG_ItemCoreBase* ItemCore, int32 Number = 1);

	UFUNCTION(BlueprintCallable, Category = "角色|物品", Reliable, WithValidation, Server)
	void InvokeBuyItem(UARPG_InventoryComponent* BuyFromInventory, UARPG_ItemCoreBase* ItemCore, int32 Number = 1);

	UFUNCTION(BlueprintCallable, Category = "角色|物品", Reliable, WithValidation, Server)
	void InvokeSellItem(UARPG_InventoryComponent* SellToInventory, UARPG_ItemCoreBase* ItemCore, int32 Number = 1);

	UFUNCTION(BlueprintCallable, Category = "角色|物品", Reliable, WithValidation, Server)
	void InvokeThrowItem(UARPG_ItemCoreBase* ItemCore, int32 Number = 1);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "背包")
	UARPG_InventoryComponent* Inventory;

	UFUNCTION(Reliable, WithValidation, Client)
	void OpenMoveItemPanel(UARPG_InventoryComponent* OtherInventory);
	UFUNCTION(Reliable, WithValidation, Client)
	void CloseMoveItemPanel();
	DECLARE_MULTICAST_DELEGATE(FOnCloseMoveItemPanelNative);
	FOnCloseMoveItemPanelNative OnCloseMoveItemPanelNative;
	UFUNCTION(BlueprintCallable, Category = "角色|物品", Reliable, WithValidation, Server)
	void WhenCloseMoveItemPanel();

	UFUNCTION(Reliable, WithValidation, Client)
	void OpenTradeItemPanel(UARPG_InventoryComponent* OtherInventory);
	UFUNCTION(Reliable, WithValidation, Client)
	void CloseTradeItemPanel();
	UFUNCTION(BlueprintCallable, Category = "角色|物品", Reliable, WithValidation, Server)
	void WhenCloseTardeItemPanel();
	DECLARE_MULTICAST_DELEGATE(FOnCloseTradeItemPanelNative);
	FOnCloseTradeItemPanelNative OnCloseTradeItemPanelNative;

	UFUNCTION()
	void WhenAddItem(UXD_ItemCoreBase* ItemCore, int32 AddNumber, int32 ExistNumber);
	UFUNCTION()
	void WhenRemoveItem(UXD_ItemCoreBase* ItemCore, int32 RemoveNumber, int32 ExistNumber);
	//使用道具相关
public:
	UFUNCTION(BlueprintCallable, Category = "角色|物品")
	void InvokeUseItem(const UARPG_ItemCoreBase* ItemCore, EUseItemInput UseItemInput = EUseItemInput::LeftMouse);
	UFUNCTION(Reliable, WithValidation, Server)
	void InvokeUseItem_Server(const UARPG_ItemCoreBase* ItemCore, EUseItemInput UseItemInput);
	void InvokeUseItem_Server_Implementation(const UARPG_ItemCoreBase* ItemCore, EUseItemInput UseItemInput);
	bool InvokeUseItem_Server_Validate(const UARPG_ItemCoreBase* ItemCore, EUseItemInput UseItemInput) { return true; }

	void UseItemImmediately(const UARPG_ItemCoreBase* ItemCore, EUseItemInput UseItemInput = EUseItemInput::LeftMouse);

	UFUNCTION(BlueprintNativeEvent, Category = "角色|物品")
	AARPG_WeaponBase* EquipWaepon(const UARPG_WeaponCoreBase* WeaponCore, EUseItemInput UseItemInput);
	virtual AARPG_WeaponBase* EquipWaepon_Implementation(class UARPG_WeaponCoreBase* WeaponCore, EUseItemInput UseItemInput) { return nullptr; }

	UFUNCTION(BlueprintNativeEvent, Category = "角色|物品")
	AARPG_ArrowBase* EquipArrow(const UARPG_ArrowCoreBase* ArrowCore, EUseItemInput UseItemInput);
	virtual AARPG_ArrowBase* EquipArrow_Implementation(class UARPG_ArrowCoreBase* ArrowCore, EUseItemInput UseItemInput) { return nullptr; }

	UFUNCTION(BlueprintNativeEvent, Category = "角色|物品")
	AARPG_EquipmentBase* EquipEquipment(const UARPG_EquipmentCoreBase* EquipmentCore, EUseItemInput UseItemInput);
	virtual AARPG_EquipmentBase* EquipEquipment_Implementation(class UARPG_EquipmentCoreBase* EquipmentCore, EUseItemInput UseItemInput) { return nullptr; }

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnUseItem, ACharacterBase*, Character, const UARPG_ItemCoreBase*, ItemCore, EUseItemInput, UseItemInput);
	UPROPERTY(BlueprintAssignable)
	FOnUseItem OnUseItem;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEquip, ACharacterBase*, Character, AARPG_ItemBase*, EquipItem);
	UPROPERTY(BlueprintAssignable, Category = "角色|物品")
	FOnEquip OnEquip;
	virtual void WhenEquip(AARPG_ItemBase* EquipItem);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnNotEquip, ACharacterBase*, Character, AARPG_ItemBase*, NotEquipItem);
	UPROPERTY(BlueprintAssignable, Category = "角色|物品")
	FOnNotEquip OnNotEquip;
	virtual void WhenNotEquip(AARPG_ItemBase* NotEquipItem);

	//使用物品时显示的物品
	UPROPERTY(BlueprintReadOnly, Category = "角色|物品")
	AARPG_ItemBase* PendingUseItem;
	//战斗相关
public:
	//卡刀效果
	UFUNCTION(BlueprintCallable, Category = "角色|行为")
	void NearAttackSuccessTimeDilation(float Duration);
	static constexpr float AttackSuccessTimeDilationRate = 0.01f;
	FTimerHandle AttackSuccessTimeDilation_TimeHandle;
	void RecoverAttackSuccessTimeDilation();

	//闪避
	UPROPERTY(BlueprintReadOnly, Category = "角色|行为")
	uint8 bIsDodging : 1;

	//闪避
	virtual void WhenDodgeSucceed(float BaseDamage, ACharacterBase* InstigatedBy, const FHitResult& HitResult);
	UFUNCTION(BlueprintImplementableEvent, Category = "角色|行为")
	void ReceiveWhenDodgeSucceed(float BaseDamage, ACharacterBase* InstigatedBy, const FHitResult& HitResult);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnDodgeSucceed, ACharacterBase*, Character, float, BaseDamage, ACharacterBase*, InstigatedBy, const FHitResult&, HitResult);
	UPROPERTY(BlueprintAssignable, Category = "角色|事件")
	FOnDodgeSucceed OnDodgeSucceed;

	//防御
	UPROPERTY(BlueprintReadWrite, Category = "角色|行为")
	uint8 bIsDefense : 1;
	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "角色|行为")
	bool IsDefenseSucceed(const FVector& DamageFromLocation, const FHitResult& HitInfo) const;
	virtual bool IsDefenseSucceed_Implementation(const FVector& DamageFromLocation, const FHitResult& HitInfo) const;

	void WhenDefenseSucceed(float BaseDamage, ACharacterBase* InstigatedBy, const FVector& HitFromDirection, float DefenseBeakBackDistance, const FHitResult& HitResult);
	UFUNCTION(BlueprintImplementableEvent, BlueprintAuthorityOnly, Category = "角色|行为")
	void ReceiveWhenDefenseSucceed(float BaseDamage, ACharacterBase* InstigatedBy, const FVector& HitFromDirection, float DefenseBeakBackDistance, const FHitResult& HitResult);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnDefenseSucceed, ACharacterBase*, Character, float, BaseDamage, ACharacterBase*, InstigatedBy, const FHitResult&, HitResult);
	UPROPERTY(BlueprintAssignable, Category = "角色|事件")
	FOnDefenseSucceed OnDefenseSucceed;

	void WhenAttackedDefenseCharacter(float BaseDamage, ACharacterBase* DefenseSucceedCharacter, const FHitResult& HitResult);
	UFUNCTION(BlueprintImplementableEvent, BlueprintAuthorityOnly, Category = "角色|行为")
	void ReceiveWhenAttackedDefenseCharacter(float BaseDamage, ACharacterBase* DefenseSucceedCharacter, const FHitResult& HitResult);

	//防御反击
	UPROPERTY(BlueprintReadWrite, Category = "角色|行为")
	uint8 bIsDefenseSwipe : 1;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "角色|行为")
	bool IsDefenseSwipeSucceed(const FVector& DamageFromLocation, const FHitResult& HitInfo) const;
	virtual bool IsDefenseSwipeSucceed_Implementation(const FVector& DamageFromLocation, const FHitResult& HitInfo) const;

	UPROPERTY()
	TMap<class UAnimNotifyState*, class USocketMoveTracer*> SocketMoveTracerMap;

	//处决
public:
	//处决受击方
	UPROPERTY(BlueprintReadWrite, Category = "角色|行为")
	ACharacterBase* ExecuteTargetCharacter;
	//处决攻击方
	UPROPERTY(BlueprintReadWrite, Category = "角色|行为")
	ACharacterBase* ExecuteFromCharacter;

	//背刺
	bool CanBeBackstab(ACharacterBase* BackstabInvoker) const;

	void ExecuteOther(ACharacterBase* ExecuteTarget, const FVector& TargetLocation, const FRotator& TargetRotation, UAnimMontage* ExecuteMontage, UAnimMontage* BeExecutedMontage);
	UFUNCTION(Reliable, WithValidation, Server)
	void ExecuteOtherToServer(ACharacterBase* ExecuteTarget, const FVector& TargetLocation, const FRotator& TargetRotation, UAnimMontage* ExecuteMontage, UAnimMontage* BeExecutedMontage);
	void ExecuteOtherToServer_Implementation(ACharacterBase* ExecuteTarget, const FVector& TargetLocation, const FRotator& TargetRotation, UAnimMontage* ExecuteMontage, UAnimMontage* BeExecutedMontage);
	bool ExecuteOtherToServer_Validate(ACharacterBase* ExecuteTarget, const FVector& TargetLocation, const FRotator& TargetRotation, UAnimMontage* ExecuteMontage, UAnimMontage* BeExecutedMontage) { return true; }

	void WhenExecuteEnd();

	//通知
public:
	virtual void WhenKillOther(ACharacterBase* WhoBeKilled, UObject* KillInstigator);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnKillOther, ACharacterBase*, Killer, ACharacterBase*, WhoBeKilled, UObject*, KillInstigator);
	UPROPERTY(BlueprintAssignable, Category = "角色|事件")
	FOnKillOther OnKillOther;
	UFUNCTION(BlueprintImplementableEvent, BlueprintAuthorityOnly, meta = (DisplayName = "WhenDamagedOther"), Category = "角色|行为")
	void ReceiveWhenKillOther(ACharacterBase* WhoBeKilled, UObject* KillInstigator);

	virtual void WhenDamagedOther(ACharacterBase* WhoBeDamaged, float DamageValue, UObject* DamageInstigator);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnDamagedOther, ACharacterBase*, Attacker, ACharacterBase*, WhoBeDamaged, float, DamageValue, UObject*, DamageInstigator);
	UPROPERTY(BlueprintAssignable, Category = "角色|事件")
	FOnDamagedOther OnDamagedOther;
	UFUNCTION(BlueprintImplementableEvent, BlueprintAuthorityOnly, meta = (DisplayName = "WhenDamagedOther"), Category = "角色|行为")
	void ReceiveWhenDamagedOther(ACharacterBase* WhoBeDamaged, float DamageValue, UObject* DamageInstigator);

	float ApplyPointDamage(float BaseDamage, const FVector& HitFromDirection, const FHitResult& HitInfo, const FARPG_PropertyChangeContext& ChangeContext, TSubclassOf<class UDamageType> DamageTypeClass, const FApplyPointDamageParameter& Param);

	//潜行相关
public:
	UFUNCTION(BlueprintCallable, Category = "角色|状态")
	virtual bool IsSneaking() const;

	//AI战斗相关
public:
	TArray<FARPG_AttackInfo> AttackInfos;

	void AddAttackInfo(const FARPG_AttackInfo& AttackInfo) { AttackInfos.Add(AttackInfo); }

	void RemoveAttackInfo(const FARPG_AttackInfo& AttackInfo) { AttackInfos.RemoveSingle(AttackInfo); }

	//交互相关
public:
	UFUNCTION(BlueprintCallable, Category = "角色|交互")
	void InvokeInteract(AActor* InteractTarget);
	UFUNCTION(Reliable, WithValidation, Server)
	void InvokeInteract_ToServer(AActor* InteractTarget);

	void InvokeInteractWithEndEvent(AActor* InteractTarget, const FOnInteractEnd& OnInteractEndEvent);

	UFUNCTION(BlueprintCallable, Category = "角色|交互")
	bool CanInteractWithTarget(AActor* InteractTarget) const;

	UFUNCTION(BlueprintCallable, Category = "角色|交互")
	void InvokeAbortInteract();

	void InvokeAbortInteractWithAbortEvent(const FOnInteractAbortEnd& OnInteractAbortEndEvent);

	UFUNCTION(Reliable, WithValidation, Server)
	void InvokeAbortInteract_ToServer();

	UFUNCTION(Reliable, WithValidation, Server)
	void InvokeFinishPathFollowing_ToServer();

	UPROPERTY(BlueprintReadOnly, Category = "角色|交互")
	AActor* InvokeInteractTarget;
	UPROPERTY(BlueprintReadOnly, Category = "角色|交互", ReplicatedUsing = OnRep_InteractingTarget)
	AActor* InteractingTarget;
	UFUNCTION()
	void OnRep_InteractingTarget(AActor* PreInteractTarget);
	void SetInteractingTarget(AActor* InInteractingTarget);

	uint8 bIsInBTreeDispatching : 1;

	//交互前的处理，可能由任务之类的附加的操作，会打断之后的交互行为
	DECLARE_DELEGATE(FOnPreInteractEvent);
	TArray<FOnPreInteractEvent> OnPreInteractEvents;

	void WhenExecuteInteract_Implementation(ACharacterBase* InteractInvoker) override;
	bool CanInteract_Implementation(const ACharacterBase* InteractInvoker) const override;

	UPROPERTY(EditAnywhere, Category = "角色|交互", Instanced)
	UARPG_AD_CharacterInteract* InteractBehavior;

	UFUNCTION(BlueprintCallable, Category = "角色|交互")
	void ExecuteInteractEnd(EInteractEndResult Result);
	UFUNCTION(BlueprintCallable, Category = "角色|交互")
	void ExecuteInteractAbortEnd();
private:
#if WITH_EDITOR
	TWeakObjectPtr<AActor> PreInvokeInteractTarget;
#endif
	FOnInteractEnd OnInteractEnd;
	FOnInteractAbortEnd OnInteractAbortEnd;
public:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Character")
	UARPG_MovementComponent* ARPG_MovementComponent;

	//关系相关
public:
	//阵营关系

	UPROPERTY(EditAnywhere, Category = "角色|配置|阵营")
	FARPG_CampConfig CampConfig;

	UFUNCTION(BlueprintCallable, Category = "角色|阵营")
	class UARPG_CampInfo* GetCampInfo() const;

	UFUNCTION(BlueprintCallable, Category = "角色|阵营")
	class UARPG_CampRelationship* GetCampRelationshipToward(ACharacterBase* Other) const;

	ETeamAttitude::Type GetRelationshipTowards(const AActor* Actor) const;

	//总关系
	UFUNCTION(BlueprintCallable, Category = "角色|关系")
	ECharacterRelationship GetRelationshipTowards(const ACharacterBase* Towards) const;

	//AI
public:
	//感知

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "角色|AI|配置")
	TEnumAsByte<ECollisionChannel> SightCollisionChannel = FARPG_ECollisionChannel::AI_Sight;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "角色|AI|配置")
	FName HeadSocketName = TEXT("HeadSightSocket");

	//使用头部骨骼的方向作为视线方向
	virtual void GetActorEyesViewPoint(FVector& out_Location, FRotator& out_Rotation) const override;
	//AISightTargetInterface Start
	virtual bool CanBeSeenFrom(const FVector& ObserverLocation, FVector& OutSeenLocation, int32& NumberOfLoSChecksPerformed, float& OutSightStrength, const AActor* IgnoreActor = NULL) const override;
	//AISightTargetInterface End

	//子系统

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "角色|AI|配置", Instanced)
	class UARPG_HatredControlSystemBase* HatredControlSystem;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "角色|AI|配置", Instanced)
	class UARPG_BattleStyleSystemBase* BattleStyleSystem;

	UPROPERTY(BlueprintReadOnly, Category = "角色|AI")
	TScriptInterface<IARPG_AI_BattleInterface> BattleControl;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "角色|AI|配置", Instanced)
	class UARPG_AlertSystemBase* AlertSystem;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "角色|AI|配置", Instanced)
	class UARPG_SneakSystemBase* SneakSystem;

	//意识

	//警戒值
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "角色|AI")
	float AlertValue;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "角色|AI|配置")
	float AlertEntirelyValue = 100.f;
	UFUNCTION(BlueprintCallable, Category = "角色|AI")
	void AddAlertValue(float AddValue);
	UFUNCTION(BlueprintCallable, Category = "角色|AI")
	void ReduceAlertValue(float ReduceValue);

	UPROPERTY(EditDefaultsOnly, Category = "角色|AI|配置")
	float AlertSubsidedSpeed = 5.f;
	FTimerHandle AlertSubsided_TimerHandle;
	void AlertSubsided();

	UPROPERTY(EditDefaultsOnly, Category = "角色|AI|配置")
	float AlertNotEntirelySubsidedDelay = 5.f;
	UPROPERTY(EditDefaultsOnly, Category = "角色|AI|配置")
	float AlertEntirelySubsidedDelay = 10.f;

	UFUNCTION(BlueprintCallable, Category = "角色|AI")
	EAlertState GetAlertState() const;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "角色|AI|配置")
	ECharacterRelationship MaxAlertRelationship = ECharacterRelationship::Hostile;

	//思考逻辑

	UPROPERTY(EditAnywhere, Category = "角色|AI|配置")
	FBehaviorTreeWithSubTree MainBehaviorTree;

	bool CanInDailyBehavior() const;

	//NavLink
	UFUNCTION(BlueprintNativeEvent, Category = "角色|AI|寻路")
	void WhenStartNavLink(EARPG_NavAreaFlag NavAreaFlag, const FVector& StartLocation, const FVector& EndLocation);
	virtual void WhenStartNavLink_Implementation(EARPG_NavAreaFlag NavAreaFlag, const FVector& StartLocation, const FVector& EndLocation);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "角色|AI|配置")
	TSubclassOf<class UNavigationQueryFilter> NavigationQueryFilter;

	//接收到寻路请求时的处理
	void WhenReceivedMoveRequest();

	//感知

	//获得当前看到的角色
	UFUNCTION(BlueprintCallable, BlueprintPure = false , Category = "角色|AI|感知")
	TArray<ACharacterBase*> GetSeeingCharacters() const;

	//AI在看不见玩家后并不会立即丢失玩家（考虑玩家通过拐角），AI视觉追踪最大时长
	UPROPERTY(EditAnywhere, Category = "角色|AI|配置")
	float SightMaxTraceTime = 5.f;
	//获得还在AI视觉追踪时间内的角色
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "角色|AI|感知")
	TArray<ACharacterBase*> GetSeeingTraceCharacters() const;
	//获得还在AI视觉追踪时间内的敌人
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "角色|AI|感知")
	TArray<ACharacterBase*> GetSeeingTraceEnemies() const;

	//CharacterBehavior接口
public:
	//进入松懈状态
	UPROPERTY(EditAnywhere, Category = "角色|动画配置", Instanced)
	UCA_EnterReleaseStateBase* EnterReleaseStateAction;

	virtual bool IsInReleaseState() const { return true; }

	UARPG_CharacterBehaviorBase* EnterReleaseState(const FOnCharacterBehaviorFinished& OnBehaviorFinished);

	//转身
	UPROPERTY(EditDefaultsOnly, Category = "角色|动画配置")
	UCA_TurnConfigBase* TurnConfig;
	UPROPERTY()
	UCA_CharacterTurnBase* CharacterTurnAction;

	UPROPERTY(EditDefaultsOnly, Category = "角色|动作")
	FName TurnSlotName = TEXT("TurnInPlace");

	bool CanTurnTo() const;
	void TurnTo(const FRotator& TargetWorldRotation, const FOnCharacterBehaviorFinished& OnCharacterBehaviorFinished, bool ForceSnapRotation = true);
	UFUNCTION(BlueprintCallable, Category = "角色|动作")
	void TurnTo(const FRotator& TargetWorldRotation, bool ForceSnapRotation = true);

	// 说话
public:
	UPROPERTY(EditDefaultsOnly, Category = "角色|说话")
	FName MouthSocketName = TEXT("Mouth");

	UPROPERTY(EditDefaultsOnly, Category = "角色|说话")
	USoundAttenuation* SpeakDefaultAttenuationSettings;

	UPROPERTY(Transient, BlueprintReadOnly, BlueprintGetter = GetMouthComponent)
	UAudioComponent* MouthComponentRuntime;

	UFUNCTION(BlueprintGetter)
	UAudioComponent* GetMouthComponent();

	void Speak(USoundBase* Sentence, const FOnCharacterBehaviorFinished& OnSpeakFinished);
	void StopSpeak();

	UFUNCTION(NetMulticast, Unreliable, WithValidation)
	void SpeakToAllClient(USoundBase* Sentence);

	UFUNCTION(NetMulticast, Unreliable, WithValidation)
	void StopSpeakToAllClient();

	FOnCharacterBehaviorFinished OnSpeakFinishedDelegate;

	//寻路相关接口
public:
	UPROPERTY(Transient)
	UPathFollowingComponent* PathFollowingComponent;
	UPathFollowingComponent* GetPathFollowingComponent();

	void PauseMove();
	void ResumeMove();
public:
	void ForceSetClientWorldLocation(const FVector& Location);
	void ForceSetClientWorldLocationAndRotation(const FVector& Location, const FRotator& Rotation);
private:
	UFUNCTION(Reliable, WithValidation, Client)
	void ForceSetClientWorldLocationImpl(const FVector& Location);

	UFUNCTION(Reliable, WithValidation, Client)
	void ForceSetClientWorldLocationAndRotationImpl(const FVector& Location, const FRotator& Rotation);
public:
	UFUNCTION(Reliable, WithValidation, Client)
	void ForceSetClientWorldRotation(const FRotator& Rotation);

public:
	// 持久性的Tag，用于记录角色的特殊状态
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "角色|Tags", SaveGame)
	FGameplayTagContainer StateTags;

	UFUNCTION(BlueprintCallable, Category = "角色|Tags")
	void AddStateTag(FGameplayTag Tag);
	UFUNCTION(BlueprintCallable, Category = "角色|Tags")
	void RemoveStateTag(FGameplayTag Tag);
	UFUNCTION(BlueprintCallable, Category = "角色|Tags")
	bool HasStateTag(FGameplayTag Tag) const;
private:
	bool AD_HasStateTag_Implementation(const FGameplayTag& Tag) const override;
	void AD_AddStateTag_Implementation(const FGameplayTag& Tag) override;
};
