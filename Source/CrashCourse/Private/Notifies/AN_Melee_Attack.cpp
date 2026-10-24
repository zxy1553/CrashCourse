// Copyright eeeee


#include "Notifies/AN_Melee_Attack.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "AbilitySystemComponent.h"
#include "Chareacters/CC_BaseCharacter.h"
#include "GameplayTags/CCTags.h"

void UAN_Melee_Attack::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime,
                                  const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
	
	if (!IsValid(MeshComp)) return;
	if (!IsValid(MeshComp->GetOwner())) return;
	
	TArray<FHitResult> Hits = PerformSphereTrace(MeshComp);
	SendEventsToActors(&Hits,MeshComp);	
	
}

TArray<FHitResult> UAN_Melee_Attack::PerformSphereTrace(USkeletalMeshComponent* MeshComp)
{
	FTransform Transform = MeshComp->GetSocketTransform(SocketName);		
	FVector Start = Transform.GetLocation();
	FVector ExtendedSocketDirection = Transform.GetRotation().GetForwardVector() * SocketExtensionOffset;
	FVector End = Start - ExtendedSocketDirection; 
	TArray<FHitResult> Hits;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(MeshComp->GetOwner());
	UKismetSystemLibrary::SphereTraceMulti(MeshComp,Start,End,Radius,UEngineTypes::ConvertToTraceType(ECC_Visibility), false, ActorsToIgnore, EDrawDebugTrace::None,Hits,true);
	
	return Hits;
}

void UAN_Melee_Attack::SendEventsToActors(TArray<FHitResult>* Hits, USkeletalMeshComponent* MeshComponent)
{
	for (const FHitResult& Hit : *Hits)
	{
		AActor* HitActor = Hit.GetActor();
		
		UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(MeshComponent->GetOwner());
		if (!IsValid(ASC)) return;
		
		
		FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
		EffectContext.AddHitResult(Hit);
		ACC_BaseCharacter* PlayerCharacter =  Cast<ACC_BaseCharacter>(HitActor);
		if(!IsValid(PlayerCharacter) || !PlayerCharacter->IsAlive())
		{
			continue;
		}
		FGameplayTag MeleeTraceAttack = CCTags::Events::Enemy::MeleeTraceHit;
		FGameplayEventData Payload;
		Payload.Instigator = MeshComponent->GetOwner();
		Payload.Target = PlayerCharacter;
		Payload.ContextHandle = EffectContext;
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(MeshComponent->GetOwner(),MeleeTraceAttack,Payload);
		
		
	}
}
