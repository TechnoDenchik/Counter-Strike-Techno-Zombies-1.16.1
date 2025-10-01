/* =================================================================================== *
			 * =================== TechnoSoftware =================== *
 * =================================================================================== */

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"

#include "gamemode/zb2/zb2_const.h"
#include "gamemode/zb2/zb2_zclass.h"
#include "gamemode/zb2/zb2_skill.h"

#include "zb3_zclass_china.h"

void CZombieClass_China::Precache()
{
	PRECACHE_SOUND("sound/zb3/zombi_chinese_death.wav");
	PRECACHE_SOUND("sound/zb3/zombi_chinese_hurt.wav");
	PRECACHE_SOUND("sound/zb3/zombi_chinese_jump.wav");
}

CZombieClass_China::CZombieClass_China(CBasePlayer *player, ZombieLevel iEvolutionLevel) : CBaseZombieClass_ZB2(player, iEvolutionLevel)
{
	Precache();
	m_pZombieSkill.reset(new CZombieSkill_ZombieCrazy(m_pPlayer));

	const char *szModel = iEvolutionLevel ? "china_zombi_origin" : "china_zombi_host";
	SET_CLIENT_KEY_VALUE(m_pPlayer->entindex(), GET_INFO_BUFFER(m_pPlayer->edict()), "model", szModel);

	static char szModelPath[64];
	Q_snprintf(szModelPath, sizeof(szModelPath), "models/player/%s/%s.mdl", szModel, szModel);
	m_pPlayer->SetNewPlayerModel(szModelPath);

	m_pPlayer->pev->health = m_pPlayer->pev->max_health = 25000;
	m_pPlayer->pev->armortype = ARMOR_TYPE_HELMET;
	m_pPlayer->pev->armorvalue = 5500;
	m_pPlayer->pev->gravity = 0.98f;
	m_pPlayer->pev->renderfx = kRenderFxNone;
	m_pPlayer->pev->rendermode = kRenderNormal;
	m_pPlayer->ResetMaxSpeed();
	m_pPlayer->GiveNamedItem("Knife_Zombi_china");
	m_pPlayer->GiveNamedItem("weapon_zombibombz");
	m_pPlayer->m_bIsZombieChina = true;
}

void CZombieClass_China::InitHUD() const
{
	MESSAGE_BEGIN(MSG_ONE, gmsgZB2Msg, nullptr, m_pPlayer->pev);
	WRITE_BYTE(ZB2_MESSAGE_SKILL_INIT);
	WRITE_BYTE(ZOMBIE_CLASS_CHINA);
	WRITE_BYTE(ZOMBIE_SKILL_CHINA);
	WRITE_BYTE(ZOMBIE_SKILL_CHINA2);
	MESSAGE_END();
}

void CZombieClass_China::ResetMaxSpeed() const
{
	m_pPlayer->pev->maxspeed = 310;
}

float CZombieClass_China::AdjustDamageTaken(entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType) const
{
	flDamage *= 0.9;
	return CBaseZombieClass_ZB2::AdjustDamageTaken(pevInflictor, pevAttacker, flDamage, bitsDamageType);
}

bool CZombieClass_China::ApplyKnockback(CBasePlayer *attacker, const KnockbackData &kbd)
{
	if(attacker->m_pActiveItem && attacker->m_pActiveItem->iItemSlot() == KNIFE_SLOT)
		return CBaseZombieClass_ZB2::ApplyKnockback(attacker, { 550, 750, 400, 300, 0.85f });

	if(m_pPlayer->m_iZombieLevel == ZOMBIE_LEVEL_HOST)
	{
		return CBaseZombieClass_ZB2::ApplyKnockback(attacker, {
			kbd.flOnGround * 3.f,
			kbd.flNotOnGround * 2.f,
			kbd.flFlying * 2.f,
			kbd.flDucking * 3.f,
			kbd.flVelocityModifier * 0.9f,

		});
	}

	return CBaseZombieClass_ZB2::ApplyKnockback(attacker, {
			kbd.flOnGround * 2.4f,
			kbd.flNotOnGround * 1.7f,
			kbd.flFlying * 1.7f,
			kbd.flDucking * 2.4f,
			kbd.flVelocityModifier * 0.9f,
	});
}

void CZombieClass_China::Pain_Zombie(int m_LastHitGroup, bool HasArmour)
{
	EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_STATIC, "zb3/zombi_chinese_hurt.wav", VOL_NORM, ATTN_NORM);
}

void CZombieClass_China::DeathSound_Zombie()
{
	EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_STATIC, "zb3/zombi_chinese_death.wav", VOL_NORM, ATTN_NORM);
}

void CZombieClass_China::OnThink()
{
	Zombie_HealthRecoveryThink();
	return CZombieClass_China::OnThink();
}

void CZombieClass_China::Zombie_HealthRecoveryThink()
{
	if (!m_pPlayer->IsAlive() || !m_pPlayer->m_bIsZombie)
		return;

	if (m_pPlayer->pev->button & (IN_FORWARD | IN_BACK | IN_MOVELEFT | IN_MOVERIGHT))
	{
		m_flTimeNextZombieHealthRecovery = gpGlobals->time + 3.0f;
		return;
	}

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
			CLIENT_COMMAND(m_pPlayer->edict(), "spk zb3/zombi_heal.wav\n");

			MESSAGE_BEGIN(MSG_ONE, gmsgZB2Msg, nullptr, m_pPlayer->pev);
			WRITE_BYTE(ZB2_MESSAGE_HEALTH_RECOVERY);
			MESSAGE_END();
		}
	}
}