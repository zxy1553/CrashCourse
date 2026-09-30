#include "GameplayTags/CCTags.h"

namespace CCTags
{
	namespace CCAbilities
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Primary, "CCTags.CCAbilities.Primary", "Tag for the Primary Ability");	
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Secondary, "CCTags.CCAbilities.Secondary", "Tag for the Secondary Ability");	
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Tertiary, "CCTags.CCAbilities.Tertiary", "Tag for the Tertiary Ability");	
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(ActivateOnGiven, "CCTags.CCAbilities.ActivateOnGiven", "Tag for Activate Ability Event");	
	}

	namespace Event
	{
		namespace Enemy
		{
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(HitReact, "CCTags.Event.Enemy.HitReact", "Tag for Enemy HitReact Event");
		}
	}
}
