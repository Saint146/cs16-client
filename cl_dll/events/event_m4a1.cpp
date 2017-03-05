// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
/*
*
*    This program is free software; you can redistribute it and/or modify it
*    under the terms of the GNU General Public License as published by the
*    Free Software Foundation; either version 2 of the License, or (at
*    your option) any later version.
*
*    This program is distributed in the hope that it will be useful, but
*    WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*    General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with this program; if not, write to the Free Software Foundation,
*    Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
*    In addition, as a special exception, the author gives permission to
*    link the code of this program with the Half-Life Game Engine ("HL
*    Engine") and Modified Game Libraries ("MODs") developed by Valve,
*    L.L.C ("Valve").  You must obey the GNU General Public License in all
*    respects for all of the code used other than the HL Engine and MODs
*    from Valve.  If you modify this file, you may extend this exception
*    to your version of the file, but you are not obligated to do so.  If
*    you do not wish to do so, delete this exception statement from your
*    version.
*
*/
#include "events.h"
#include "wpn_shared.h"


static const char *SOUNDS_NAME[] =
{
	"weapons/m4a1-1.wav",
	"weapons/m4a1_unsil-1.wav",
	"weapons/m4a1_unsil-2.wav"
};

// bparam1: 1 if silenced, 0 if unsilenced
void EV_FireM4A1( event_args_t *args )
{
	Vector ShellVelocity;
	Vector ShellOrigin;
	Vector vecSrc, vecAiming;
	int    sequence, idx = args->entindex;
	Vector origin( args->origin );
	Vector angles(
		args->iparam1 / 100.0f + args->angles[0],
		args->iparam2 / 100.0f + args->angles[1],
		args->angles[2]
		);
	Vector velocity( args->velocity );
	Vector forward, right, up;
	int smokeType;

	AngleVectors( angles, forward, right, up );

	if ( EV_IsLocal( idx ) )
	{
		++g_iShotsFired;
		EV_MuzzleFlash();
		if( args->bparam1 )
		{
			sequence = Com_RandomLong( M4A1_SHOOT1, M4A1_SHOOT3 );
			smokeType = SMOKE_BLACK;
		}
		else
		{
			sequence = Com_RandomLong( M4A1_UNSIL_SHOOT1, M4A1_UNSIL_SHOOT3 );
			smokeType = SMOKE_RIFLE;
		}
		gEngfuncs.pEventAPI->EV_WeaponAnimation(sequence, 2);
		if( !gHUD.cl_righthand->value )
		{
			EV_GetDefaultShellInfo( args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 20.0, -8.0, -10.0, 0);
		}
		else
		{
			EV_GetDefaultShellInfo( args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 20.0, -8.0, 10.0, 0);
		}

		if( gHUD.cl_gunsmoke->value )
		{
			cl_entity_t *ent = gEngfuncs.GetViewModel();

			if( ent )
			{
				EV_CS16Client_CreateSmoke( smokeType, ent->attachment[0], forward, 3, 0.3, 20, 20, 20, false, velocity );
			}
		}
	}
	else
	{
		EV_GetDefaultShellInfo( args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 20.0, -12.0, 4.0, 0);
	}


	EV_EjectBrass(ShellOrigin, ShellVelocity, angles[ YAW ], g_iRShell, TE_BOUNCE_SHELL);

	PLAY_EVENT_SOUND( args->bparam1 ? SOUNDS_NAME[0] : SOUNDS_NAME[Com_RandomLong( 1, 2 )]);

	EV_GetGunPosition( args, vecSrc, origin );
	VectorCopy( forward, vecAiming );
	Vector vSpread;
	
	vSpread.x = args->fparam1;
	vSpread.y = args->fparam2;
	EV_HLDM_FireBullets( idx,
		forward, right,	up,
		1, vecSrc, vecAiming,
		vSpread, 8192.0, BULLET_PLAYER_556MM,
		2 );
}
