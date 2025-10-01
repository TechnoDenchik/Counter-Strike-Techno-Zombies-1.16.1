/* =================================================================================== *
			 * =================== TechnoSoftware =================== *
 * =================================================================================== */

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "weapons.h"
#include "wpn_balrog3.h"
#ifndef CLIENT_DLL
#include "gamemode/mods.h"
#endif

enum balrog3_e
{
	BALROG_IDLE1,
	BALROG_RELOAD,
	BALROG_DRAW,
	BALROG_SHOOT1,
	BALROG_SHOOT2
};

LINK_ENTITY_TO_CLASS(weapon_balrog3, CBalrog3)

void CBalrog3::Spawn(void)
{
	pev->classname = MAKE_STRING("weapon_balrog3");

	Precache();
	m_iId = WEAPON_P90;
	SET_MODEL(ENT(pev), "models/w_balrog3.mdl");

	m_iDefaultAmmo = P90_DEFAULT_GIVE;
	m_flAccuracy = 0.2;
	m_iShotsFired = 0;
	m_bDelayFire = false;
	FireClip2 = false;

	FallInit();
}

void CBalrog3::Precache(void)
{
	PRECACHE_MODEL("models/v_balrog3.mdl");
	PRECACHE_MODEL("models/p_balrog3.mdl");
	PRECACHE_MODEL("models/w_balrog3.mdl");
	PRECACHE_MODEL("sprites/balrog5stack.spr");

	PRECACHE_SOUND("weapons/balrig3_boltpull.wav");
	PRECACHE_SOUND("weapons/balrig3_clipin.wav");
	PRECACHE_SOUND("weapons/balrig3_clipout.wav");
	PRECACHE_SOUND("weapons/balrig3_draw.wav");
	PRECACHE_SOUND("weapons/balrig3-1.wav");
	PRECACHE_SOUND("weapons/balrig3-2.wav");
	PRECACHE_SOUND("weapons/usas_draw.wav");

	m_iShell = PRECACHE_MODEL("models/rshell.mdl");
	m_usFireBALROG3 = PRECACHE_EVENT(1, "events/balrog3.sc");
	m_usFire2BALROG3 = PRECACHE_EVENT(1, "events/2balrog3.sc");
}

int CBalrog3::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "57mm";
	p->iMaxAmmo1 = 30;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->pszAmmo3 = NULL;
	p->iMaxAmmo3 = -1;
	p->pszAmmoGrenade = NULL;
	p->iMaxAmmoGrenade = -1;
	p->iMaxClip = P90_MAX_CLIP;
	p->iSlot = 0;
	p->iPosition = 8;
	p->iId = m_iId = WEAPON_P90;
	p->iFlags = 0;
	p->iWeight = P90_WEIGHT;

	return 1;
}

BOOL CBalrog3::Deploy(void)
{
	m_iShotsFired = 0;
	m_bDelayFire = false;
	m_flAccuracy = 0.2;
	FireClip2 = false;

	return DefaultDeploy("models/v_balrog3.mdl", "models/p_balrog3.mdl", BALROG_DRAW, "carbine", UseDecrement() != FALSE);
}

void CBalrog3::PrimaryAttack(void)
{
	if (FireClip2 == false)
	{
		if (!(m_pPlayer->pev->flags & FL_ONGROUND))
			BALROGFire((0.3) * m_flAccuracy, 0.076, FALSE);
		else if (m_pPlayer->pev->velocity.Length2D() > 170)
			BALROGFire((0.115) * m_flAccuracy, 0.076, FALSE);
		else
			BALROGFire((0.045) * m_flAccuracy, 0.076, FALSE);
	}
	else
	{
		if (!(m_pPlayer->pev->flags & FL_ONGROUND))
			BALROGFire2((0.3) * m_flAccuracy, 0.061, FALSE);
		else if (m_pPlayer->pev->velocity.Length2D() > 170)
			BALROGFire2((0.115) * m_flAccuracy, 0.061, FALSE);
		else
			BALROGFire2((0.045) * m_flAccuracy, 0.061, FALSE);
	}
}

void CBalrog3::BALROGFire(float flSpread, float flCycleTime, BOOL fUseAutoAim)
{
	m_bDelayFire = true;
	m_iShotsFired++;
	m_flAccuracy = ((float)(m_iShotsFired * m_iShotsFired * m_iShotsFired) / 175) + 0.45;

	if (m_flAccuracy > 1)
		m_flAccuracy = 1;

	if (m_iClip <= 0)
	{
		if (m_fFireOnEmpty)
		{
			PlayEmptySound();
			m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.2;
		}

		return;
	}

	FireClip++;

	if (FireClip > 25)
	{
		FireClip2 = true;

		m_iClip--;
		m_pPlayer->pev->effects |= EF_MUZZLEFLASH;
	#ifndef CLIENT_DLL
		m_pPlayer->SetAnimation(PLAYER_ATTACK1);
	#endif

		EMIT_SOUND(ENT(pev), CHAN_WEAPON, "weapons/balrog5-1.wav", VOL_NORM, ATTN_NORM);

		SendWeaponAnim(BALROG_SHOOT1, UseDecrement() != FALSE);
	}
	else
	{
		m_iClip--;
		m_pPlayer->pev->effects |= EF_MUZZLEFLASH;
#ifndef CLIENT_DLL
		m_pPlayer->SetAnimation(PLAYER_ATTACK1);
#endif

		EMIT_SOUND(ENT(pev), CHAN_WEAPON, "weapons/balrog5-1.wav", VOL_NORM, ATTN_NORM);

		SendWeaponAnim(BALROG_SHOOT1, UseDecrement() != FALSE);

		FireClip2 = false;
	}
	

	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);

	m_pPlayer->m_iWeaponVolume = NORMAL_GUN_VOLUME;
	m_pPlayer->m_iWeaponFlash = DIM_GUN_FLASH;

#ifndef CLIENT_DLL
	if (g_pModRunning->DamageTrack() == DT_ZBS)
		BALROG_DAMAGE = 240.0f;
	if (g_pModRunning->DamageTrack() == DT_ZB)
		BALROG_DAMAGE = 125.0f;
	else
		BALROG_DAMAGE = 44.0f;
#endif


	Vector vecSrc = m_pPlayer->GetGunPosition();
	Vector vecDir = m_pPlayer->FireBullets3(vecSrc, gpGlobals->v_forward, flSpread, 8192, 1, BULLET_PLAYER_57MM, BALROG_DAMAGE, 0.885, m_pPlayer->pev, FALSE, m_pPlayer->random_seed);

	int flags;
#ifdef CLIENT_WEAPONS
	flags = FEV_NOTHOST;
#else
	flags = 0;
#endif

	PLAYBACK_EVENT_FULL(flags, m_pPlayer->edict(), m_usFireBALROG3, 0, (float *)&g_vecZero, (float *)&g_vecZero, vecDir.x, vecDir.y, (int)(m_pPlayer->pev->punchangle.x * 100), (int)(m_pPlayer->pev->punchangle.y * 100), 5, FALSE);
	m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + flCycleTime;
#ifndef CLIENT_DLL
	if (!m_iClip && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
		m_pPlayer->SetSuitUpdate("!HEV_AMO0", FALSE, 0);
#endif
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 2;

	if (!FBitSet(m_pPlayer->pev->flags, FL_ONGROUND))
		KickBack(0.9, 0.45, 0.35, 0.04, 5.25, 3.5, 4);
	else if (m_pPlayer->pev->velocity.Length2D() > 0)
		KickBack(0.45, 0.3, 0.2, 0.0275, 4.0, 2.25, 7);
	else if (FBitSet(m_pPlayer->pev->flags, FL_DUCKING))
		KickBack(0.275, 0.2, 0.125, 0.02, 3.0, 1.0, 9);
	else
		KickBack(0.3, 0.225, 0.125, 0.02, 3.25, 1.25, 8);
}

void CBalrog3::BALROGFire2(float flSpread, float flCycleTime, BOOL fUseAutoAim)
{
	m_bDelayFire = true;
	m_iShotsFired++;
	m_flAccuracy = ((float)(m_iShotsFired * m_iShotsFired * m_iShotsFired) / 175) + 0.45;

	if (m_flAccuracy > 1)
		m_flAccuracy = 1;

	if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
	{
		if (m_fFireOnEmpty)
		{
			PlayEmptySound();
			m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.2;
		}

		return;
	}

	FireClip = 0;

	m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]--;

	m_pPlayer->pev->effects |= EF_MUZZLEFLASH;
#ifndef CLIENT_DLL
	m_pPlayer->SetAnimation(PLAYER_ATTACK1);
#endif

	EMIT_SOUND(ENT(pev), CHAN_WEAPON, "weapons/balrog5-2.wav", VOL_NORM, ATTN_NORM);

	SendWeaponAnim(BALROG_SHOOT2, UseDecrement() != FALSE);

	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);

	m_pPlayer->m_iWeaponVolume = NORMAL_GUN_VOLUME;
	m_pPlayer->m_iWeaponFlash = DIM_GUN_FLASH;

#ifndef CLIENT_DLL
	if (g_pModRunning->DamageTrack() == DT_ZBS)
		BALROG_DAMAGE2 = 440.0f;
	if (g_pModRunning->DamageTrack() == DT_ZB)
		BALROG_DAMAGE2 = 325.0f;
	else
		BALROG_DAMAGE2 = 84.0f;
#endif

	Vector vecSrc = m_pPlayer->GetGunPosition();
	Vector vecDir = m_pPlayer->FireBullets3(vecSrc, gpGlobals->v_forward, flSpread, 8192, 1, BULLET_PLAYER_57MM, BALROG_DAMAGE2, 0.885, m_pPlayer->pev, FALSE, m_pPlayer->random_seed);

	int flags;
#ifdef CLIENT_WEAPONS
	flags = FEV_NOTHOST;
#else
	flags = 0;
#endif

	CBaseEntity* pevAttacker = this->m_pPlayer;
	auto vecShootPosition = Get_ShootPosition(pevAttacker, vecSrc);

#ifndef CLIENT_DLL
	RadiusDamage(vecShootPosition, g_pModRunning->DamageTrack() == DT_NONE ? 30 : 300);
#endif

	PLAYBACK_EVENT_FULL(flags, m_pPlayer->edict(), m_usFire2BALROG3, 0, (float*)&g_vecZero, (float*)&g_vecZero, vecDir.x, vecDir.y, (int)(m_pPlayer->pev->punchangle.x * 100), (int)(m_pPlayer->pev->punchangle.y * 100), 5, FALSE);
	m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + flCycleTime;
#ifndef CLIENT_DLL
	if (!m_iClip && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
		m_pPlayer->SetSuitUpdate("!HEV_AMO0", FALSE, 0);
#endif
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 2;

	if (!FBitSet(m_pPlayer->pev->flags, FL_ONGROUND))
		KickBack(0.9, 0.45, 0.35, 0.04, 5.25, 3.5, 4);
	else if (m_pPlayer->pev->velocity.Length2D() > 0)
		KickBack(0.45, 0.3, 0.2, 0.0275, 4.0, 2.25, 7);
	else if (FBitSet(m_pPlayer->pev->flags, FL_DUCKING))
		KickBack(0.275, 0.2, 0.125, 0.02, 3.0, 1.0, 9);
	else
		KickBack(0.3, 0.225, 0.125, 0.02, 3.25, 1.25, 8);
}

void CBalrog3::Reload(void)
{
	if (m_pPlayer->ammo_57mm <= 0)
		return;

	if (DefaultReload(P90_MAX_CLIP, BALROG_RELOAD, 3.4))
	{
		FireClip2 = false;
#ifndef CLIENT_DLL
		m_pPlayer->SetAnimation(PLAYER_RELOAD);
#endif
		m_flAccuracy = 0.2;
		m_iShotsFired = 0;
	}
}

void CBalrog3::WeaponIdle(void)
{
	FireClip2 = false;
	ResetEmptySound();
	m_pPlayer->GetAutoaimVector(AUTOAIM_10DEGREES);

	if (m_flTimeWeaponIdle > UTIL_WeaponTimeBase())
		return;

	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 20;
	SendWeaponAnim(BALROG_IDLE1, UseDecrement() != FALSE);
}

Vector CBalrog3::Get_ShootPosition(CBaseEntity* pevAttacker, Vector Start)
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
void CBalrog3::RadiusDamage(Vector vecAiming, float flDamage)
{
	float flRadius = 85.0f;

	if (g_pModRunning->DamageTrack() == DT_ZBS)
		flRadius = 85.0f;
	if (g_pModRunning->DamageTrack() == DT_ZB)
		flRadius = 85.0f;

	const Vector vecSrc = vecAiming;
	entvars_t* const pevAttacker = VARS(pev->owner);
	entvars_t* const pevInflictor = this->pev;
	int bitsDamageType = DMG_BULLET;

	TraceResult tr;
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
	WRITE_SHORT(MODEL_INDEX("sprites/balrog5stack.spr"));
	WRITE_BYTE(7);
	WRITE_BYTE(15);
	WRITE_BYTE(TE_EXPLFLAG_NOPARTICLES | TE_EXPLFLAG_NODLIGHTS | TE_EXPLFLAG_NOSOUND);
	MESSAGE_END();

	MESSAGE_BEGIN(MSG_BROADCAST, SVC_TEMPENTITY);
	WRITE_BYTE(TE_EXPLOSION);
	WRITE_COORD(vecAiming[0]);
	WRITE_COORD(vecAiming[1]);
	WRITE_COORD(vecAiming[2]);
	WRITE_SHORT(MODEL_INDEX("sprites/balrog5stack.spr"));
	WRITE_BYTE(7);
	WRITE_BYTE(15);
	WRITE_BYTE(TE_EXPLFLAG_NOPARTICLES | TE_EXPLFLAG_NODLIGHTS | TE_EXPLFLAG_NOSOUND);
	MESSAGE_END();
}
#endif	