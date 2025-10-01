/* =================================================================================== *
			 * =================== TechnoSoftware =================== *
 * =================================================================================== */

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "weapons.h"
#include "wpn_balrog1.h"
#ifndef CLIENT_DLL
#include "gamemode/mods.h"
#endif

enum balrog1_e
{
	BALROG_IDLE1,
	BALROG_IDLE2,
	BALROG_SHOOT1,
	BALROG_SHOOT2,
	BALROG_RELOAD,
	BALROG_DRAW,
	BALROG_CHANGEA,
	BALROG_CHANGEB,
	BALROG_RELOADB,
};

LINK_ENTITY_TO_CLASS(weapon_balrog1, CBalrog1)

void CBalrog1::Spawn(void)
{
	pev->classname = MAKE_STRING("weapon_balrog1");

	Precache();
	m_iId = WEAPON_DEAGLE;
	SET_MODEL(ENT(pev), "models/w_balrog1.mdl");

	m_iDefaultAmmo = DEAGLE_DEFAULT_GIVE;
	m_flAccuracy = 0.9;
	m_iWeaponState &= ~WPNSTATE_SHIELD_DRAWN;
	m_fMaxSpeed = 250;
	BalrogAmmo = 0;

	FallInit();
}

void CBalrog1::Precache(void)
{
	PRECACHE_MODEL("models/v_balrog1.mdl");
	PRECACHE_MODEL("models/p_balrog1.mdl");
	PRECACHE_MODEL("models/w_balrog1.mdl");
	PRECACHE_MODEL("sprites/ef_balrog1.spr");

	PRECACHE_SOUND("weapons/balrog1-1.wav");
	PRECACHE_SOUND("weapons/balrog1-2.wav");
	PRECACHE_SOUND("weapons/balrog1_draw.wav");
	PRECACHE_SOUND("weapons/balrog1_reload.wav");
	PRECACHE_SOUND("weapons/balrog1_changea.wav");
	PRECACHE_SOUND("weapons/balrog1_changeb.wav");
	PRECACHE_SOUND("weapons/balrog1_reloadb.wav");
	
	m_iShell = PRECACHE_MODEL("models/pshell.mdl");
	m_usFireBALROG1 = PRECACHE_EVENT(1, "events/balrog1.sc");
	m_usFire2BALROG1 = PRECACHE_EVENT(1, "events/2balrog1.sc");
}

int CBalrog1::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "50AE";
	p->iMaxAmmo1 = MAX_AMMO_50AE;
	p->pszAmmo2 = "balrog1ammo";
	p->iMaxAmmo2 = 0;
	p->pszAmmo3 = NULL;
	p->iMaxAmmo3 = -1;
	p->pszAmmoGrenade = NULL;
	p->iMaxAmmoGrenade = -1;
	p->iMaxClip = DEAGLE_MAX_CLIP;
	p->iSlot = 1;
	p->iPosition = 1;
	p->iId = m_iId = WEAPON_DEAGLE;
	p->iFlags = 0;
	p->iWeight = DEAGLE_WEIGHT;

	return 1;
}

BOOL CBalrog1::Deploy(void)
{
	m_flAccuracy = 0.9;
	m_iWeaponState &= ~WPNSTATE_SHIELD_DRAWN;
	m_pPlayer->m_bShieldDrawn = false;
	m_fMaxSpeed = 250;
	m_fireuse2 = false;
	m_pPlayer->m_rgAmmo[m_iSecondaryAmmoType] = 0;

	return DefaultDeploy("models/v_balrog1.mdl", "models/p_balrog1.mdl", BALROG_DRAW, "onehanded", UseDecrement() != FALSE);
}

void CBalrog1::PrimaryAttack(void)
{
	if (m_fireuse2 == false)
	{
		if (!FBitSet(m_pPlayer->pev->flags, FL_ONGROUND))
			BALROGFire((1.5) * (1 - m_flAccuracy), 0.3, FALSE);
		else if (m_pPlayer->pev->velocity.Length2D() > 0)
			BALROGFire((0.25) * (1 - m_flAccuracy), 0.3, FALSE);
		else if (FBitSet(m_pPlayer->pev->flags, FL_DUCKING))
			BALROGFire((0.115) * (1 - m_flAccuracy), 0.3, FALSE);
		else
			BALROGFire((0.13) * (1 - m_flAccuracy), 0.3, FALSE);
	}
	else
	{
		if (!FBitSet(m_pPlayer->pev->flags, FL_ONGROUND))
			BALROGFire2((1.5) * (1 - m_flAccuracy), 0.3, FALSE);
		else if (m_pPlayer->pev->velocity.Length2D() > 0)
			BALROGFire2((0.25) * (1 - m_flAccuracy), 0.3, FALSE);
		else if (FBitSet(m_pPlayer->pev->flags, FL_DUCKING))
			BALROGFire2((0.115) * (1 - m_flAccuracy), 0.3, FALSE);
		else
			BALROGFire2((0.13) * (1 - m_flAccuracy), 0.3, FALSE);
	}
}

void CBalrog1::SecondaryAttack(void)
{
	if (m_iClip == 0)
		return;

	if (m_iClip == 1)
		return;

	if (m_fireuse2 == false)
	{
		m_iClip--;
		m_pPlayer->m_rgAmmo[m_iSecondaryAmmoType]++;

		m_fireuse2 = true;
		SendWeaponAnim(BALROG_CHANGEA, UseDecrement() != FALSE);

	}
	else if(m_fireuse2 == true)
	{
		m_iClip++;
		m_pPlayer->m_rgAmmo[m_iSecondaryAmmoType]--;

		m_fireuse2 = false;
		SendWeaponAnim(BALROG_CHANGEB, UseDecrement() != FALSE);
	}

	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 2;
	m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 2;
	m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 2;
}

void CBalrog1::BALROGFire(float flSpread, float flCycleTime, BOOL fUseAutoAim)
{
	flCycleTime -= 0.075;
	m_iShotsFired++;

	if (m_iShotsFired > 1)
		return;

	if (m_flLastFire)
	{
		m_flAccuracy -= (0.4 - (gpGlobals->time - m_flLastFire)) * 0.35;

		if (m_flAccuracy > 0.9)
			m_flAccuracy = 0.9;
		else if (m_flAccuracy < 0.55)
			m_flAccuracy = 0.55;
	}

	m_flLastFire = gpGlobals->time;

	if (m_iClip <= 0)
	{
		if (m_fFireOnEmpty)
		{
			PlayEmptySound();
			m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.2;
		}

		return;
	}

	EMIT_SOUND(ENT(pev), CHAN_WEAPON, "weapons/balrog1-1.wav", VOL_NORM, ATTN_NORM);

	SendWeaponAnim(BALROG_SHOOT1, UseDecrement() != FALSE);

	m_iClip--;
	m_pPlayer->pev->effects |= EF_MUZZLEFLASH;
	SetPlayerShieldAnim();
#ifndef CLIENT_DLL
	m_pPlayer->SetAnimation(PLAYER_ATTACK1);
#endif
	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);

	m_pPlayer->m_iWeaponVolume = BIG_EXPLOSION_VOLUME;
	m_pPlayer->m_iWeaponFlash = BRIGHT_GUN_FLASH;

	Vector vecSrc = m_pPlayer->GetGunPosition();

#ifndef CLIENT_DLL
	if (g_pModRunning->DamageTrack() == DT_ZBS)
		BALROG_DAMAGE = 280.0f;
	if (g_pModRunning->DamageTrack() == DT_ZB)
		BALROG_DAMAGE = 135.0f;
	else
		BALROG_DAMAGE = 34.0f;
#endif

	Vector vecDir = m_pPlayer->FireBullets3(vecSrc, gpGlobals->v_forward, flSpread, 4096, 2, BULLET_PLAYER_50AE, BALROG_DAMAGE, 0.81, m_pPlayer->pev, TRUE, m_pPlayer->random_seed);

	int flags;
#ifdef CLIENT_WEAPONS
	flags = FEV_NOTHOST;
#else
	flags = 0;
#endif

	PLAYBACK_EVENT_FULL(flags, m_pPlayer->edict(), m_usFireBALROG1, 0, (float *)&g_vecZero, (float *)&g_vecZero, vecDir.x, vecDir.y, (int)(m_pPlayer->pev->punchangle.x * 100), (int)(m_pPlayer->pev->punchangle.y * 100), m_iClip != 0, FALSE);

	m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + flCycleTime;

#ifndef CLIENT_DLL
	if (!m_iClip && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
		m_pPlayer->SetSuitUpdate("!HEV_AMO0", FALSE, 0);
#endif
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.8;
	m_pPlayer->pev->punchangle.x -= 2;
	ResetPlayerShieldAnim();
}

void CBalrog1::BALROGFire2(float flSpread, float flCycleTime, BOOL fUseAutoAim)
{
	flCycleTime -= 0.075;
	m_iShotsFired++;

	if (m_iShotsFired > 1)
		return;

	if (m_flLastFire)
	{
		m_flAccuracy -= (0.4 - (gpGlobals->time - m_flLastFire)) * 0.35;

		if (m_flAccuracy > 0.9)
			m_flAccuracy = 0.9;
		else if (m_flAccuracy < 0.55)
			m_flAccuracy = 0.55;
	}

	m_flLastFire = gpGlobals->time;

	if (m_pPlayer->m_rgAmmo[m_iSecondaryAmmoType] <= 0)
	{
		if (m_fFireOnEmpty)
		{
			PlayEmptySound();
			m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.2;
		}

		return;
	}

	EMIT_SOUND(ENT(pev), CHAN_WEAPON, "weapons/balrog1-2.wav", VOL_NORM, ATTN_NORM);

	SendWeaponAnim(BALROG_SHOOT2, UseDecrement() != FALSE);

	m_pPlayer->m_rgAmmo[m_iSecondaryAmmoType]--;
	m_pPlayer->pev->effects |= EF_MUZZLEFLASH;
	SetPlayerShieldAnim();
#ifndef CLIENT_DLL
	m_pPlayer->SetAnimation(PLAYER_ATTACK1);
#endif
	m_fireuse2 = false;
	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);

	m_pPlayer->m_iWeaponVolume = BIG_EXPLOSION_VOLUME;
	m_pPlayer->m_iWeaponFlash = BRIGHT_GUN_FLASH;

	Vector vecSrc = m_pPlayer->GetGunPosition();

#ifndef CLIENT_DLL
	if (g_pModRunning->DamageTrack() == DT_ZBS)
		BALROG_DAMAGE2 = 340.0f;
	if (g_pModRunning->DamageTrack() == DT_ZB)
		BALROG_DAMAGE2 = 225.0f;
	else
		BALROG_DAMAGE2 = 84.0f;
#endif

	Vector vecDir = m_pPlayer->FireBullets3(vecSrc, gpGlobals->v_forward, flSpread, 4096, 2, BULLET_PLAYER_50AE, BALROG_DAMAGE2, 0.81, m_pPlayer->pev, TRUE, m_pPlayer->random_seed);

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

	PLAYBACK_EVENT_FULL(flags, m_pPlayer->edict(), m_usFire2BALROG1, 0, (float*)&g_vecZero, (float*)&g_vecZero, vecDir.x, vecDir.y, (int)(m_pPlayer->pev->punchangle.x * 100), (int)(m_pPlayer->pev->punchangle.y * 100), m_iClip != 0, FALSE);

	m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 3;

#ifndef CLIENT_DLL
	if (!m_iClip && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
		m_pPlayer->SetSuitUpdate("!HEV_AMO0", FALSE, 0);
#endif
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 3;
	m_pPlayer->pev->punchangle.x -= 2;
	ResetPlayerShieldAnim();
}

void CBalrog1::ItemPostFrame()
{
	return CBasePlayerWeapon::ItemPostFrame();
}

void CBalrog1::Reload(void)
{
	if (m_pPlayer->ammo_50ae <= 0)
		return;

	if (m_fireuse2 == true)
	{
		if (DefaultReload(DEAGLE_MAX_CLIP, BALROG_RELOADB, 2.5))
		{
			m_fireuse2 = false;

			#ifndef CLIENT_DLL
				m_pPlayer->SetAnimation(PLAYER_RELOAD);
			#endif

			m_flAccuracy = 0.9;
			m_pPlayer->m_rgAmmo[m_iSecondaryAmmoType]--;
		}
	}
	else
	{
		if (DefaultReload(DEAGLE_MAX_CLIP, BALROG_RELOAD, 2.5))
		{
			#ifndef CLIENT_DLL
				m_pPlayer->SetAnimation(PLAYER_RELOAD);
			#endif

			m_flAccuracy = 0.9;
		}
	}
}

void CBalrog1::WeaponIdle(void)
{
	ResetEmptySound();
	m_pPlayer->GetAutoaimVector(AUTOAIM_10DEGREES);

	if (m_flTimeWeaponIdle > UTIL_WeaponTimeBase())
		return;

	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 20;

	if (m_fireuse2 == true)
		SendWeaponAnim(BALROG_IDLE2, UseDecrement() != FALSE);
	else
		SendWeaponAnim(BALROG_IDLE1, UseDecrement() != FALSE);
}

int CBalrog1::ExtractAmmo(CBasePlayerWeapon* pWeapon)
{
	if (BalrogAmmo)
	{
		m_pPlayer->m_rgAmmo[m_iSecondaryAmmoType] = BalrogAmmo;
		BalrogAmmo = 0;
	}
	return CBasePlayerWeapon::ExtractAmmo(pWeapon);
}

Vector CBalrog1::Get_ShootPosition(CBaseEntity* pevAttacker, Vector Start)
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
void CBalrog1::RadiusDamage(Vector vecAiming, float flDamage)
{
	float flRadius = 85.0f;

	if (g_pModRunning->DamageTrack() == DT_ZBS)
		flRadius = 340.0f;
	if (g_pModRunning->DamageTrack() == DT_ZB)
		flRadius = 225.0f;

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
	WRITE_SHORT(MODEL_INDEX("sprites/ef_balrog1.spr"));
	WRITE_BYTE(4);
	WRITE_BYTE(30);
	WRITE_BYTE(TE_EXPLFLAG_NOPARTICLES | TE_EXPLFLAG_NODLIGHTS | TE_EXPLFLAG_NOSOUND);
	MESSAGE_END();

	MESSAGE_BEGIN(MSG_BROADCAST, SVC_TEMPENTITY);
	WRITE_BYTE(TE_EXPLOSION);
	WRITE_COORD(vecAiming[0]);
	WRITE_COORD(vecAiming[1]);
	WRITE_COORD(vecAiming[2]);
	WRITE_SHORT(MODEL_INDEX("sprites/ef_balrog1.spr"));
	WRITE_BYTE(4);
	WRITE_BYTE(30);
	WRITE_BYTE(TE_EXPLFLAG_NOPARTICLES | TE_EXPLFLAG_NODLIGHTS | TE_EXPLFLAG_NOSOUND);
	MESSAGE_END();
}
#endif	