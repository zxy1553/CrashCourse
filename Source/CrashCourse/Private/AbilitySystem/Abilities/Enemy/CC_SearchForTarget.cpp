// Copyright eeeee


#include "AbilitySystem/Abilities/Enemy/CC_SearchForTarget.h"

#include <ThirdParty/hlslcc/hlslcc/src/hlslcc_lib/ir_hierarchical_visitor.h>

#include "AbilitySystemComponent.h"
#include "Abilities/Async/AbilityAsync.h"
#include "Abilities/Async/AbilityAsync_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "AbilitySystem/AbilityTasks/CC_WaitGameplayEvent.h"
#include "Chareacters/CC_EnemyCharacter.h"
#include "GameplayTags/CCTags.h"
#include "Runtime/AIModule/Classes/AIController.h"
#include "Runtime/AIModule/Classes/Tasks/AITask_MoveTo.h"
#include "Utils/CC_BlueprintLibrary.h"

UCC_SearchForTarget::UCC_SearchForTarget()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
}

void UCC_SearchForTarget::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	OwningEnemy = Cast<ACC_EnemyCharacter>(GetAvatarActorFromActorInfo());
	check(OwningEnemy.IsValid()); 
	OwingAIController = Cast<AAIController>(OwningEnemy->GetController());
	check(OwingAIController.IsValid()); 
	
	StartSearch();
	
	WaitGameplayEventTask = UCC_WaitGameplayEvent::WaitGameplayEventToActorProxy(GetAvatarActorFromActorInfo(),CCTags::Events::Enemy::EndAttack);
	WaitGameplayEventTask->EventReceived.AddDynamic(this, &ThisClass::EndAttackEventReceived);
	WaitGameplayEventTask->StartActivation();
	
}

void UCC_SearchForTarget::StartSearch()
{
	if (bDrawDebugs)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, FString::Printf(TEXT("UCC_SearchForTarget::StartSearch")));
	}

	if (!OwningEnemy.IsValid()) return;
	
	const float SearchDelay = FMath::RandRange(OwningEnemy->MinAttackDelay, OwningEnemy->MaxAttackDelay);
	SearchDelayTask = UAbilityTask_WaitDelay::WaitDelay(this, SearchDelay);
	SearchDelayTask->OnFinish.AddDynamic(this, &ThisClass::Search);
	SearchDelayTask->Activate();	
}

void UCC_SearchForTarget::EndAttackEventReceived(FGameplayEventData Payload)
{
	if (OwningEnemy.IsValid() && !OwningEnemy->bIsBeginLaunched)
	{
		StartSearch();
	}
}

void UCC_SearchForTarget::Search()
{
	const FVector SearchOrigin = GetAvatarActorFromActorInfo()->GetActorLocation();
	if (!OwningEnemy.IsValid()) return;
	
	FClosestActorWhitTagResult ClosestActorResult = UCC_BlueprintLibrary::FindClosestActorWhitTag(GetAvatarActorFromActorInfo(),SearchOrigin, CrashTags::Player, OwningEnemy->SearchRange); 
	
	TargetBaseCharacter = Cast<ACC_BaseCharacter>(ClosestActorResult.Actor);
	
	if (!TargetBaseCharacter.IsValid())
	{
		StartSearch();
		return;
	}
	if (TargetBaseCharacter->IsAlive())
	{
		MoveToTargetAndAttack();	
	}
	else
	{
		StartSearch();
	}
}

void UCC_SearchForTarget::MoveToTargetAndAttack()
{
	if (!OwningEnemy.IsValid() || !OwingAIController.IsValid() || !TargetBaseCharacter.IsValid()) return;
	if (!OwningEnemy->IsAlive())
	{
		StartSearch();
		return;
	}
	MoveToLocationOrActorTask = UAITask_MoveTo::AIMoveTo(OwingAIController.Get(), FVector(), TargetBaseCharacter.Get(),
	                                                     OwningEnemy->AcceptanceRadius); 
	MoveToLocationOrActorTask->OnMoveTaskFinished.AddUObject(this,&ThisClass::AttackTarget);
	MoveToLocationOrActorTask->ConditionalPerformMove();
}

void UCC_SearchForTarget::AttackTarget(TEnumAsByte<EPathFollowingResult::Type> Result, AAIController* AIController)
{
	if (Result != EPathFollowingResult::Success)
	{
		StartSearch();
		return;
	}
	OwningEnemy->RotateToTarget(TargetBaseCharacter.Get());
	AttackDelayTask = UAbilityTask_WaitDelay::WaitDelay(this, OwningEnemy->GetTimelineLength()); 
	AttackDelayTask->OnFinish.AddDynamic(this,&ThisClass::Attack);
	AttackDelayTask->Activate();
	
}

void UCC_SearchForTarget::Attack()
{ 
	const FGameplayTag AttackTag = CCTags::CCAbilities::Enemy::Attack;
	GetAbilitySystemComponentFromActorInfo()->TryActivateAbilitiesByTag(AttackTag.GetSingleTagContainer());
	
}
