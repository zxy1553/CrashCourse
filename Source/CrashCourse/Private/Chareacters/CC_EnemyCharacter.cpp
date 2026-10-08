// Copyright eeeee


#include "Chareacters/CC_EnemyCharacter.h"
#include "AbilitySystem/CC_AbilitySystemComponent.h"
#include "AbilitySystem/CC_AttributeSet.h"


// Sets default values
ACC_EnemyCharacter::ACC_EnemyCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
	
	AbilitySystemComponent = CreateDefaultSubobject<UCC_AbilitySystemComponent>("CC_AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	
	AttributeSet = CreateDefaultSubobject<UCC_AttributeSet>("AttributeSet");
}

UAbilitySystemComponent* ACC_EnemyCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UAttributeSet* ACC_EnemyCharacter::GetAttributeSet() const
{
	return AttributeSet;
}

void ACC_EnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (!IsValid(GetAbilitySystemComponent())) return;
	
	GetAbilitySystemComponent()->InitAbilityActorInfo(this,this);
	OnASCInitialized.Broadcast(GetAbilitySystemComponent(),GetAttributeSet());
	
	if (!HasAuthority()) return;
	
	GiveStartupAbilities();
	InitializeAttributes();
}
