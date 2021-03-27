#include "../../idlib/precompiled.h"
#pragma hdrstop

#include "../Game_local.h"
#include "../Weapon.h"

#define BLASTER_SPARM_CHARGEGLOW		6

class rvWeaponBlaster : public rvWeapon {
public:

	CLASS_PROTOTYPE( rvWeaponBlaster );

	rvWeaponBlaster ( void );

	virtual void		Spawn				( void );
	void				Save				( idSaveGame *savefile ) const;
	void				Restore				( idRestoreGame *savefile );
	void				PreSave		( void );
	void				PostSave	( void );

protected:

	bool				UpdateAttack		( void );
	bool				UpdateFlashlight	( void );
	void				Flashlight			( bool on );

private:

	
	int					fireHeldTime;

	stateResult_t		State_Raise				( const stateParms_t& parms );
	stateResult_t		State_Lower				( const stateParms_t& parms );
	stateResult_t		State_Idle				( const stateParms_t& parms );
	stateResult_t		State_Fire				( const stateParms_t& parms );
	stateResult_t		State_Flashlight		( const stateParms_t& parms );
	
	CLASS_STATES_PROTOTYPE ( rvWeaponBlaster );
};

CLASS_DECLARATION( rvWeapon, rvWeaponBlaster )
END_CLASS

/*
================
rvWeaponBlaster::rvWeaponBlaster
================
*/
rvWeaponBlaster::rvWeaponBlaster ( void ) {
}

/*
================
rvWeaponBlaster::UpdateFlashlight
================
*/
bool rvWeaponBlaster::UpdateFlashlight ( void ) {
	if ( !wsfl.flashlight ) {
		return false;
	}
	
	SetState ( "Flashlight", 0 );
	return true;		
}

/*
================
rvWeaponBlaster::Flashlight
================
*/
void rvWeaponBlaster::Flashlight ( bool on ) {
	owner->Flashlight ( on );
	
	if ( on ) {
		worldModel->ShowSurface ( "models/weapons/blaster/flare" );
		viewModel->ShowSurface ( "models/weapons/blaster/flare" );
	} else {
		worldModel->HideSurface ( "models/weapons/blaster/flare" );
		viewModel->HideSurface ( "models/weapons/blaster/flare" );
	}
}

/*
================
rvWeaponBlaster::UpdateAttack
================
*/
bool rvWeaponBlaster::UpdateAttack ( void ) {

	// If the player is pressing the fire button and they have enough ammo for a shot
	// then start the shooting process.
	if ( wsfl.attack && gameLocal.time >= nextAttackTime ) {
		// Save the time which the fire button was pressed
		if ( fireHeldTime == 0 ) {		
			nextAttackTime = gameLocal.time + (fireRate * owner->PowerUpModifier ( PMOD_FIRERATE ));
			fireHeldTime   = gameLocal.time;
			//viewModel->SetShaderParm ( BLASTER_SPARM_CHARGEGLOW, chargeGlow[0] );
		}
	}		

	// If they have the charge mod and they have overcome the initial charge 
	// delay then transition to the charge state.
		// If the fire button was let go but was pressed at one point then 
		// release the shot.
		if ( !wsfl.attack ) {
			idPlayer * player = gameLocal.GetLocalPlayer();
			if( player )	{
			
				if( player->GuiActive())	{
					//make sure the player isn't looking at a gui first
					SetState ( "Lower", 0 );
				} else {
					SetState ( "Fire", 0 );
				}
			}
			return true;
		}
	
	return false;
}

/*
================
rvWeaponBlaster::Spawn
================
*/
void rvWeaponBlaster::Spawn ( void ) {
	
	SetState ( "Raise", 0 );

	fireHeldTime		= 0;
	
			
	Flashlight ( owner->IsFlashlightOn() );
}

/*
================
rvWeaponBlaster::Save
================
*/
void rvWeaponBlaster::Save ( idSaveGame *savefile ) const {
	
	savefile->WriteInt ( fireHeldTime );
}

/*
================
rvWeaponBlaster::Restore
================
*/
void rvWeaponBlaster::Restore ( idRestoreGame *savefile ) {
	
	savefile->ReadInt ( fireHeldTime );
}

/*
================
rvWeaponBlaster::PreSave
================
*/
void rvWeaponBlaster::PreSave ( void ) {

	SetState ( "Idle", 4 );

	StopSound( SND_CHANNEL_WEAPON, 0);
	StopSound( SND_CHANNEL_BODY, 0);
	StopSound( SND_CHANNEL_ITEM, 0);
	StopSound( SND_CHANNEL_ANY, false );
	
}

/*
================
rvWeaponBlaster::PostSave
================
*/
void rvWeaponBlaster::PostSave ( void ) {
}

/*
===============================================================================

	States 

===============================================================================
*/

CLASS_STATES_DECLARATION ( rvWeaponBlaster )
	STATE ( "Raise",						rvWeaponBlaster::State_Raise )
	STATE ( "Lower",						rvWeaponBlaster::State_Lower )
	STATE ( "Idle",							rvWeaponBlaster::State_Idle)
	STATE ( "Fire",							rvWeaponBlaster::State_Fire )
	STATE ( "Flashlight",					rvWeaponBlaster::State_Flashlight )
END_CLASS_STATES

/*
================
rvWeaponBlaster::State_Raise
================
*/
stateResult_t rvWeaponBlaster::State_Raise( const stateParms_t& parms ) {
	enum {
		RAISE_INIT,
		RAISE_WAIT,
	};	
	switch ( parms.stage ) {
		case RAISE_INIT:			
			SetStatus ( WP_RISING );
			PlayAnim( ANIMCHANNEL_ALL, "raise", parms.blendFrames );
			return SRESULT_STAGE(RAISE_WAIT);
			
		case RAISE_WAIT:
			if ( AnimDone ( ANIMCHANNEL_ALL, 4 ) ) {
				SetState ( "Idle", 4 );
				return SRESULT_DONE;
			}
			if ( wsfl.lowerWeapon ) {
				SetState ( "Lower", 4 );
				return SRESULT_DONE;
			}
			return SRESULT_WAIT;
	}
	return SRESULT_ERROR;	
}

/*
================
rvWeaponBlaster::State_Lower
================
*/
stateResult_t rvWeaponBlaster::State_Lower ( const stateParms_t& parms ) {
	enum {
		LOWER_INIT,
		LOWER_WAIT,
		LOWER_WAITRAISE
	};	
	switch ( parms.stage ) {
		case LOWER_INIT:
			SetStatus ( WP_LOWERING );
			PlayAnim( ANIMCHANNEL_ALL, "putaway", parms.blendFrames );
			return SRESULT_STAGE(LOWER_WAIT);
			
		case LOWER_WAIT:
			if ( AnimDone ( ANIMCHANNEL_ALL, 0 ) ) {
				SetStatus ( WP_HOLSTERED );
				return SRESULT_STAGE(LOWER_WAITRAISE);
			}
			return SRESULT_WAIT;
	
		case LOWER_WAITRAISE:
			if ( wsfl.raiseWeapon ) {
				SetState ( "Raise", 0 );
				return SRESULT_DONE;
			}
			return SRESULT_WAIT;
	}
	return SRESULT_ERROR;
}

/*
================
rvWeaponBlaster::State_Idle
================
*/
stateResult_t rvWeaponBlaster::State_Idle ( const stateParms_t& parms ) {	
	enum {
		IDLE_INIT,
		IDLE_WAIT,
	};	
	switch ( parms.stage ) {
		case IDLE_INIT:			
			SetStatus ( WP_READY );
			PlayCycle( ANIMCHANNEL_ALL, "idle", parms.blendFrames );
			return SRESULT_STAGE ( IDLE_WAIT );
			
		case IDLE_WAIT:
			if ( wsfl.lowerWeapon ) {
				SetState ( "Lower", 4 );
				return SRESULT_DONE;
			}
			
			if ( UpdateFlashlight ( ) ) { 
				return SRESULT_DONE;
			}
			if ( UpdateAttack ( ) ) {
				return SRESULT_DONE;
			}
			return SRESULT_WAIT;
	}
	return SRESULT_ERROR;
}

/*
================
rvWeaponBlaster::State_Fire
================
*/
stateResult_t rvWeaponBlaster::State_Fire ( const stateParms_t& parms ) {
	enum {
		FIRE_INIT,
		FIRE_WAIT,
	};	
	switch ( parms.stage ) {
		case FIRE_INIT:	

			StopSound ( SND_CHANNEL_ITEM, false );
		//	viewModel->SetShaderParm ( BLASTER_SPARM_CHARGEGLOW, 0 );
			//don't fire if we're targeting a gui.
			idPlayer* player;
			player = gameLocal.GetLocalPlayer();

			//make sure the player isn't looking at a gui first
			if( player && player->GuiActive() )	{
				fireHeldTime = 0;
				SetState ( "Lower", 0 );
				return SRESULT_DONE;
			}

			if( player && !player->CanFire() )	{
				fireHeldTime = 0;
				SetState ( "Idle", 4 );
				return SRESULT_DONE;
			}


	
			/*if ( gameLocal.time - fireHeldTime > chargeTime ) {	
				Attack ( true, 10, 2, 0, 1.2f );
				PlayEffect ( "fx_chargedflash", barrelJointView, false );
				PlayAnim( ANIMCHANNEL_ALL, "chargedfire", parms.blendFrames );
			} else {*/
				Attack ( false, 10, 5, 0, 0.8f );
				PlayEffect ( "fx_normalflash", barrelJointView, false );
				PlayAnim( ANIMCHANNEL_ALL, "fire", parms.blendFrames );
			//}
			fireHeldTime = 0;
			
			return SRESULT_STAGE(FIRE_WAIT);
		
		case FIRE_WAIT:
			if ( AnimDone ( ANIMCHANNEL_ALL, 4 ) ) {
				SetState ( "Idle", 4 );
				return SRESULT_DONE;
			}
			if ( UpdateFlashlight ( ) || UpdateAttack ( ) ) {
				return SRESULT_DONE;
			}
			return SRESULT_WAIT;
	}			
	return SRESULT_ERROR;
}

/*
================
rvWeaponBlaster::State_Flashlight
================
*/
stateResult_t rvWeaponBlaster::State_Flashlight ( const stateParms_t& parms ) {
	enum {
		FLASHLIGHT_INIT,
		FLASHLIGHT_WAIT,
	};	
	switch ( parms.stage ) {
		case FLASHLIGHT_INIT:			
			SetStatus ( WP_FLASHLIGHT );
			// Wait for the flashlight anim to play		
			PlayAnim( ANIMCHANNEL_ALL, "flashlight", 0 );
			return SRESULT_STAGE ( FLASHLIGHT_WAIT );
			
		case FLASHLIGHT_WAIT:
			if ( !AnimDone ( ANIMCHANNEL_ALL, 4 ) ) {
				return SRESULT_WAIT;
			}
			
			if ( owner->IsFlashlightOn() ) {
				Flashlight ( false );
			} else {
				Flashlight ( true );
			}
			
			SetState ( "Idle", 4 );
			return SRESULT_DONE;
	}
	return SRESULT_ERROR;
}
