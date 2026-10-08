// Copyright eeeee

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "Components/WidgetComponent.h"
#include "CC_WidgetComponent.generated.h"


struct FGameplayAttribute;
class UAbilitySystemComponent;
class UCC_AttributeSet;
class UCC_AbilitySystemComponent;
class ACC_BaseCharacter;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CRASHCOURSE_API UCC_WidgetComponent : public UWidgetComponent
{
	GENERATED_BODY()

public:

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere)
	TMap<FGameplayAttribute, FGameplayAttribute> AttributeMap;

private:
	TWeakObjectPtr<ACC_BaseCharacter> CrashCharacter;
	TWeakObjectPtr<UCC_AbilitySystemComponent> AbilitySystemComponent;
	TWeakObjectPtr<UCC_AttributeSet> AttributeSet;
	
	void InitAbilitySystemData();
	bool IsASCInitialized() const;
	void InitializeAttributeDelegate();
	
	UFUNCTION()
	void OnASCInitialized(UAbilitySystemComponent* ASC, UAttributeSet* AS);
	
	UFUNCTION()
	void BindToAttributeChanges();
	void BindWidgetToAttributeChanges(UWidget* WidgetObject, const TTuple<FGameplayAttribute, FGameplayAttribute>& Pair) const;
	
};
