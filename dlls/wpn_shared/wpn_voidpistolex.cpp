/* =================================================================================== *
			 * =================== TechnoSoftware =================== *
 * =================================================================================== */

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "weapons.h"
#include "wpn_voidpistolex.h"


#ifndef CLIENT_DLL
#include "gamemode/mods.h"
#endif

enum blackhole_anim
{
	BLACKHOLE_START,
	BLACKHOLE_LOOP,
	BLACKHOLE_END
};

#ifndef CLIENT_DLL

	class CVoidpistolExBlackhole : public CBaseEntity
	{
	public:

		KnockbackData GetKnockBackData() { return { 50.0f,20.0f,30.0f,10.0f,0.6f }; }
		float m_flNextSpawnMen;
		void Spawn() override
		{
			Precache();

			SET_MODEL(this->edict(), "models/ef_voidpistolex.mdl");
			pev->animtime = gpGlobals->time;
			pev->sequence = 0;
			pev->framerate = 1.0;	
			pev->solid = SOLID_BBOX; // 2	
			pev->rendermode = kRenderTransAlpha; // 5;
			pev->renderfx = kRenderFxNone;
			pev->renderamt = 255.0;
			pev->movetype = MOVETYPE_FLY; // 9
			SetThink(&CVoidpistolExBlackhole::OnStart);
			pev->nextthink = gpGlobals->time + 0.01f;
			pev->classname = MAKE_STRING("voidpistol");
		}

		void OnStart()
		{
			pev->sequence = 0; 
			pev->animtime = gpGlobals->time;
			pev->framerate = 1.0;
			m_iState = BLACKHOLE_START;
			PLAYBACK_EVENT_FULL(FEV_GLOBAL, ENT(pev), m_usFireVoidpistol, 0.0, pev->origin, (float*)&g_vecZero, 0.0, 0.0, 0, m_iState, FALSE, TRUE);

			SetThink(&CVoidpistolExBlackhole::OnLoop);
			pev->nextthink = gpGlobals->time + 1.5f;
		}

		void OnLoop()
		{		
			if (!m_iState)
			{
				m_iState = BLACKHOLE_LOOP;
				pev->sequence = 1;
				pev->animtime = gpGlobals->time;
				pev->framerate = 1.0;
				m_freq = 30;
				m_flLoopTime = gpGlobals->time + 4;
				m_SoundRepeat = 0;
				EMIT_SOUND(ENT(pev), CHAN_WEAPON, "weapons/voidpistolex_blackhole_idle.wav", VOL_NORM, ATTN_NORM);
				PLAYBACK_EVENT_FULL(FEV_GLOBAL, ENT(pev), m_usFireVoidpistol, 0.0, pev->origin, (float*)&g_vecZero, 0.0, 0.0, 0, m_iState, FALSE, TRUE);
			}
				RadiusDamage(pev->origin, m_BlackholeDamage, TRUE, m_freq);

			if (m_freq == 30)
			{
				m_freq = 0;
			}
			if (m_SoundRepeat == 100)
			{
				EMIT_SOUND(ENT(pev), CHAN_WEAPON, "weapons/voidpistolex_blackhole_idle.wav", VOL_NORM, ATTN_NORM);
				m_SoundRepeat = 0;
			}
			m_SoundRepeat++;
			m_freq++;
			pev->nextthink = gpGlobals->time + 0.01f;
			//RadiusDamage();
			if (gpGlobals->time > m_flLoopTime)		//after 4s
			{
				SetThink(&CVoidpistolExBlackhole::OnEnd);
				pev->nextthink = gpGlobals->time + 0.01f;
			}

		}
		
		void OnEnd()
		{	
			if (m_iState != BLACKHOLE_END)
			{
				pev->sequence = 2;
				pev->animtime = gpGlobals->time;
				pev->framerate = 1.0;
				m_iState = BLACKHOLE_END;
				m_flLoopTime = gpGlobals->time + 1.6f;
				m_freq = 0;
				PLAYBACK_EVENT_FULL(FEV_GLOBAL, ENT(pev), m_usFireVoidpistol, 0.0, pev->origin, (float*)&g_vecZero, 0.0, 0.0, 0, m_iState, FALSE, TRUE);
			}

			

				RadiusDamage(pev->origin, DetonationDamage(), FALSE, m_freq);
			if (m_freq == 72)
			{	
				m_freq = 73;
			}
			m_freq += 1;
			pev->nextthink = gpGlobals->time + 0.01f;
			if (gpGlobals->time > m_flLoopTime)		//after 1.6s
			{
				SetThink(&CVoidpistolExBlackhole::Remove);
				pev->nextthink = gpGlobals->time + 0.01f;
			}
			
		}

		void RadiusDamage(Vector vecAiming, float flDamage, bool bIsLoop, int freq)
		{
			float flRadius = 600.0f;
			if (g_pModRunning->DamageTrack() == DT_ZBS)
				flRadius = 600.0f;
			if (g_pModRunning->DamageTrack() == DT_ZB)
				flRadius = 600.0f;
			const Vector vecSrc = vecAiming;

			entvars_t* const pevAttacker = VARS(pev->owner);
			entvars_t* const pevInflictor = this->pev;
			int bitsDamageType = DMG_BULLET;

			TraceResult tr;
			const float falloff = flRadius ? flDamage / flRadius : 1;
			const int bInWater = (UTIL_PointContents(vecSrc) == CONTENTS_WATER);

			CBaseEntity* pEntity = NULL;
			while ((pEntity = UTIL_FindEntityInSphere(pEntity, vecSrc, flRadius)) != NULL)
			{
				if (pEntity->pev->takedamage != DAMAGE_NO)
				{
					if (pEntity->pev == pevAttacker)
						continue;

					if (bInWater && !pEntity->pev->waterlevel)
						continue;

					if (!bInWater && pEntity->pev->waterlevel == 3)
						continue;

					if (pEntity->IsBSPModel())
						continue;

					if (!m_pPlayer->m_bIsZombie)
					{
						if (bIsLoop)
						{
							if (pEntity->IsPlayer())
							{
								if (g_pGameRules->PlayerRelationship(m_pPlayer, pEntity) != GR_TEAMMATE)
								{
									CBasePlayer* pPlayer;
									Vector vecPullVelocity;
									const float power = 8;
									pPlayer = (CBasePlayer*)pEntity;
									vecPullVelocity = (vecSrc - pPlayer->pev->origin) * power;
									pPlayer->pev->velocity = vecPullVelocity;
								}
							}
						}
						else
						{
							if (pEntity->IsPlayer())
							{
								if (g_pGameRules->PlayerRelationship(m_pPlayer, pEntity) != GR_TEAMMATE)
								{
									CBasePlayer* pPlayer;
									Vector vecPullVelocity;
									const float power = 1.3;
									pPlayer = (CBasePlayer*)pEntity;
									vecPullVelocity = -(vecSrc - pPlayer->pev->origin) * power;
									pPlayer->pev->velocity = vecPullVelocity;
								}
							}
						}

						Vector vecSpot = pEntity->BodyTarget(vecSrc);
						UTIL_TraceLine(vecSrc, vecSpot, missile, ENT(pevInflictor), &tr);

						if (tr.flFraction == 1.0f || tr.pHit == pEntity->edict())
						{
							if (tr.fStartSolid)
							{
								tr.vecEndPos = vecSrc;
								tr.flFraction = 0;
							}
							float flAdjustedDamage = flDamage - (vecSrc - pEntity->pev->origin).Length() * falloff;
							flAdjustedDamage = Q_max(0, flAdjustedDamage);

							if (freq == 30 || freq == 72)
							{
								if (tr.flFraction == 1.0f)
								{
									pEntity->TakeDamage(pevInflictor, pevAttacker, flAdjustedDamage, bitsDamageType);
								}
								else
								{
									tr.iHitgroup = HITGROUP_CHEST;
									ClearMultiDamage();
									pEntity->TraceAttack(pevAttacker, flAdjustedDamage, (tr.vecEndPos - vecSrc).Normalize(), &tr, bitsDamageType);
									ApplyMultiDamage(pevInflictor, pevAttacker);
								}
							}
							
						}
					}
				}
			}
		}


		float DetonationDamage() const
		{
			float flDamage = 40.0f;
#ifndef CLIENT_DLL
			if (g_pModRunning->DamageTrack() == DT_ZB)
				flDamage = 200.0f;
			//flDamage = 750.0f;
			else if (g_pModRunning->DamageTrack() == DT_ZBS)
				flDamage = 1500.0f;
			//flDamage = 1500.0f;
#endif
			return flDamage;
		}

		void Init(float flDamage, int eventindex, CBasePlayer* pPlayer)
		{
			m_BlackholeDamage = flDamage;
			m_usFireVoidpistol = eventindex;
			m_pPlayer = pPlayer;
		}

		float m_BlackholeDamage;
		int m_usFireVoidpistol;
		int m_freq;
		int m_SoundRepeat;;
		CBasePlayer* m_pPlayer;
		int m_iState;
		time_point_t m_flLoopTime;

	protected:
		void Remove()
		{
			SetThink(nullptr);
			SetTouch(nullptr);
			pev->effects |= EF_NODRAW; // 0x80u

			return UTIL_Remove(this);
		}

	};
	LINK_ENTITY_TO_CLASS(voidpistolex_blackhole, CVoidpistolExBlackhole)

	class CVoidpistolExProjectile : public CBaseEntity
	{
	public:
		void Spawn() override
		{
			Precache();

			SET_MODEL(this->edict(), "models/ef_voidpistolex_projectile.mdl");
			SetThink(&CVoidpistolExProjectile::OnThink);
			SetTouch(&CVoidpistolExProjectile::OnTouch);
			m_bCreateSpr = FALSE;
			pev->solid = SOLID_BBOX; // 2
			pev->nextthink = gpGlobals->time + 0.01f;
			m_flExplodeTime = gpGlobals->time + 4;
			pev->rendermode = kRenderTransAdd; // 5;
			pev->renderfx = kRenderFxNone;
			pev->renderamt = 255.0;
			pev->framerate = 1.0;
			UTIL_SetSize(pev, { -14, -14, -14 }, { 14, 14, 14 });
			pev->solid = SOLID_BBOX; // 2
			pev->movetype = MOVETYPE_FLY; // 9
		}

		void EXPORT OnTouch(CBaseEntity* pOther)
		{
			if (pev->owner == pOther->edict())
				return;
			Explode();
		}

		void Precache() override
		{
		}

		void OnThink()
		{
			if (gpGlobals->time < m_flExplodeTime)
			{
				if (!m_bCreateSpr)
				{
					m_bCreateSpr = TRUE;
					PLAYBACK_EVENT_FULL(FEV_GLOBAL, ENT(pev), m_usFireVoidpistol, 0.0, pev->origin, vecForward, 0.0, 0.0, 0, 4, FALSE, TRUE);
				}
				
			}
			else
			{
				Explode();
			}
			

			pev->nextthink = gpGlobals->time + 0.01f;
		}
		void Explode()
		{
			CVoidpistolExBlackhole* pEnt = static_cast<CVoidpistolExBlackhole*>(CBaseEntity::Create("voidpistolex_blackhole", pev->origin, pev->angles, pev->owner));
			if (pEnt)
			{
				pEnt->Init(m_BlackholeDamage, m_usFireVoidpistol, m_pPlayer);
			}
			Remove();
		}
		void Init(Vector vecVelocity, float flDamage, CBasePlayer* player, int eventindex)
		{
			std::tie(m_BlackholeDamage, m_iTeam) = std::make_tuple(flDamage, player->m_iTeam);
			vecForward = vecVelocity;
			pev->velocity = vecVelocity * 600;
			m_vecStartOrigin = pev->origin;
			VEC_TO_ANGLES(vecVelocity.Normalize(), pev->angles);
			pev->owner = player->edict();
			m_usFireVoidpistol = eventindex;
			m_pPlayer = player;
		}

		int m_usFireVoidpistol;
		time_point_t m_flExplodeTime;
		bool m_bCreateSpr;
		float m_BlackholeDamage;
		CBasePlayer* m_pPlayer;
		Vector vecForward;
		Vector m_vecStartOrigin;
		Vector m_vecStartVelocity;
		TeamName m_iTeam;

	protected:
		void Remove()
		{
			SetTouch(nullptr);
			SetThink(nullptr);
			pev->effects |= EF_NODRAW; // 0x80u
			return UTIL_Remove(this);
		}

	};

	LINK_ENTITY_TO_CLASS(voidpistolex_projectile, CVoidpistolExProjectile)

#endif


enum voidpistolex_e
{
	VOIDPISTOL_IDLEA,
	VOIDPISTOL_IDLEB,
	VOIDPISTOL_IDLEC,
	VOIDPISTOL_SHOOTA,
	VOIDPISTOL_SHOOTB,
	VOIDPISTOL_SHOOTC,
	VOIDPISTOL_SHOOT_BLACKHOLE_A,
	VOIDPISTOL_SHOOT_BLACKHOLE_B,
	VOIDPISTOL_RELOADA,
	VOIDPISTOL_RELOADB,
	VOIDPISTOL_RELOADC,
	VOIDPISTOL_SCANNING_ON,
	VOIDPISTOL_SCANNING_OFF,
	VOIDPISTOL_CHANGEAC,
	VOIDPISTOL_CHANGEBC,
	VOIDPISTOL_DRAWA,
	VOIDPISTOL_DRAWB,
	VOIDPISTOL_DRAWC,
};

enum voidpistolex_mode
{
	VOIDPISTOL_MODEA,
	VOIDPISTOL_MODEB,
	VOIDPISTOL_MODEC
};

LINK_ENTITY_TO_CLASS(weapon_voidpistolex, CVoidpistolEx)

void CVoidpistolEx::Spawn(void)
{
	pev->classname = MAKE_STRING("weapon_voidpistolex");

	Precache();
	m_iId = WEAPON_DEAGLE;
	SET_MODEL(ENT(pev), "models/w_voidpistolex.mdl");
	m_iCharging = 0;
	m_iDefaultAmmo = 300;
	m_iDefaultAmmo2 = 0;
	m_flAccuracy = 0.9;
	m_iMode = VOIDPISTOL_MODEA;
	m_fMaxSpeed = 250;

	FallInit();
}

void CVoidpistolEx::Precache(void)
{
	PRECACHE_MODEL("models/v_voidpistolex.mdl");
	PRECACHE_MODEL("models/w_voidpistolex.mdl");
	PRECACHE_MODEL("models/p_voidpistolex.mdl");

	PRECACHE_MODEL("sprites/ef_voidpistolex01.spr"); //charing spr attached to model
	PRECACHE_MODEL("sprites/ef_voidpistolex02.spr");
	PRECACHE_MODEL("sprites/ef_voidpistolex03.spr");
	PRECACHE_MODEL("sprites/ef_voidpistolex04.spr");//infront of the gun charing finished
	PRECACHE_MODEL("models/ef_voidpistolex_projectile.mdl");

	PRECACHE_MODEL("sprites/ef_voidpistolex_star.spr"); //muzzleflash
	
	PRECACHE_MODEL("models/ef_voidpistolex.mdl");
	
	PRECACHE_MODEL("sprites/ef_voidpistolex_start.spr");//shooting project
	PRECACHE_MODEL("sprites/ef_voidpistolex_loop.spr");
	PRECACHE_MODEL("sprites/ef_voidpistolex_end.spr");

	PRECACHE_SOUND("weapons/voidpistol-1.wav");
	PRECACHE_SOUND("weapons/voidpistol-2.wav");


	m_iShell = PRECACHE_MODEL("models/pshell.mdl");
	m_usFireVoidpistolEx = PRECACHE_EVENT(1, "events/voidpistolex.sc");
}

int CVoidpistolEx::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "VoidExAmmo";
	p->iMaxAmmo1 = 450;
	p->pszAmmo2 = "VoidpistolProjectile";
	p->iMaxAmmo2 = 0;
	p->pszAmmo3 = NULL;
	p->iMaxAmmo3 = -1;
	p->pszAmmoGrenade = NULL;
	p->iMaxAmmoGrenade = -1;
	p->iMaxClip = VOIDPISTOL_MAX_CLIP;
	p->iSlot = 1;
	p->iPosition = 1;
	p->iId = m_iId = WEAPON_DEAGLE;
	p->iFlags = 0;
	p->iWeight = DEAGLE_WEIGHT;

	return 1;
}

BOOL CVoidpistolEx::Deploy(void)
{
	m_flAccuracy = 0.9;

	m_fMaxSpeed = 250;
#ifndef CLIENT_DLL
	if(IsModeCEnabled(m_iCharging))
		return DefaultDeploy("models/v_voidpistolex.mdl", "models/p_voidpistolex.mdl", VOIDPISTOL_DRAWC, "onehanded", UseDecrement() != FALSE);
	else
	{
		switch (m_iMode)
		{
		case VOIDPISTOL_MODEA:
		{
			return DefaultDeploy("models/v_voidpistolex.mdl", "models/p_voidpistolex.mdl", VOIDPISTOL_DRAWA, "onehanded", UseDecrement() != FALSE); break;
		}
		case VOIDPISTOL_MODEB:
		{
			return DefaultDeploy("models/v_voidpistolex.mdl", "models/p_voidpistolex.mdl", VOIDPISTOL_DRAWB, "onehanded", UseDecrement() != FALSE); break;
		}
		default:
			return FALSE;

		}
	}
#endif
	return TRUE;
}

void CVoidpistolEx::ItemPostFrame()
{
	Vector vecPlayerOrigin;
	vecPlayerOrigin = m_pPlayer->pev->origin;
#ifndef CLIENT_DLL

	CBaseEntity* pEntity = NULL;
	while ((pEntity = UTIL_FindEntityInSphere(pEntity, vecPlayerOrigin, VOIDPISTOL_SCAN_RADIUS)) != NULL)
	{
		if (pEntity->pev->takedamage != DAMAGE_NO)
		{
			if (pEntity->pev == m_pPlayer->pev)
				continue;

			if (pEntity->IsBSPModel())
				continue;

			if (pEntity->IsPlayer())
			{
				if(g_pGameRules->PlayerRelationship(m_pPlayer, pEntity) != GR_TEAMMATE)
				{
					if (!IsModeCEnabled(m_iCharging))
					{
						if (m_iMode == VOIDPISTOL_MODEB)
						{
							return CBasePlayerWeapon::ItemPostFrame();	//already MODE B
						}

						m_iMode = VOIDPISTOL_MODEB;
						SendWeaponAnim(VOIDPISTOL_SCANNING_ON, 0);
						m_flNextSecondaryAttack = m_flNextPrimaryAttack = 0.7f;
						m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.8f;
						pev->iuser1 = 0;
						return CBasePlayerWeapon::ItemPostFrame();
					}
					else
					{
						if (pev->iuser1)
						{				
							m_iMode = VOIDPISTOL_MODEB;
							return CBasePlayerWeapon::ItemPostFrame();	//already MODE B
						}
						pev->iuser1 = 1;
						m_iMode = VOIDPISTOL_MODEB;
						SendWeaponAnim(VOIDPISTOL_CHANGEBC, 0);
						m_flNextSecondaryAttack = m_flNextPrimaryAttack = 0.5f;
						m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.8f;
						return CBasePlayerWeapon::ItemPostFrame();
					}
				}
				else
				{
					if (!IsModeCEnabled(m_iCharging))
					{
						if (m_iMode == VOIDPISTOL_MODEA)
						{
							return CBasePlayerWeapon::ItemPostFrame();	//already MODE B
						}
						m_iMode = VOIDPISTOL_MODEA;
						SendWeaponAnim(VOIDPISTOL_SCANNING_OFF, 0);
						m_flNextSecondaryAttack = m_flNextPrimaryAttack = 0.57f;
						m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.8f;
						pev->iuser1 = 0;
					}
					else
					{
						if (pev->iuser1)
						{
							return CBasePlayerWeapon::ItemPostFrame();	//already MODE B
						}
						m_iMode = VOIDPISTOL_MODEB;
						pev->iuser1 = 1;
						SendWeaponAnim(VOIDPISTOL_CHANGEAC, 0);
						m_flNextSecondaryAttack = m_flNextPrimaryAttack = 0.5f;
						m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.8f;
					}
				}
			}		
		}
	}

	if (!IsModeCEnabled(m_iCharging))
	{
		if (m_iMode == VOIDPISTOL_MODEA)
		{
			return CBasePlayerWeapon::ItemPostFrame();
		}
		m_iMode = VOIDPISTOL_MODEA;
		SendWeaponAnim(VOIDPISTOL_SCANNING_OFF, 0);
		m_flNextSecondaryAttack = m_flNextPrimaryAttack = 0.57f;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.8f;
		pev->iuser1 = 0;
	}
	else
	{
		if (pev->iuser1)
		{		
			m_iMode = VOIDPISTOL_MODEB;
			return CBasePlayerWeapon::ItemPostFrame();
		}
		pev->iuser1 = 1;
		m_iMode = VOIDPISTOL_MODEB;
		SendWeaponAnim(VOIDPISTOL_CHANGEAC, 0);
		m_flNextSecondaryAttack = m_flNextPrimaryAttack = 0.5f;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.8f;
	}
	return CBasePlayerWeapon::ItemPostFrame();
	
#endif
}

void CVoidpistolEx::PrimaryAttack(void)
{
#ifndef CLIENT_DLL
	switch (m_iMode)
	{
	case VOIDPISTOL_MODEA:
	{
		if (!FBitSet(m_pPlayer->pev->flags, FL_ONGROUND))
			VoidpistolFireA((1.5) * (1 - m_flAccuracy), 0.215f, FALSE);
		else if (m_pPlayer->pev->velocity.Length2D() > 0)
			VoidpistolFireA((0.25) * (1 - m_flAccuracy), 0.215f, FALSE);
		else if (FBitSet(m_pPlayer->pev->flags, FL_DUCKING))
			VoidpistolFireA((0.115) * (1 - m_flAccuracy), 0.215f, FALSE);
		else
			VoidpistolFireA((0.13) * (1 - m_flAccuracy), 0.215f, FALSE);
		break;
	}
	case VOIDPISTOL_MODEB:
	{
		VoidpistolFireB((0.13) * (1 - m_flAccuracy), 0.215f, FALSE); break;
	}
	}
#endif
}
bool CVoidpistolEx::IsModeCEnabled(int iCharging)
{
	return iCharging > 60;
}

void CVoidpistolEx::VoidpistolFireC(void)
{
	Vector vecPlayerOrigin;
	vecPlayerOrigin = m_pPlayer->pev->origin;
	m_iShotsFired++;
#ifndef CLIENT_DLL
	m_pPlayer->SetAnimation(PLAYER_ATTACK1);
#endif
	m_pPlayer->m_rgAmmo[m_iSecondaryAmmoType]--;
#ifndef CLIENT_DLL
	m_iCharging = 0;
	CBaseEntity* pEntity = NULL;

	while ((pEntity = UTIL_FindEntityInSphere(pEntity, vecPlayerOrigin, VOIDPISTOL_SCAN_RADIUS)) != NULL)
	{
		if (pEntity->pev->takedamage != DAMAGE_NO)
		{
			if (pEntity->pev == m_pPlayer->pev)
				continue;

			if (pEntity->IsBSPModel())
				continue;

			if (pEntity->IsPlayer() && g_pGameRules->PlayerRelationship(m_pPlayer, pEntity) != GR_TEAMMATE)
			{
				EMIT_SOUND(ENT(pev), CHAN_WEAPON, "weapons/voidpistol-2.wav", VOL_NORM, ATTN_NORM);
				SendWeaponAnim(VOIDPISTOL_SHOOT_BLACKHOLE_B, UseDecrement() != FALSE);
				m_flNextSecondaryAttack = m_flNextPrimaryAttack = 1.03f;
				m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.1f;
				m_iMode = VOIDPISTOL_MODEB;
				break;
				
			}
			else
			{
				EMIT_SOUND(ENT(pev), CHAN_WEAPON, "weapons/voidpistol_shoot_blackhole.wav", VOL_NORM, ATTN_NORM);
				SendWeaponAnim(VOIDPISTOL_SHOOT_BLACKHOLE_A, UseDecrement() != FALSE);
				m_flNextSecondaryAttack = m_flNextPrimaryAttack = 1.03f;
				m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.1f;
				m_iMode = VOIDPISTOL_MODEA;
				break;
			}
		}
	}
	if (pEntity == NULL)
	{
		SendWeaponAnim(VOIDPISTOL_SHOOT_BLACKHOLE_A, UseDecrement() != FALSE);
		m_flNextSecondaryAttack = m_flNextPrimaryAttack = 1.03f;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.1f;
		m_iMode = VOIDPISTOL_MODEA;
	}
#endif
#ifndef CLIENT_DLL
	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);
	CVoidpistolExProjectile* pEnt = static_cast<CVoidpistolExProjectile*>(CBaseEntity::Create("voidpistolex_projectile", m_pPlayer->GetGunPosition() + 50.0 * gpGlobals->v_forward, m_pPlayer->pev->v_angle, ENT(m_pPlayer->pev)));
	if (pEnt)
	{
		pEnt->Init(gpGlobals->v_forward, BlackholeDamage(), m_pPlayer, m_usFireVoidpistolEx);
	}
#endif
}

void CVoidpistolEx::VoidpistolFireB(float flSpread, duration_t flCycleTime, BOOL fUseSemi)
{
	Vector vecPlayerOrigin;
	TraceResult tr;
	vecPlayerOrigin = m_pPlayer->pev->origin;

	m_iShotsFired++;

	if (m_flLastFire )
	{
		m_flAccuracy -= (0.4 - ((gpGlobals->time - m_flLastFire) / 1)) * 0.35;

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
			m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.2f;
		}

		return;
	}

#ifndef CLIENT_DLL
	m_pPlayer->SetAnimation(PLAYER_ATTACK1);
#endif
		
	Vector vecSrc = m_pPlayer->GetGunPosition();
#ifndef CLIENT_DLL
		CBaseEntity* pEntity = NULL;
		while ((pEntity = UTIL_FindEntityInSphere(pEntity, vecPlayerOrigin, VOIDPISTOL_SCAN_RADIUS)) != NULL)
		{
			if (pEntity->pev->takedamage != DAMAGE_NO)
			{
				if (pEntity->pev == m_pPlayer->pev)
					continue;

				if (pEntity->IsBSPModel())
					continue;

				if (m_iCountPlayer > 10)
					continue;

				if (pEntity->IsPlayer())
				{
					if (g_pGameRules->PlayerRelationship(m_pPlayer, pEntity) == GR_TEAMMATE)
						continue;

					Vector vecSpot = pEntity->BodyTarget(vecSrc);
					UTIL_TraceLine(vecSrc, vecSpot, missile, ENT(m_pPlayer->pev), &tr);
					if (tr.flFraction < 1.0f)
					{
						if (m_iClip > 0)
						{
							m_iClip--;
							m_iCharging++;
							m_iCountPlayer++;
							tr.iHitgroup = HITGROUP_CHEST;
							ClearMultiDamage();
							pEntity->TraceAttack(m_pPlayer->pev, GetDamage(), (tr.vecEndPos - vecSrc).Normalize(), &tr, DMG_BULLET);
							ApplyMultiDamage(m_pPlayer->pev, m_pPlayer->pev);
						}
						else
							break;
						
					}
				}
			}
		}
		m_iCountPlayer = 0;

		//EMIT_SOUND(ENT(pev), CHAN_WEAPON, "weapons/voidpistol-1.wav", VOL_NORM, ATTN_NORM);
		//EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_STATIC, "weapons/voidpistol_beep.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM);

		CBaseEntity* pevAttacker = this->m_pPlayer;

		auto vecShootPosition = Get_ShootPosition(pevAttacker, vecSrc);

		RadiusDamage(vecShootPosition, 0);

		if (IsModeCEnabled(m_iCharging))
		{
			SendWeaponAnim(VOIDPISTOL_SHOOTC, UseDecrement() != FALSE);
			m_pPlayer->m_rgAmmo[m_iSecondaryAmmoType] = 1;
			m_iMode = VOIDPISTOL_MODEB;
		}
		else
		{
			SendWeaponAnim(VOIDPISTOL_SHOOTB, UseDecrement() != FALSE);
		}		

		m_pPlayer->m_iWeaponVolume = NORMAL_GUN_VOLUME; // 600
		m_pPlayer->m_iWeaponFlash = BRIGHT_GUN_FLASH; // 512

		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.0f;
#endif
		m_pPlayer->pev->effects |= EF_MUZZLEFLASH;
		PLAYBACK_EVENT_FULL(0, m_pPlayer->edict(), m_usFireVoidpistolEx, 0, (float*)&g_vecZero, (float*)&g_vecZero, 2.0, 0.0, 0, 0, IsModeCEnabled(m_iCharging), FALSE);

		m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + flCycleTime;

#ifndef CLIENT_DLL
		if (!m_iClip && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
			m_pPlayer->SetSuitUpdate("!HEV_AMO0", FALSE, 0);
#endif
		m_pPlayer->pev->punchangle.x -= 2;
		ResetPlayerShieldAnim();

}

void CVoidpistolEx::SecondaryAttack(void)
{
#ifndef CLIENT_DLL
	if (IsModeCEnabled(m_iCharging))
		VoidpistolFireC();
	else
		return;
#endif
}

Vector CVoidpistolEx::Get_ShootPosition(CBaseEntity* pevAttacker, Vector Start)
{
	CBaseEntity* pEntity = nullptr;
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
	vecforward = (end - GunFire) / 4.5;
	if ((GunFire - end).Length() > 40.0)
	{

		int iCount = round((end - GunFire).Length() / 4.5);
		int iCount2 = iCount;
		while (iCount)
		{

			Vector vecEffectPos = pev->origin - pev->velocity.Normalize() * 10;

			iCount--;
		}
	}

#endif
	return end;
}

#ifndef CLIENT_DLL
void CVoidpistolEx::RadiusDamage(Vector vecAiming, float flDamage)
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
			}
		}
	}

	MESSAGE_BEGIN(MSG_BROADCAST, SVC_TEMPENTITY);
	WRITE_BYTE(TE_EXPLOSION);
	WRITE_COORD(vecAiming[0]);
	WRITE_COORD(vecAiming[1]);
	WRITE_COORD(vecAiming[2]);
	WRITE_SHORT(MODEL_INDEX("sprites/ef_voidpistolex_star.spr"));
	WRITE_BYTE(3);
	WRITE_BYTE(40);
	WRITE_BYTE(TE_EXPLFLAG_NOPARTICLES | TE_EXPLFLAG_NODLIGHTS | TE_EXPLFLAG_NOSOUND);
	MESSAGE_END();

	MESSAGE_BEGIN(MSG_BROADCAST, SVC_TEMPENTITY);
	WRITE_BYTE(TE_EXPLOSION);
	WRITE_COORD(vecAiming[0]);
	WRITE_COORD(vecAiming[1]);
	WRITE_COORD(vecAiming[2]);
	WRITE_SHORT(MODEL_INDEX("sprites/ef_voidpistolex_star.spr"));
	WRITE_BYTE(3);
	WRITE_BYTE(40);
	WRITE_BYTE(TE_EXPLFLAG_NOPARTICLES | TE_EXPLFLAG_NODLIGHTS | TE_EXPLFLAG_NOSOUND);
	MESSAGE_END();


}
#endif	

void CVoidpistolEx::VoidpistolFireA(float flSpread, duration_t flCycleTime, BOOL fUseAutoAim)
{
	m_iShotsFired++;

	if (m_flLastFire )
	{
		m_flAccuracy -= (0.4 - ((gpGlobals->time - m_flLastFire) / 1)) * 0.35;

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
			m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.2f;
		}

		return;
	}

	m_iClip--;
	m_pPlayer->pev->effects |= EF_MUZZLEFLASH;
	SetPlayerShieldAnim();
#ifndef CLIENT_DLL
	m_pPlayer->SetAnimation(PLAYER_ATTACK1);
	SendWeaponAnim(VOIDPISTOL_SHOOTA, UseDecrement() != FALSE);
#endif
	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);

	m_pPlayer->m_iWeaponVolume = BIG_EXPLOSION_VOLUME;
	m_pPlayer->m_iWeaponFlash = BRIGHT_GUN_FLASH;

	Vector vecSrc = m_pPlayer->GetGunPosition();
	
	//EMIT_SOUND(ENT(pev), CHAN_WEAPON, "weapons/voidpistol-1.wav", VOL_NORM, ATTN_NORM);
	CBaseEntity* pevAttacker = this->m_pPlayer;

	auto vecShootPosition = Get_ShootPosition(pevAttacker, vecSrc);

#ifndef CLIENT_DLL	
	RadiusDamage(vecShootPosition, 0);
#endif	
#ifndef CLIENT_DLL	
	Vector vecDir = m_pPlayer->FireBullets4(vecSrc, gpGlobals->v_forward, flSpread, 4096, 2, BULLET_PLAYER_50AE, GetDamage(), 0.81, m_pPlayer->pev, TRUE, m_pPlayer->random_seed, 1);
	TraceResult tr;
	UTIL_TraceLine(vecSrc, vecDir, missile, ENT(m_pPlayer->pev), &tr);
	{
		if (tr.flFraction < 1.0)
		{
			CBaseEntity* pHit = CBaseEntity::Instance(tr.pHit);
			
			if (pHit->IsPlayer())
			{
				PLAYBACK_EVENT_FULL(FEV_GLOBAL, ENT(pHit->pev), PRECACHE_EVENT(1, "events/wpneffects.sc"), 0.0, pHit->pev->origin, (float*)&g_vecZero, 0.0, 0.0, 2, 0, TRUE, FALSE);
				if (g_pGameRules->PlayerRelationship(m_pPlayer, pHit) != GR_TEAMMATE)
					m_iCharging++;
			}
				PLAYBACK_EVENT_FULL(FEV_GLOBAL, ENT(pHit->pev), PRECACHE_EVENT(1, "events/wpneffects.sc"), 0.0, tr.vecEndPos, (float*)&g_vecZero, 0.0, 0.0, 2, 0, TRUE, FALSE);
		}
	}
	if (IsModeCEnabled(m_iCharging))
	{
		m_pPlayer->m_rgAmmo[m_iSecondaryAmmoType] = 1;
	}

	PLAYBACK_EVENT_FULL(0, m_pPlayer->edict(), m_usFireVoidpistolEx, 0, (float*)&g_vecZero, (float*)&g_vecZero, vecDir.x, vecDir.y, (int)(m_pPlayer->pev->punchangle.x * 100), 1, IsModeCEnabled(m_iCharging), FALSE);
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.0f;
#endif

	m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + flCycleTime;

#ifndef CLIENT_DLL
	if (!m_iClip && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
		m_pPlayer->SetSuitUpdate("!HEV_AMO0", FALSE, 0);
#endif
	
	m_pPlayer->pev->punchangle.x -= 2;
	ResetPlayerShieldAnim();
}

void CVoidpistolEx::Reload(void)
{
#ifndef CLIENT_DLL
	if (IsModeCEnabled(m_iCharging))
	{
		if (DefaultReload(VOIDPISTOL_MAX_CLIP, VOIDPISTOL_RELOADC, VOIDPISTOL_RELOAD_TIME))
		{
			m_pPlayer->SetAnimation(PLAYER_RELOAD);
			m_iShotsFired = 0;
		}
	}
	else
	{
		switch (m_iMode)
		{
		case VOIDPISTOL_MODEA:
		{
			if (DefaultReload(VOIDPISTOL_MAX_CLIP, VOIDPISTOL_RELOADA, VOIDPISTOL_RELOAD_TIME))
			{
				m_pPlayer->SetAnimation(PLAYER_RELOAD);
				m_iShotsFired = 0;
			}
			break;
		}
		case VOIDPISTOL_MODEB:
		{
			if (DefaultReload(VOIDPISTOL_MAX_CLIP, VOIDPISTOL_RELOADB, VOIDPISTOL_RELOAD_TIME))
			{
				m_pPlayer->SetAnimation(PLAYER_RELOAD);
				m_iShotsFired = 0;
				m_iMode = VOIDPISTOL_MODEA;
			}
			break;
		}
		}
	}
#endif
}

void CVoidpistolEx::WeaponIdle(void)
{
	ResetEmptySound();
	m_pPlayer->GetAutoaimVector(AUTOAIM_10DEGREES);
#ifndef CLIENT_DLL
	if (m_flTimeWeaponIdle > UTIL_WeaponTimeBase())
		return;

	if (IsModeCEnabled(m_iCharging))
	{
		SendWeaponAnim(VOIDPISTOL_IDLEC, UseDecrement() != FALSE);
	}
	else
	{
		switch (m_iMode)
		{
		case VOIDPISTOL_MODEA:
		{
			SendWeaponAnim(VOIDPISTOL_IDLEA, UseDecrement() != FALSE); break;
		}
		case VOIDPISTOL_MODEB:
		{
			SendWeaponAnim(VOIDPISTOL_IDLEB, UseDecrement() != FALSE); break;
		}
		}
	}
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 20;
#endif
	
}

float CVoidpistolEx::GetDamage() const
{
	float flDamage = 24.0f;
#ifndef CLIENT_DLL
	if (g_pModRunning->DamageTrack() == DT_ZB)
		flDamage = 260.0f;
		//flDamage = 225.0f;
	else if (g_pModRunning->DamageTrack() == DT_ZBS)
		flDamage = 590.0f;
#endif
	return flDamage;
}

float CVoidpistolEx::BlackholeDamage() const
{
	float flDamage = 10.0f;
#ifndef CLIENT_DLL
	if (g_pModRunning->DamageTrack() == DT_ZB)
		flDamage = 890.0f;
	//flDamage = 375.0f;
	else if (g_pModRunning->DamageTrack() == DT_ZBS)
		flDamage = 70.0f;
	//flDamage = 750.0f;
#endif
	return flDamage;
}

int CVoidpistolEx::ExtractAmmo(CBasePlayerWeapon* pWeapon)
{
	if (m_iDefaultAmmo2)
	{
		m_pPlayer->m_rgAmmo[m_iSecondaryAmmoType] = m_iDefaultAmmo2;
		m_iDefaultAmmo2 = 0;
	}
	return CBasePlayerWeapon::ExtractAmmo(pWeapon);
}
