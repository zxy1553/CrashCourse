// Copyright eeeee

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AN_Melee_Attack.generated.h"

/**
 * 
 */
UCLASS()
class CRASHCOURSE_API UAN_Melee_Attack : public UAnimNotifyState
{
	GENERATED_BODY()
	
	
	virtual void NotifyTick(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;
	
private:
	
	TArray<FHitResult> PerformSphereTrace(USkeletalMeshComponent * MeshComp);
	
	void SendEventsToActors(TArray<FHitResult>* Hits, USkeletalMeshComponent* MeshComponent);
	
	
	FName SocketName = TEXT("FX_Trail_01_R");
	float SocketExtensionOffset{40.f};
	float Radius{10.f};
	
};
