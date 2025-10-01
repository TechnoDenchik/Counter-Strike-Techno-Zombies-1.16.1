/* =================================================================================== *
			 * =================== TechnoSoftware =================== *
 * =================================================================================== */

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"

#include "gamemode/zb2/zb2_const.h"
#include "gamemode/zb5/zb2reference/zb2_skillreference.h"
#include "gamemode/zb5/zb2reference/zb2_zclassreference.h"

#include "zb5_zclass_deathknight.h"

void CZombieClass_DeathKnight::Precache()
{
	PRECACHE_SOUND("sound/zb5/deathknight_hurt1.wav");
	PRECACHE_SOUND("sound/zb5/deathknight_hurt2.wav");
	PRECACHE_SOUND("sound/zb5/deathknight_death1.wav");
	PRECACHE_SOUND("sound/zb5/deathknight_death2.wav");

	PRECACHE_MODEL("sprites/deathres_zombie.spr");
}

CZombieClass_DeathKnight::CZombieClass_DeathKnight(CBasePlayer *player, ZombieLevel iEvolutionLevel) : CBaseZombieClass_ZB2R(player, iEvolutionLevel)
{
	m_pZombieSkillR.reset(new CZombieSkill_ZombieCrazyR(m_pPlayer));

	const char *szModel = "zombi_deathknight";
	SET_CLIENT_KEY_VALUE(m_pPlayer->entindex(), GET_INFO_BUFFER(m_pPlayer->edict()), "model", szModel);

	static char szModelPath[64];
	Q_snprintf(szModelPath, sizeof(szModelPath), "models/player/%s/%s.mdl", szModel, szModel);
	m_pPlayer->SetNewPlayerModel(szModelPath);

	m_pPlayer->pev->health = m_pPlayer->pev->max_health = 31800.0f;
	m_pPlayer->pev->armortype = ARMOR_TYPE_HELMET;
	m_pPlayer->pev->armorvalue = 30000;
	m_pPlayer->pev->gravity = 0.78f;
	m_pPlayer->pev->renderfx = kRenderFxNone;
	m_pPlayer->pev->rendermode = kRenderNormal;
	m_pPlayer->ResetMaxSpeed();
	m_pPlayer->GiveNamedItem("knife_zombi_deathknight");
	m_pPlayer->GiveNamedItem("weapon_zombibombz");
	
	m_pPlayer->m_bIsZombieDeathKnight = true;
}

void CZombieClass_DeathKnight::InitHUD() const
{
	MESSAGE_BEGIN(MSG_ONE, gmsgZB2Msg, nullptr, m_pPlayer->pev);
	WRITE_BYTE(ZB2_MESSAGE_SKILL_INIT);
	WRITE_BYTE(ZOMBIE_CLASS_DEATHKNIGHT);
	WRITE_BYTE(ZOMBIE_SKILL_DK);
	WRITE_BYTE(ZOMBIE_SKILL_DK2);
	MESSAGE_END();
}

void CZombieClass_DeathKnight::ResetMaxSpeed() const
{
	m_pPlayer->pev->maxspeed = 280;
}

float CZombieClass_DeathKnight::AdjustDamageTaken(entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType) const
{
	flDamage *= 0.9;
	return CBaseZombieClass_ZB2R::AdjustDamageTaken(pevInflictor, pevAttacker, flDamage, bitsDamageType);
}

bool CZombieClass_DeathKnight::ApplyKnockback(CBasePlayer *attacker, const KnockbackData &kbd)
{
	// knife knockback
	if(attacker->m_pActiveItem && attacker->m_pActiveItem->iItemSlot() == KNIFE_SLOT)
		return CBaseZombieClass_ZB2R::ApplyKnockback(attacker, { 700, 1600, 1300, 400, 1.0f });

	if(m_pPlayer->m_iZombieLevel == ZOMBIE_LEVEL_HOST)
		return CBaseZombieClass_ZB2R::ApplyKnockback(attacker, kbd);

	return CBaseZombieClass_ZB2R::ApplyKnockback(attacker, {
			kbd.flOnGround * 0.7f,
			kbd.flNotOnGround * 0.75f,
			kbd.flFlying * 0.75f,
			kbd.flDucking * 0.7f,
			kbd.flVelocityModifier,
	});
}

void CZombieClass_DeathKnight::Pain_Zombie(int m_LastHitGroup, bool HasArmour)
{
	switch (RANDOM_LONG(0, 1))
	{
		case 0: EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_AUTO, "zb5/deathknight_hurt1.wav", VOL_NORM, ATTN_NORM); break;
		case 1: EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_AUTO, "zb5/deathknight_hurt2.wav", VOL_NORM, ATTN_NORM); break;
		default:break;
	}
}

void CZombieClass_DeathKnight::DeathSound_Zombie()
{
	switch (RANDOM_LONG(1, 2))
	{
		case 1: EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_AUTO, "zb5/deathknight_death1.wav", VOL_NORM, ATTN_NORM); break;
		case 2: EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_AUTO, "zb5/deathknight_death2.wav", VOL_NORM, ATTN_NORM); break;
		default:break;
	}
	MESSAGE_BEGIN(MSG_BROADCAST, SVC_TEMPENTITY);
	WRITE_BYTE(TE_EXPLOSION);
	WRITE_COORD(m_pPlayer->pev->origin.x);
	WRITE_COORD(m_pPlayer->pev->origin.y);
	WRITE_COORD(m_pPlayer->pev->origin.z);
	WRITE_SHORT(MODEL_INDEX("sprites/deathres_zombie.spr"));
	WRITE_BYTE(8);
	WRITE_BYTE(40);
	WRITE_BYTE(TE_EXPLFLAG_NOPARTICLES | TE_EXPLFLAG_NODLIGHTS | TE_EXPLFLAG_NOSOUND);
	MESSAGE_END();
}

void CZombieClass_DeathKnight::OnThink()
{
	Zombie_HealthRecoveryThink();
	return CZombieClass_DeathKnight::OnThink();
}


void CZombieClass_DeathKnight::Zombie_HealthRecoveryThink()
{
	if (!m_pPlayer->IsAlive() || !m_pPlayer->m_bIsZombie)
		return;

	if (m_pPlayer->pev->button & (IN_FORWARD | IN_BACK | IN_MOVELEFT | IN_MOVERIGHT))
	{
		m_flTimeNextZombieHealthRecovery = gpGlobals->time + 3.0f;
		return;
	}

	// cannot recover during using zombie skill.
	const float flRecoverValue = m_pCharacter_ZB2->HealthRecoveryAmount();
	if (flRecoverValue <= 0.0f)
		return;

	if (gpGlobals->time > m_flTimeNextZombieHealthRecovery)
	{
		if (m_pPlayer->pev->max_health != m_pPlayer->pev->health)
		{

			m_flTimeNextZombieHealthRecovery = gpGlobals->time + 1.0f;
			m_pPlayer->pev->health = std::min(m_pPlayer->pev->max_health, m_pPlayer->pev->health + flRecoverValue);

			// effects
			CLIENT_COMMAND(m_pPlayer->edict(), "spk zb5/zombi_heal.wav\n");

			MESSAGE_BEGIN(MSG_ONE, gmsgZB2Msg, nullptr, m_pPlayer->pev);
			WRITE_BYTE(ZB2_MESSAGE_HEALTH_RECOVERY);
			MESSAGE_END();
		}
	}
}
