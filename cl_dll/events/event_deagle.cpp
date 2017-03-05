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
	"weapons/deagle-1.wav",
	"weapons/deagle-2.wav"
};

void EV_FireDEAGLE( event_args_t *args )
{
	Vector ShellVelocity;
	Vector ShellOrigin;
	vec3_t vecSrc;

	int idx = args->entindex;
	Vector origin( args->origin );
	Vector angles(
		args->iparam1 / 100.0f + args->angles[0],
		args->iparam2 / 100.0f + args->angles[1],
		args->angles[2]
	);
	Vector velocity( args->velocity );
	Vector forward, right, up;
	AngleVectors( angles, forward, right, up );

	if ( EV_IsLocal( idx ) )
	{
		++g_iShotsFired;
		EV_MuzzleFlash();
		if( args->bparam1 )
		{
			gEngfuncs.pEventAPI->EV_WeaponAnimation( Com_RandomLong(DEAGLE_SHOOT1, DEAGLE_SHOOT2), 2 );
		}
		else
		{
			gEngfuncs.pEventAPI->EV_WeaponAnimation( DEAGLE_SHOOT_EMPTY, 2 );
		}
		if( !gHUD.cl_righthand->value )
		{
			EV_GetDefaultShellInfo( args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 35.0, -11.0, -16.0, 0);
		}
		else
		{
			EV_GetDefaultShellInfo( args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 35.0, -11.0, 16.0, 0);
		}

		if( gHUD.cl_gunsmoke->value )
		{
			cl_entity_t *ent = gEngfuncs.GetViewModel();

			if( ent )
			{
				Vector smoke_origin = ent->attachment[0];

				smoke_origin = smoke_origin - forward * 3;

				float scale = Com_RandomFloat( 0.2, 0.3 );

				EV_CS16Client_CreateSmoke( SMOKE_PISTOL, smoke_origin, forward, 0,  scale, 7,7,7, false, velocity );
				EV_CS16Client_CreateSmoke( SMOKE_PISTOL, smoke_origin, forward, 20, scale + 0.1, 10,10,10, false, velocity );
				EV_CS16Client_CreateSmoke( SMOKE_PISTOL, smoke_origin, forward, 40, scale, 13,13,13, false, velocity );
			}
		}
	}
	else
	{
		EV_GetDefaultShellInfo( args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 20.0, -12.0, 4.0, 0);
	}

	EV_EjectBrass(ShellOrigin, ShellVelocity, angles[ YAW ], g_iPShell, TE_BOUNCE_SHELL);

	PLAY_EVENT_SOUND( SOUNDS_NAME[Com_RandomLong(0, 1)] );

	EV_GetGunPosition( args, vecSrc, origin );
	Vector vSpread( args->fparam1, args->fparam2, 0.0f );
	
	EV_HLDM_FireBullets( idx,
		forward, right,	up,
		1, vecSrc, forward,
		vSpread, 8192.0, BULLET_PLAYER_50AE,
		2 );
}
