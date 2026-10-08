// Copyright eeeee


#include "CrashCourse/Public/Chareacters/CC_BaseCharacter.h"

#include "CrashCourse/Public/AbilitySystem/CC_AbilitySystemComponent.h"


ACC_BaseCharacter::ACC_BaseCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
	
	// Tick and refresh bones transforms whether rendered or not - for bone updates on a dedicated server (tigger or hit)
	GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
	
}

UAbilitySystemComponent* ACC_BaseCharacter::GetAbilitySystemComponent() const
{
	return nullptr;
}

void ACC_BaseCharacter::GiveStartupAbilities()
{
	if (!IsValid(GetAbilitySystemComponent()))  return;
	for (const auto& Ability: StartupAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(Ability);
		GetAbilitySystemComponent()->GiveAbility(AbilitySpec);
	}
	
}

void ACC_BaseCharacter::InitializeAttributes() const
{
	checkf(IsValid(InitializeAttributesEffect), TEXT("InitializeAttributesEffect not set."));
	
	FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
	FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(InitializeAttributesEffect, 1.f, ContextHandle);
	if (!SpecHandle.IsValid()) return;
	GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
}


