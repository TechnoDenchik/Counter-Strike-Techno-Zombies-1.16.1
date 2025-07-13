/* =================================================================================== *
			 * =================== TechnoSoftware =================== *
 * =================================================================================== */

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "weapons.h"
#include "wpn_paladin.h"

#ifndef CLIENT_DLL
#include "gamemode/mods.h"
#endif

enum ak47paladin_e
{
	AK47_IDLE1,
	AK47_RELOAD,
	AK47_DRAW,
	AK47_SHOOT1,
	AK47_SHOOT2,
	AK47_SHOOT3,
};

LINK_ENTITY_TO_CLASS(weapon_paladin, CAK47Paladin)

void CAK47Paladin::Spawn(void)
{
	pev->classname = MAKE_STRING("weapon_paladin");

	Precache();
	m_iId = WEAPON_AK47;
	SET_MODEL(ENT(pev), "models/w_buffak.mdl");

	m_iDefaultAmmo = AK47_DEFAULT_GIVE;
	m_flAccuracy = 0.2;
	m_iShotsFired = 0;

	FallInit();
}

void CAK47Paladin::Precache(void)
{
	PRECACHE_MODEL("models/v_buffak.mdl");
	PRECACHE_MODEL("models/w_buffak.mdl");
	PRECACHE_MODEL("sprites/ef_buffak_hit.spr");
	PRECACHE_MODEL("sprites/muzzleflash40.spr");
	PRECACHE_MODEL("sprites/muzzleflash41.spr");
	PRECACHE_SOUND("weapons/ak47buff_draw.wav");
	PRECACHE_SOUND("weapons/ak47buff_idle.wav");
	PRECACHE_SOUND("weapons/ak47buff_reload.wav");
	PRECACHE_SOUND("weapons/ak47buff-1.wav");
	PRECACHE_SOUND("weapons/ak47buff-2.wav");

	m_iShell = PRECACHE_MODEL("models/rshell.mdl");
	m_usFirePaladin = PRECACHE_EVENT(1, "events/ak47paladin.sc");
	m_usFirePaladin2 = PRECACHE_EVENT(2, "events/ak47paladin2.sc");
}

int CAK47Paladin::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "762Nato";
	p->iMaxAmmo1 = MAX_AMMO_762NATO;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->pszAmmo3 = NULL;
	p->iMaxAmmo3 = -1;
	p->pszAmmoGrenade = NULL;
	p->iMaxAmmoGrenade = -1;
	p->iMaxClip = AK47_MAX_CLIP;
	p->iSlot = 0;
	p->iPosition = 1;
	p->iId = m_iId = WEAPON_AK47;
	p->iFlags = 0;
	p->iWeight = AK47_WEIGHT;

	return 1;
}

BOOL CAK47Paladin::Deploy(void)
{
	m_flAccuracy = 0.2;
	m_iShotsFired = 0;
	iShellOn = 1;
	return DefaultDeploy("models/v_buffak.mdl", "models/p_buffak.mdl", AK47_DRAW, "ak47", UseDecrement() != FALSE);
}

void CAK47Paladin::PrimaryAttack(void)
{
	if (!FBitSet(m_pPlayer->pev->flags, FL_ONGROUND))
		AK47PaladinFire(0.04 + (0.4) * m_flAccuracy, 0.0955, FALSE);
	else if (m_pPlayer->pev->velocity.Length2D() > 140)
		AK47PaladinFire(0.04 + (0.07) * m_flAccuracy, 0.0955, FALSE);
	else
		AK47PaladinFire((0.0275), 0.0955, FALSE);
}

void CAK47Paladin::SecondaryAttack(void)
{
	if (!FBitSet(m_pPlayer->pev->flags, FL_ONGROUND))
		AK47PaladinFire2(0.04 + (0.4) * m_flAccuracy, 0.6, FALSE);
	else if (m_pPlayer->pev->velocity.Length2D() > 140)
		AK47PaladinFire2(0.04 + (0.07) * m_flAccuracy, 0.6, FALSE);
	else
		AK47PaladinFire2((0.0275), 0.6, FALSE);
}

Vector CAK47Paladin::Get_ShootPosition(CBaseEntity* pevAttacker, Vector Start)
{
	Vector end, vecforward, GunFire;

	end = pevAttacker->pev->v_angle;
	UTIL_MakeVectors(end);
	GunFire[0] = Start[0] + gpGlobals->v_forward[0] * 160.0 + gpGlobals->v_right[0] * 40.0 + gpGlobals->v_up[0] * -40.0;
	GunFire[1] = Start[1] + gpGlobals->v_forward[1] * 160.0 + gpGlobals->v_right[1] * 40.0 + gpGlobals->v_up[1] * -40.0;
	GunFire[2] = Start[2] + gpGlobals->v_forward[2] * 160.0 + gpGlobals->v_right[2] * 40.0 + gpGlobals->v_up[2] * -40.0;
	end = gpGlobals->v_forward;

	end = end * 8192.0;
	end = Start + end;


	TraceResult tr;
	UTIL_TraceLine(Start, end, dont_ignore_monsters, pevAttacker->edict(), &tr);
	end = tr.vecEndPos;

#ifndef CLIENT_DLL
	vecforward = (end - GunFire) / 17.5;
	if ((GunFire - end).Length() > 125.0)
	{

		int iCount = round((end - GunFire).Length() / 17.5);
		int iCount2 = iCount;
		while (iCount)
		{
			float fPrecent = float(iCount / iCount2);
			GunFire + vecforward;
			iCount--;
		}
	}
#endif
	return end;
}

#ifndef CLIENT_DLL
void CAK47Paladin::RadiusDamage(Vector vecAiming, float flDamage)
{
	float flRadius = 85.0f;

	if (g_pModRunning->DamageTrack() == DT_ZBS)
		flRadius = 140.0f;
	if (g_pModRunning->DamageTrack() == DT_ZB)
		flRadius = 125.0f;

	const Vector vecSrc = vecAiming;
	entvars_t* const pevAttacker = VARS(pev->owner);
	entvars_t* const pevInflictor = this->pev;
	int bitsDamageType = DMG_BULLET;

	TraceResult tr;
	//const float falloff = flRadius ? flDamage / flRadius : 1;
	const int bInWater = (UTIL_PointContents(vecSrc) == CONTENTS_WATER);

	CBaseEntity* pEntity = NULL;
	while ((pEntity = UTIL_FindEntityInSphere(pEntity, vecSrc, flRadius)) != NULL)
	{
		if (pEntity->pev->takedamage != DAMAGE_NO)
		{
			if (bInWater && !pEntity->pev->waterlevel)
				continue;

			if (!bInWater && pEntity->pev->waterlevel == 3)
				continue;

			if (pEntity->IsBSPModel())
				continue;

			if (pEntity->pev == pevAttacker)
				continue;

			Vector vecSpot = pEntity->BodyTarget(vecSrc);
			UTIL_TraceLine(vecSrc, vecSpot, missile, ENT(pevInflictor), &tr);

			if (tr.flFraction == 1.0f || tr.pHit == pEntity->edict())
			{
				if (tr.fStartSolid)
				{
					tr.vecEndPos = vecSrc;
					tr.flFraction = 0;
				}
				/*float flAdjustedDamage = flDamage - (vecSrc - pEntity->pev->origin).Length() * falloff;
				flAdjustedDamage = Q_max(0, flAdjustedDamage);*/

				if (tr.flFraction == 1.0f)
				{
					pEntity->TakeDamage(pevInflictor, pevAttacker, flDamage, bitsDamageType);
				}
				else
				{
					tr.iHitgroup = HITGROUP_CHEST;
					ClearMultiDamage();
					pEntity->TraceAttack(pevInflictor, flDamage, (tr.vecEndPos - vecSrc).Normalize(), &tr, bitsDamageType);
					ApplyMultiDamage(pevInflictor, pevAttacker);
				}
			}
		}
	}

	MESSAGE_BEGIN(MSG_BROADCAST, SVC_TEMPENTITY);
	WRITE_BYTE(TE_EXPLOSION);
	WRITE_COORD(vecAiming[0]);
	WRITE_COORD(vecAiming[1]);
	WRITE_COORD(vecAiming[2]);
	WRITE_SHORT(MODEL_INDEX("sprites/ef_buffak_hit.spr"));
	WRITE_BYTE(4);
	WRITE_BYTE(30);
	WRITE_BYTE(TE_EXPLFLAG_NOPARTICLES | TE_EXPLFLAG_NODLIGHTS | TE_EXPLFLAG_NOSOUND);
	MESSAGE_END();

	MESSAGE_BEGIN(MSG_BROADCAST, SVC_TEMPENTITY);
	WRITE_BYTE(TE_EXPLOSION);
	WRITE_COORD(vecAiming[0]);
	WRITE_COORD(vecAiming[1]);
	WRITE_COORD(vecAiming[2]);
	WRITE_SHORT(MODEL_INDEX("sprites/ef_buffak_hit.spr"));
	WRITE_BYTE(4);
	WRITE_BYTE(30);
	WRITE_BYTE(TE_EXPLFLAG_NOPARTICLES | TE_EXPLFLAG_NODLIGHTS | TE_EXPLFLAG_NOSOUND);
	MESSAGE_END();
}
#endif	

void CAK47Paladin::AK47PaladinFire(float flSpread, float flCycleTime, BOOL fUseAutoAim)
{
	m_bDelayFire = true;
	m_iShotsFired++;
	m_flAccuracy = ((float)(m_iShotsFired * m_iShotsFired * m_iShotsFired) / 200.0) + 0.35;

	if (m_flAccuracy > 1.25)
		m_flAccuracy = 1.25;

	if (m_iClip <= 0)
	{
		if (m_fFireOnEmpty)
		{
			PlayEmptySound();
			m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.2;
		}

		return;
	}

	m_iClip--;
	
#ifndef CLIENT_DLL
	m_pPlayer->SetAnimation(PLAYER_ATTACK1);
#endif
	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);
	Vector vecSrc = m_pPlayer->GetGunPosition();
	Vector vecDir = m_pPlayer->FireBullets3(vecSrc, gpGlobals->v_forward, flSpread, 8192, 2, BULLET_PLAYER_762MM, 36, 0.98, m_pPlayer->pev, FALSE, m_pPlayer->random_seed);

	int flags;
#ifdef CLIENT_WEAPONS
	flags = FEV_NOTHOST;
#else
	flags = 0;
#endif

	switch (RANDOM_LONG(1,2))
	{
	case 1:
		SendWeaponAnim(AK47_SHOOT1, UseDecrement() != FALSE);
		break;
	case 2:
		SendWeaponAnim(AK47_SHOOT2, UseDecrement() != FALSE);
		break;
	}
	EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_STATIC, "weapons/ak47buff-1.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM);

	PLAYBACK_EVENT_FULL(flags, m_pPlayer->edict(), m_usFirePaladin, 0, (float *)&g_vecZero, (float *)&g_vecZero, vecDir.x, vecDir.y, (int)(m_pPlayer->pev->punchangle.x * 100), (int)(m_pPlayer->pev->punchangle.y * 100), FALSE, FALSE);

	m_pPlayer->m_iWeaponVolume = NORMAL_GUN_VOLUME;
	m_pPlayer->m_iWeaponFlash = BRIGHT_GUN_FLASH;
	m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + flCycleTime;

#ifndef CLIENT_DLL
	if (!m_iClip && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
		m_pPlayer->SetSuitUpdate("!HEV_AMO0", FALSE, 0);
#endif
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.9;

	if (m_pPlayer->pev->velocity.Length2D() > 0)
		KickBack(1.5, 0.45, 0.225, 0.05, 6.5, 2.5, 7);
	else if (!FBitSet(m_pPlayer->pev->flags, FL_ONGROUND))
		KickBack(2.0, 1.0, 0.5, 0.35, 9.0, 6.0, 5);
	else if (FBitSet(m_pPlayer->pev->flags, FL_DUCKING))
		KickBack(0.9, 0.35, 0.15, 0.025, 5.5, 1.5, 9);
	else
		KickBack(1.0, 0.375, 0.175, 0.0375, 5.75, 1.75, 8);
}

void CAK47Paladin::AK47PaladinFire2(float flSpread, float flCycleTime, BOOL fUseAutoAim)
{
	m_bDelayFire = true;
	m_iShotsFired++;
	m_flAccuracy = ((float)(m_iShotsFired * m_iShotsFired * m_iShotsFired) / 200.0) + 0.35;

	if (m_flAccuracy > 1.25)
		m_flAccuracy = 1.25;

	if (m_iClip <= 0)
	{
		if (m_fFireOnEmpty)
		{
			PlayEmptySound();
			m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.2;
		}

		return;
	}

	m_iClip--;
	
#ifndef CLIENT_DLL
	m_pPlayer->SetAnimation(PLAYER_ATTACK1);
#endif
	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);
	Vector vecSrc = m_pPlayer->GetGunPosition();
	Vector vecDir = m_pPlayer->FireBullets3(vecSrc, gpGlobals->v_forward, flSpread, 8192, 2, BULLET_PLAYER_556MM, GetDamage(), 0.96, m_pPlayer->pev, FALSE, m_pPlayer->random_seed);

	int flags;
#ifdef CLIENT_WEAPONS
	flags = FEV_NOTHOST;
#else
	flags = 0;
#endif

	SendWeaponAnim(AK47_SHOOT3, UseDecrement() != FALSE);

	CBaseEntity* pevAttacker = this->m_pPlayer;
	auto vecShootPosition = Get_ShootPosition(pevAttacker, vecSrc);
#ifndef CLIENT_DLL
	RadiusDamage(vecShootPosition, g_pModRunning->DamageTrack() == DT_NONE ? 30 : 300);
#endif

	EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_STATIC, "weapons/ak47buff-2.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM);

	PLAYBACK_EVENT_FULL(flags, m_pPlayer->edict(), m_usFirePaladin2, 0, (float*)&g_vecZero, (float*)&g_vecZero, vecDir.x, vecDir.y, (int)(m_pPlayer->pev->punchangle.x * 100), (int)(m_pPlayer->pev->punchangle.y * 100), FALSE, FALSE);

	m_pPlayer->m_iWeaponVolume = NORMAL_GUN_VOLUME;
	m_pPlayer->m_iWeaponFlash = BRIGHT_GUN_FLASH;
	m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + flCycleTime;

#ifndef CLIENT_DLL
	if (!m_iClip && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
		m_pPlayer->SetSuitUpdate("!HEV_AMO0", FALSE, 0);
#endif
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.9;

	if (m_pPlayer->pev->velocity.Length2D() > 0)
		KickBack(1.5, 0.45, 0.225, 0.05, 6.5, 2.5, 7);
	else if (!FBitSet(m_pPlayer->pev->flags, FL_ONGROUND))
		KickBack(2.0, 1.0, 0.5, 0.35, 9.0, 6.0, 5);
	else if (FBitSet(m_pPlayer->pev->flags, FL_DUCKING))
		KickBack(0.9, 0.35, 0.15, 0.025, 5.5, 1.5, 9);
	else
		KickBack(1.0, 0.375, 0.175, 0.0375, 5.75, 1.75, 8);
}

void CAK47Paladin::Reload(void)
{
	if (m_pPlayer->ammo_762nato <= 0)
		return;

	if (DefaultReload(AK47_MAX_CLIP, AK47_RELOAD, 2.45))
	{
#ifndef CLIENT_DLL
		m_pPlayer->SetAnimation(PLAYER_RELOAD);
#endif
		m_flAccuracy = 0.2;
		m_iShotsFired = 0;
		m_bDelayFire = false;
	}
}

void CAK47Paladin::ItemPostFrame()
{

	return CBasePlayerWeapon::ItemPostFrame();
}

void CAK47Paladin::WeaponIdle(void)
{
	ResetEmptySound();
	m_pPlayer->GetAutoaimVector(AUTOAIM_10DEGREES);

	if (m_flTimeWeaponIdle > UTIL_WeaponTimeBase())
		return;

	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 20;
	SendWeaponAnim(AK47_IDLE1, UseDecrement() != FALSE);
}

float CAK47Paladin::GetDamage() const
{
	float flDamage = 32.0f;
#ifndef CLIENT_DLL
	if (g_pModRunning->DamageTrack() == DT_ZB)
		flDamage = 70.0f;
	else if (g_pModRunning->DamageTrack() == DT_ZBS)
		flDamage = 70.0f;
#endif
	return flDamage;
}