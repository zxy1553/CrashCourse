// Copyright eeeee

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemCOmponent.h"
#include "CC_AbilitySystemComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CRASHCOURSE_API UCC_AbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	UCC_AbilitySystemComponent();
	
	virtual void OnGiveAbility(FGameplayAbilitySpec& AbilitySpec) override;
	virtual void OnRep_ActivateAbilities() override;
	
private:
	void HandleAutoActivatedAbility(const FGameplayAbilitySpec& AbilitySpec);
};
