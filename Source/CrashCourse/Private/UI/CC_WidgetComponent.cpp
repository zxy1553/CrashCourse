// Copyright eeeee


#include "UI/CC_WidgetComponent.h"

#include "AbilitySystem/CC_AbilitySystemComponent.h"
#include "AbilitySystem/CC_AttributeSet.h"
#include "Blueprint/WidgetTree.h"
#include "Chareacters/CC_BaseCharacter.h"
#include "UI/CC_AttributeWidget.h"


void UCC_WidgetComponent::BeginPlay()
{
	Super::BeginPlay();
	InitAbilitySystemData();
	if (!CrashCharacter.IsValid()) return;

	if (!IsASCInitialized())
	{
		CrashCharacter->OnASCInitialized.AddDynamic(this, &ThisClass::OnASCInitialized);
		return;
	}

	InitializeAttributeDelegate();
}

void UCC_WidgetComponent::InitAbilitySystemData()
{
	CrashCharacter = Cast<ACC_BaseCharacter>(GetOwner());
	if (!CrashCharacter.IsValid()) return;

	AttributeSet = Cast<UCC_AttributeSet>(CrashCharacter->GetAttributeSet());
	AbilitySystemComponent = Cast<UCC_AbilitySystemComponent>(CrashCharacter->GetAbilitySystemComponent());
}

bool UCC_WidgetComponent::IsASCInitialized() const
{
	return AbilitySystemComponent.IsValid() && AttributeSet.IsValid();
}

void UCC_WidgetComponent::InitializeAttributeDelegate()
{
	if (!AttributeSet->bAttributesInitialized)
	{
		AttributeSet->OnAttributesInitialized.AddDynamic(this, &ThisClass::BindToAttributeChanges);		 
	}
	else
	{
		BindToAttributeChanges();
	}
}

void UCC_WidgetComponent::OnASCInitialized(UAbilitySystemComponent* ASC, UAttributeSet* AS)
{
	AttributeSet = Cast<UCC_AttributeSet>(AS);
	AbilitySystemComponent = Cast<UCC_AbilitySystemComponent>(ASC);
	
	if (!IsASCInitialized()) return;
	
	InitializeAttributeDelegate();
	
}

void UCC_WidgetComponent::BindToAttributeChanges()
{
	for ( const TTuple<FGameplayAttribute,FGameplayAttribute>& Pair: AttributeMap)
	{
		BindWidgetToAttributeChanges(GetUserWidgetObject(), Pair);
		
		GetUserWidgetObject()->WidgetTree->ForEachWidget([this, &Pair](UWidget* ChildWidget)
		{
			BindWidgetToAttributeChanges(ChildWidget, Pair);	
		});	
		
	}	
}

void UCC_WidgetComponent::BindWidgetToAttributeChanges(UWidget* WidgetObject, const TTuple<FGameplayAttribute, FGameplayAttribute>& Pair) const
{
	UCC_AttributeWidget* AttributeWidget = Cast<UCC_AttributeWidget>(WidgetObject);
	if (!IsValid(AttributeWidget)) return; // we only care about CC Attribute Widget
	if (!AttributeWidget->MatchesAttributes(Pair)) return; // Only subscribe for matching Attributes
		
	AttributeWidget->OnAttributeChange(Pair, AttributeSet.Get());  // for initial values
		
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Pair.Key).AddLambda([this, AttributeWidget, &Pair](const FOnAttributeChangeData& AttributeChangeData)
	{
		AttributeWidget->OnAttributeChange(Pair, AttributeSet.Get());	// for change during game
	});
	
}
