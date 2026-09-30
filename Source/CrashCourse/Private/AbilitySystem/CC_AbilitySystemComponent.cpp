// Copyright eeeee


#include "AbilitySystem/CC_AbilitySystemComponent.h"

#include "GameplayTags/CCTags.h"
#include "Abilities/GameplayAbility.h"


UCC_AbilitySystemComponent::UCC_AbilitySystemComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UCC_AbilitySystemComponent::OnGiveAbility(FGameplayAbilitySpec& AbilitySpec)
{
	Super::OnGiveAbility(AbilitySpec);
	HandleAutoActivatedAbility(AbilitySpec);
	
}

void UCC_AbilitySystemComponent::OnRep_ActivateAbilities()
{
	Super::OnRep_ActivateAbilities();

	FScopedAbilityListLock ActiveScopeLock(*this);
	for (auto& AbilitySpec : GetActivatableAbilities())
	{
		HandleAutoActivatedAbility(AbilitySpec);
	}
}

void UCC_AbilitySystemComponent::HandleAutoActivatedAbility(const FGameplayAbilitySpec& AbilitySpec)
{
	if (!IsValid(AbilitySpec.Ability)) return;

	for (const FGameplayTag& Tag : AbilitySpec.Ability->GetAssetTags())
	{
		if (Tag.MatchesTagExact(CCTags::CCAbilities::ActivateOnGiven))
		{
			TryActivateAbility(AbilitySpec.Handle);
			return;
		}
	}
}

