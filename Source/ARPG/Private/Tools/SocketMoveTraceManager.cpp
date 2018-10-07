﻿// Fill out your copyright notice in the Description page of Project Settings.

#include "SocketMoveTraceManager.h"




void USocketMoveTracer::Tick(float DeltaTime)
{
	DoTrace(DeltaTime);
}

UWorld* USocketMoveTracer::GetTickableGameObjectWorld() const
{
	return TargetSocketMesh ? TargetSocketMesh->GetWorld() : nullptr;
}

AActor* USocketMoveTracer::GetOwner() const
{
	return TargetSocketMesh ? TargetSocketMesh->GetOwner() : nullptr;
}

void USocketMoveTracer::InitSocketMoveTracer(UPrimitiveComponent* TargetComponent)
{
	if (TargetComponent)
	{
		TargetSocketMesh = TargetComponent;
		TracedActors.Append({ GetOwner(), GetOwner()->GetOwner() });
		if (TraceSocketList.Num() < 2)
		{
			TraceSocketList = TargetSocketMesh->GetAllSocketNames();
		}
		PrePosList.SetNum(TraceSocketList.Num());
	}
}

void USocketMoveTracer::StartTraceSetting()
{
	TracedActors.RemoveAt(2, TracedActors.Num() - 2);
	RecordPrePosition();
}

void USocketMoveTracer::EndTraceSetting()
{

}

void USocketMoveTracer::DoTrace(float DeltaTime)
{
	TArray<AActor*> TracedInOnceTraceActor;
	for (int i = 1; i < TraceSocketList.Num(); ++i)
	{
		float StepAddAlpha = StepLength / (PrePosList[i - 1] - PrePosList[i]).Size();

		FVector TraceStart_Start = PrePosList[i - 1];
		FVector TraceStart_End = PrePosList[i];
		FVector TraceEnd_Start = TargetSocketMesh->GetSocketLocation(TraceSocketList[i - 1]);
		FVector TraceEnd_End = TargetSocketMesh->GetSocketLocation(TraceSocketList[i]);

		for (float Alpha = 0; Alpha < 1.f; Alpha += StepAddAlpha)
		{
			FVector TraceStart = FMath::Lerp(TraceStart_Start, TraceStart_End, Alpha);
			FVector TraceEnd = FMath::Lerp(TraceEnd_Start, TraceEnd_End, Alpha);

			TArray<FHitResult> Hits;

			if (UKismetSystemLibrary::SphereTraceMulti(TargetSocketMesh, TraceStart, TraceEnd, StepLength / 2.f, TraceTypeQuery, true, TracedActors, GetDebugType(), Hits, true))
			{
				for (const FHitResult& E_Hit : Hits)
				{
					if (E_Hit.bBlockingHit && !TracedInOnceTraceActor.Contains(E_Hit.GetActor()))
					{
						TracedInOnceTraceActor.Add(E_Hit.GetActor());
						TracedActors.Add(E_Hit.GetActor());
						OnTraceActor.ExecuteIfBound(TargetSocketMesh, TraceSocketList[i], E_Hit.GetActor(), E_Hit.GetComponent(), E_Hit);
					}
				}
			}
		}
	}

	//记录位置信息
	RecordPrePosition();
}

void USocketMoveTracer::RecordPrePosition()
{
	for (int i = 0; i < TraceSocketList.Num(); ++i)
	{
		PrePosList[i] = TargetSocketMesh->GetSocketLocation(TraceSocketList[i]);
	}
}

void USocketMoveTracer::SetTraceSocketList(const TArray<FName>& SocketList)
{
	TraceSocketList = SocketList;
	PrePosList.SetNum(TraceSocketList.Num());
}

void USocketMoveTracer::ClearTraceSocketList()
{
	TraceSocketList.Empty();
	PrePosList.Empty();
}

void USocketMoveTracer::SetEnableTrace(bool Enable)
{
	bEnableTrace = Enable;
	if (bEnableTrace)
	{
		StartTraceSetting();
	}
	else
	{
		EndTraceSetting();
	}
}
