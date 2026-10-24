// Copyright eeeee


#include "CC_Primary.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Engine/OverlapResult.h"
#include "GameplayTags/CCTags.h"

void UCC_Primary::SendHitReactEventToActors(const TArray<AActor*>& ActorsHit)
{
	for (AActor* ActorHit : ActorsHit)
	{
		FGameplayEventData PayLoad;
		PayLoad.Instigator = GetAvatarActorFromActorInfo();
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(ActorHit, CCTags::Events::Enemy::HitReact, PayLoad);		
	}	
}

