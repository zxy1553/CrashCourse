// Copyright eeeee


#include "Utils/CC_BlueprintLibrary.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemGlobals.h"
#include "AssetTypeCategories.h"
#include "AbilitySystem/CC_AttributeSet.h"
#include "Chareacters/CC_BaseCharacter.h"
#include "Chareacters/CC_EnemyCharacter.h"
#include "Chareacters/CC_PlayerCharacter.h"
#include "Engine/OverlapResult.h"
#include "GameplayTags/CCTags.h"
#include "Kismet/GameplayStatics.h"

EHitDirection UCC_BlueprintLibrary::GetHitDirection(const FVector& TargetForward, const FVector& ToInstigator)
{
	const float Dot = FVector::DotProduct( TargetForward, ToInstigator);
	if (Dot < -0.5f)
	{
		return EHitDirection::Back;
	}
	else if (Dot < .5f)
	{
		// Either Left or Right
		const FVector Cross = FVector::CrossProduct(TargetForward, ToInstigator);
		if (Cross.Z < .0f)
		{
			return EHitDirection::Left;
		}
		return EHitDirection::Right;
	}
	return EHitDirection::Forward;
}

FName UCC_BlueprintLibrary::GetHitDirectionName(const EHitDirection& HitDirection) 
{
	switch (HitDirection)
	{
		case EHitDirection::Forward: return FName("Forward");
		case EHitDirection::Back: return FName("Back");
		case EHitDirection::Right: return FName("Right");
		case EHitDirection::Left: return FName("Left");
		default: return FName("None");
	}
}

FClosestActorWhitTagResult UCC_BlueprintLibrary::FindClosestActorWhitTag(UObject* WorldContextObject,
	const FVector& Origin, const FName& Tag, float SearchRange)
{
	TArray<AActor*> ActorsWithTag;
	UGameplayStatics::GetAllActorsWithTag(WorldContextObject,Tag,ActorsWithTag);
	
	float ClosestDistance = TNumericLimits<float>::Max();
	AActor* ClosestActor = nullptr;

	for (AActor* Actor: ActorsWithTag)
	{
		if (!IsValid(Actor)) continue;
		ACC_BaseCharacter* BaseCharacter = Cast<ACC_BaseCharacter>(Actor);
		if (!IsValid(BaseCharacter) || !BaseCharacter->IsAlive()) continue;
			
		const float Distance = FVector::Dist(Origin, Actor->GetActorLocation());  
		if (ACC_BaseCharacter* SearchingCharacter = Cast<ACC_BaseCharacter>(WorldContextObject); IsValid(SearchingCharacter))
		{
			if (Distance > SearchingCharacter->SearchRange) continue;
		}	
		if (Distance < ClosestDistance)
		{
			ClosestDistance = Distance;
			ClosestActor = Actor;
		}
	}
	
	FClosestActorWhitTagResult Result;
	Result.Actor=ClosestActor;
	Result.Distance=ClosestDistance;
	
	return Result;
}

void UCC_BlueprintLibrary::SendDamageEventToPlayer(AActor* Target, const TSubclassOf<UGameplayEffect>& DamageEffect,
	 FGameplayEventData& Payload, const FGameplayTag& DataTag, float Damage, const FGameplayTag& EventTagOverride, UObject* OptionalParticleSystem)
{
	ACC_BaseCharacter* PlayerCharacter = Cast<ACC_BaseCharacter>(Target);
	if (!IsValid(PlayerCharacter) || !PlayerCharacter->IsAlive()) return;
	
	FGameplayTag EventTag;
	if (!EventTagOverride.MatchesTagExact(CCTags::None))
	{
		EventTag = EventTagOverride;	
	}
	else
	{
		UCC_AttributeSet* AttributeSet = Cast<UCC_AttributeSet>(PlayerCharacter->GetAttributeSet());
		if (!IsValid(AttributeSet)) return;
	
		const bool bLethal = AttributeSet->GetHealth() - Damage <= 0.f;
		EventTag = bLethal ? CCTags::Events::Player::Death : CCTags::Events::Player::HitReact;	
	}
	
	Payload.OptionalObject = OptionalParticleSystem;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(PlayerCharacter,EventTag,Payload);
	
	UAbilitySystemComponent* TargetASC = PlayerCharacter->GetAbilitySystemComponent();
	if (!IsValid(TargetASC)) return;

	if (!IsValid(DamageEffect))
	{
		UE_LOG(LogTemp, Warning, TEXT("SendDamageEventToPlayer: DamageEffect not set."));
		return;
	}

	UAbilitySystemComponent* SourceASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Payload.Instigator.Get());
	if (!IsValid(SourceASC)) SourceASC = TargetASC;

	FGameplayEffectContextHandle ContextHandle = SourceASC->MakeEffectContext();
	FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffect,1.f,ContextHandle);
	if (!SpecHandle.IsValid()) return;

	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle,DataTag,-Damage);

	SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data, TargetASC);
}

void UCC_BlueprintLibrary::SendDamageEventToPlayers(TArray<AActor*> Targets,
	const TSubclassOf<UGameplayEffect>& DamageEffect, FGameplayEventData& Payload, const FGameplayTag& DataTag,
	float Damage, const FGameplayTag& EventTagOverride, UObject* OptionalParticleSystem)
{
	for (auto Target : Targets)
	{
		SendDamageEventToPlayer(Target,DamageEffect, Payload, DataTag, Damage, EventTagOverride, OptionalParticleSystem);	
	}
}

TArray<AActor*> UCC_BlueprintLibrary::HitBoxOverlapTest(AActor* AvatarActor, float HitBoxRadius, float HitBoxForwardOffset, float HitBoxElevationOffset, bool bDrawDebugs)
{
	if (!IsValid(AvatarActor)) return TArray<AActor*>();
	
	// Ensure that the overlap test ignores the Avatar Actor
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(AvatarActor);
	
	FCollisionResponseParams ResponseParams;
	ResponseParams.CollisionResponse.SetAllChannels(ECR_Ignore);
	ResponseParams.CollisionResponse.SetResponse(ECC_Pawn, ECR_Block);
	
	TArray<FOverlapResult> OverlapResults;
	FCollisionShape Sphere = FCollisionShape::MakeSphere(HitBoxRadius);
	
	const FVector Forward = AvatarActor->GetActorForwardVector() * HitBoxForwardOffset;
	const FVector HitBoxLocation = AvatarActor->GetActorLocation() + Forward + FVector(0.f,0.f, HitBoxElevationOffset);
	
	UWorld* World = GEngine->GetWorldFromContextObject(AvatarActor, EGetWorldErrorMode::LogAndReturnNull);
	if (!IsValid(World)) return TArray<AActor*>();
	
	World->OverlapMultiByChannel(OverlapResults, HitBoxLocation, FQuat::Identity, ECC_Visibility, Sphere, QueryParams, ResponseParams);
	
	TArray<AActor*> ActorsHit;
	for (const FOverlapResult& Result : OverlapResults)
	{
		if (!IsValid(Result.GetActor())) continue;
		
		ActorsHit.AddUnique(Result.GetActor());
	}	
	
	if (bDrawDebugs)
	{
		DrawHitBoxOverlapDebugs(AvatarActor, OverlapResults, HitBoxLocation, HitBoxRadius);
	}
	
	return ActorsHit;
}

void UCC_BlueprintLibrary::DrawHitBoxOverlapDebugs(const UObject* WorldContextObject, const TArray<FOverlapResult>& OverlapResults,
	const FVector& HitBoxLocation, float HitBoxRadius)
{
	
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	DrawDebugSphere(World, HitBoxLocation, HitBoxRadius, 16 ,FColor::Red, false, 3.f);
	
	for (const FOverlapResult&  Result: OverlapResults)
	{
		if (IsValid(Result.GetActor()))
		{
			FVector DebugLocation = Result.GetActor()->GetActorLocation();
			DebugLocation.Z += 100.f;
			DrawDebugSphere(World, DebugLocation, 30.f, 10 ,FColor::Green, false, 3.f);
		}
	}
}

TArray<AActor*> UCC_BlueprintLibrary::ApplyKnockback(AActor* AvatarActor, const TArray<AActor*>& HitActors, float InnerRadius,
	float OuterRadius, float LaunchForceMagnitude, float RotationAngle, bool bDrawDebugs)
{
	if (!IsValid(AvatarActor) || !GEngine) return TArray<AActor*>();

	UWorld* World = GEngine->GetWorldFromContextObject(AvatarActor, EGetWorldErrorMode::LogAndReturnNull);
	const FVector AvatarLocation = AvatarActor->GetActorLocation();

	for (AActor* HitActor : HitActors)
	{
		ACharacter* HitCharacter = Cast<ACharacter>(HitActor);
		if (!IsValid(HitCharacter)) continue;

		const FVector HitCharacterLocation = HitCharacter->GetActorLocation();
		const FVector ToHitActor = HitCharacterLocation - AvatarLocation;
		const float Distance = ToHitActor.Size();

		float LaunchForce = 0.f;
		if (Distance > OuterRadius) continue;
		if (Distance <= InnerRadius)
		{
			LaunchForce = LaunchForceMagnitude;
		}
		else
		{
			const FVector2D FalloffRange(InnerRadius,OuterRadius);  //input range
			const FVector2D LaunchForceRange(LaunchForceMagnitude, 0.f); // output range
			LaunchForce = FMath::GetMappedRangeValueClamped(FalloffRange,LaunchForceRange,Distance);
		}
		if (bDrawDebugs) GEngine->AddOnScreenDebugMessage(-1,3.f,FColor::Red, FString::Printf(TEXT("LaunchForce: %f"), LaunchForce));
		// Horizontal unit direction from the avatar to the hit character
		FVector KnockbackDirection = ToHitActor.GetSafeNormal2D();
		if (KnockbackDirection.IsNearlyZero())
		{
			// Hit character is directly above/below the avatar, fall back to the avatar's forward
			KnockbackDirection = AvatarActor->GetActorForwardVector().GetSafeNormal2D();
		}

		// Pitch the direction upward by RotationAngle around the horizontal right axis
		const FVector Right = KnockbackDirection.RotateAngleAxis(90.f, FVector::UpVector);
		KnockbackDirection = KnockbackDirection.RotateAngleAxis(-RotationAngle, Right);

		const FVector LaunchVelocity = KnockbackDirection * LaunchForce;

		if (bDrawDebugs)
		{
			DrawDebugDirectionalArrow(World, HitCharacterLocation, HitCharacterLocation + KnockbackDirection * 100.f, 100.f, FColor::Green, false, 3.f);
		}

		if (ACC_EnemyCharacter* EnemyCharacter = Cast<ACC_EnemyCharacter>(HitCharacter); IsValid(EnemyCharacter))
		{
			EnemyCharacter->StopMovementUntilLanded();
		}
		HitCharacter->LaunchCharacter(LaunchVelocity, true, true);
	}
	return HitActors;
}
