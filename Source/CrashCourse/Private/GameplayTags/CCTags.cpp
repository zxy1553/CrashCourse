#include "GameplayTags/CCTags.h"

namespace CCTags
{
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(None, "CCTags.None", "Tag for None");
	namespace SetByCaller
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Projectile, "CCTags.SetByCaller.Projectile", "Tag for Caller Magnitude for Projectile");
	}
	namespace CCAbilities
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Primary, "CCTags.CCAbilities.Primary", "Tag for the Primary Ability");	
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Secondary, "CCTags.CCAbilities.Secondary", "Tag for the Secondary Ability");	
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Tertiary, "CCTags.CCAbilities.Tertiary", "Tag for the Tertiary Ability");	
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(ActivateOnGiven, "CCTags.CCAbilities.ActivateOnGiven", "Tag for Activate Ability Event");	
		
		namespace Enemy
		{
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attack, "CCTags.CCAbilities.Enemy.Attack", "Tag for Enemy Attack Event");
		}
	}


	namespace Events
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(KillScored, "CCTags.Events.KillScored", "Tag for KillScore Event");
		namespace Player
		{
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(HitReact, "CCTags.Events.Player.HitReact", "Tag for Player Hit React");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Death, "CCTags.Events.Player.Death", "Tag for Player Death");
		}
		namespace Enemy
		{
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(HitReact, "CCTags.Events.Enemy.HitReact", "Tag for Enemy HitReact Event");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(EndAttack, "CCTags.Events.Enemy.EndAttack", "Tag for Enemy Ending an Attack");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(MeleeTraceHit, "CCTags.Events.Enemy.MeleeTraceHit", "Tag for Melee Trace Hit");
		}
	}
}
