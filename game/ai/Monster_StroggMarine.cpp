
#include "../../idlib/precompiled.h"
#pragma hdrstop

#include "../Game_local.h"


//NOTE: actually a bit of a misnomer, as all Strogg Marine types use this class now...
class rvMonsterStroggMarine : public idAI {
public:

	CLASS_PROTOTYPE(rvMonsterStroggMarine);

	rvMonsterStroggMarine(void);

	void				Spawn(void);
	void				Save(idSaveGame *savefile) const;
	void				Restore(idRestoreGame *savefile);

	virtual void		AdjustHealthByDamage(int damage);

protected:

	rvAIAction			actionMeleeMoveAttack;
	//rvAIAction			actionChaingunAttack;

	virtual bool		CheckActions(void);

	virtual void		OnTacticalChange(aiTactical_t oldTactical);
	virtual void		OnDeath(void);

private:

	int					standingMeleeNoAttackTime;
	//int					rageThreshold;

	//void				RageStart(void);
	//void				RageStop(void);

	// Torso States
	//stateResult_t		State_Torso_Enrage(const stateParms_t& parms);
	stateResult_t		State_Torso_Pain(const stateParms_t& parms);
	//stateResult_t		State_Torso_LeapAttack(const stateParms_t& parms);

	CLASS_STATES_PROTOTYPE(rvMonsterStroggMarine);
};

CLASS_DECLARATION(idAI, rvMonsterStroggMarine)
END_CLASS

/*
================
rvMonsterStroggMarine::rvMonsterStroggMarine
================
*/
rvMonsterStroggMarine::rvMonsterStroggMarine(void) {
	standingMeleeNoAttackTime = 0;
}

/*void rvMonsterStroggMarine::InitSpawnArgsVariables( void )
{
maxShots = spawnArgs.GetInt ( "maxShots", "1" );
minShots = spawnArgs.GetInt ( "minShots", "1" );
attackRate = SEC2MS( spawnArgs.GetFloat( "attackRate", "0.2" ) );
attackJoint = animator.GetJointHandle( spawnArgs.GetString( "attackJoint", "muzzle" ) );
}*/
/*
================
rvMonsterStroggMarine::Spawn
================
*/
void rvMonsterStroggMarine::Spawn(void) {
	//rageThreshold = spawnArgs.GetInt("health_rageThreshold");

	// Custom actions
	actionMeleeMoveAttack.Init(spawnArgs, "action_meleeMoveAttack", NULL, AIACTIONF_ATTACK);
	//actionChaingunAttack.Init(spawnArgs, "action_chaingunAttack", NULL, AIACTIONF_ATTACK);
	//actionLeapAttack.Init(spawnArgs, "action_leapAttack", "Torso_LeapAttack", AIACTIONF_ATTACK);
	combat.tacticalCurrent = AITACTICAL_MELEE;
	// Enraged to start?
	/*if (spawnArgs.GetBool("preinject")) {
	RageStart();
	}*/
}

/*
================
rvMonsterStroggMarine::Save
================
*/
void rvMonsterStroggMarine::Save(idSaveGame *savefile) const {
	actionMeleeMoveAttack.Save(savefile);
	//actionChaingunAttack.Save(savefile);

	//savefile->WriteInt(rageThreshold);
	savefile->WriteInt(standingMeleeNoAttackTime);
}

/*
================
rvMonsterStroggMarine::Restore
================
*/
void rvMonsterStroggMarine::Restore(idRestoreGame *savefile) {
	actionMeleeMoveAttack.Restore(savefile);
	//actionChaingunAttack.Restore(savefile);

	//savefile->ReadInt(rageThreshold);
	savefile->ReadInt(standingMeleeNoAttackTime);
}

/*
============
rvMonsterStroggMarine::OnStopMoving
============
*/
bool rvMonsterStroggMarine::CheckActions(void) {
	// If our health is below the rage threshold then enrage
	/*if (health < rageThreshold) {
	PerformAction("Torso_Enrage", 4, true);
	return true;
	}*/

	// Moving melee attack?
	if (PerformAction(&actionMeleeMoveAttack, (checkAction_t)&idAI::CheckAction_MeleeAttack, NULL)) {
		return true;
	}

	// Default actions
	if (CheckPainActions()) {
		return true;
	}

	/*if (PerformAction(&actionEvadeLeft, (checkAction_t)&idAI::CheckAction_EvadeLeft, &actionTimerEvade) ||
	PerformAction(&actionEvadeRight, (checkAction_t)&idAI::CheckAction_EvadeRight, &actionTimerEvade) ||
	PerformAction(&actionJumpBack, (checkAction_t)&idAI::CheckAction_JumpBack, &actionTimerEvade) ||
	PerformAction(&actionLeapAttack, (checkAction_t)&idAI::CheckAction_LeapAttack)) {
	return true;
	}
	else*/ if (PerformAction(&actionMeleeAttack, (checkAction_t)&idAI::CheckAction_MeleeAttack)) {
		standingMeleeNoAttackTime = 0;
		return true;
	}
	else {
		if (actionMeleeAttack.status != rvAIAction::STATUS_FAIL_TIMER
			&& actionMeleeAttack.status != rvAIAction::STATUS_FAIL_EXTERNALTIMER
			&& actionMeleeAttack.status != rvAIAction::STATUS_FAIL_CHANCE)
		{//melee attack fail for any reason other than timer?
			if (combat.tacticalCurrent == AITACTICAL_MELEE && !move.fl.moving)
			{//special case: we're in tactical melee and we're close enough to think we've reached the enemy, but he's just out of melee range!
				if (!standingMeleeNoAttackTime)
				{
					standingMeleeNoAttackTime = gameLocal.GetTime();
				}
				//else if (standingMeleeNoAttackTime + 2500 < gameLocal.GetTime())
				//{//we've been standing still and not attacking for at least 2.5 seconds, fall back to ranged attack
				//allow ranged attack
				//actionRangedAttack.fl.disabled = false;
				//}
			}
		}
		/*if (PerformAction(&actionRangedAttack, (checkAction_t)&idAI::CheckAction_RangedAttack, &actionTimerRangedAttack)) {
		return true;
		}*/
	}
	return false;
}

/*
================
rvMonsterStroggMarine::OnDeath
================
*/
void rvMonsterStroggMarine::OnDeath(void) {
	//RageStop();
	return idAI::OnDeath();
}

/*
================
rvMonsterStroggMarine::OnTacticalChange

Enable/Disable the ranged attack based on whether the grunt needs it
================
*/
void rvMonsterStroggMarine::OnTacticalChange(aiTactical_t oldTactical) {
	switch (combat.tacticalCurrent) {
	case AITACTICAL_MELEE:
		actionRangedAttack.fl.disabled = true;
		break;

	default:
		actionRangedAttack.fl.disabled = true;
		break;
	}
}

/*
=====================
rvMonsterStroggMarine::AdjustHealthByDamage
=====================
*/
void rvMonsterStroggMarine::AdjustHealthByDamage(int damage) {
	// Take less damage during enrage process 
	/*if (rageThreshold && health < rageThreshold) {
	health -= (damage * 0.25f);
	return;
	}*/
	return idAI::AdjustHealthByDamage(damage);
}

/*
===============================================================================

States

===============================================================================
*/

CLASS_STATES_DECLARATION(rvMonsterStroggMarine)
//STATE("Torso_Enrage", rvMonsterStroggMarine::State_Torso_Enrage)
STATE("Torso_Pain", rvMonsterStroggMarine::State_Torso_Pain)
//STATE("Torso_LeapAttack", rvMonsterStroggMarine::State_Torso_LeapAttack)
END_CLASS_STATES

/*
================
rvMonsterStroggMarine::State_Torso_Pain
================
*/
stateResult_t rvMonsterStroggMarine::State_Torso_Pain(const stateParms_t& parms) {
	// Stop streaming pain if its time to get angry
	/*if (pain.loopEndTime && health < rageThreshold) {
	pain.loopEndTime = 0;
	}*/
	return idAI::State_Torso_Pain(parms);
}

/*
================
rvMonsterStroggMarine::State_Torso_Enrage
================
*/
/*stateResult_t rvMonsterStroggMarine::State_Torso_Enrage(const stateParms_t& parms) {
enum {
STAGE_ANIM,
STAGE_ANIM_WAIT,
};
switch (parms.stage) {
case STAGE_ANIM:
DisableAnimState(ANIMCHANNEL_LEGS);
PlayAnim(ANIMCHANNEL_TORSO, "anger", parms.blendFrames);
return SRESULT_STAGE(STAGE_ANIM_WAIT);

case STAGE_ANIM_WAIT:
if (AnimDone(ANIMCHANNEL_TORSO, 4)) {
RageStart();
return SRESULT_DONE;
}
return SRESULT_WAIT;
}
return SRESULT_ERROR;
}*/


/*
================
rvMonsterStroggMarine::State_Torso_LeapAttack
================
*/