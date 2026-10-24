// Copyright eeeee


#include "CrashCourse/Public/Chareacters/CC_BaseCharacter.h"

#include "AbilitySystem/CC_AttributeSet.h"
#include "CrashCourse/Public/AbilitySystem/CC_AbilitySystemComponent.h"

namespace CrashTags
{
	const FName Player = FName("Player");	
}

ACC_BaseCharacter::ACC_BaseCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
	
	// Tick and refresh bones transforms whether rendered or not - for bone updates on a dedicated server (tigger or hit)
	GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
	
}

void ACC_BaseCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// UE 5.6 默认自动注册开启时，
	// bAlive 可以不再显式写 DOREPLIFETIME。
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
	
	UCC_AttributeSet* AttributeSet = Cast<UCC_AttributeSet>( GetAttributeSet());
	if (!IsValid(AttributeSet)) return;
	AttributeSet->bAttributesInitialized = true;	
	AttributeSet->OnAttributesInitialized.Broadcast();
}

void ACC_BaseCharacter::OnHealthChanged(const FOnAttributeChangeData& AttributeChangeData)
{
	if (AttributeChangeData.NewValue <= .0f)
	{
		HandleDeath();	
	}
}

void ACC_BaseCharacter::HandleDeath()
{
	bAlive = false;
	

}

void ACC_BaseCharacter::ResetAttributes()
{
	if (!HasAuthority()) return;
	
	checkf(IsValid(ResetAttributesEffect),TEXT("ResetAttributesEffect not set."));
	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	if (!IsValid(ASC)) return;
	
	
	ASC->BP_ApplyGameplayEffectToSelf(ResetAttributesEffect,1.f,ASC->MakeEffectContext());  // BP 但可以在cpp里用，传class，engine完成生成CDO、构造Spec、Apply
	// ASC->ApplyGameplayEffectToSelf(ResetAttributesEffect->GetDefaultObject<UGameplayEffect>(),1.f,ASC->MakeEffectContext());  // 需要传入CDO、engine构造Spec、Apply
	
	// FGameplayEffectContextHandle Context =  ASC->MakeEffectContext();
	// FGameplayEffectSpecHandle Spec = ASC->MakeOutgoingSpec(ResetAttributesEffect,1.f,Context);
	// if (!Spec.IsValid()) return;
	// ASC->ApplyGameplayEffectSpecToSelf(*Spec.Data);  //手动构造Spec，直接Apply Spec
	
	
}

void ACC_BaseCharacter::HandleRespawn()
{
	bAlive = true;
}
