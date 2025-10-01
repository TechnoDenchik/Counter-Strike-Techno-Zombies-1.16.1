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

#include "gamemode/zb5/zb2reference/zb2_skillreference.h"
#include "gamemode/zb5/zb2reference/zb2_zclassreference.h"

#include "zb3_zclass_akshaR.h"

void CZombieClass_AkshaR::Precache()
{
	PRECACHE_SOUND("sound/zb3/akshazombie_death1.wav");
	PRECACHE_SOUND("sound/zb3/akshazombie_death2.wav");
	PRECACHE_SOUND("sound/zb3/akshazombie_hurt1.wav");
	PRECACHE_SOUND("sound/zb3/akshazombie_hurt2.wav");
}

CZombieClass_AkshaR::CZombieClass_AkshaR(CBasePlayer *player, ZombieLevel iEvolutionLevel) : CBaseZombieClass_ZB2R(player, iEvolutionLevel)
{
	Precache();
	m_pZombieSkillR.reset(new CZombieSkill_ZombieCrazyR(m_pPlayer));

	const char *szModel = iEvolutionLevel ? "akshazombi_origin" : "akshazombi_host";
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
	m_pPlayer->GiveNamedItem("Knife_Zombi_aksha");
	m_pPlayer->GiveNamedItem("weapon_zombibombz");
	m_pPlayer->ResetMaxSpeed();
	m_pPlayer->m_bIsZombieAksha = true;
}

void CZombieClass_AkshaR::InitHUD() const
{
	MESSAGE_BEGIN(MSG_ONE, gmsgZB2Msg, nullptr, m_pPlayer->pev);
	WRITE_BYTE(ZB2_MESSAGE_SKILL_INIT);
	WRITE_BYTE(ZOMBIE_CLASS_AKSHA);
	WRITE_BYTE(ZOMBIE_SKILL_AKSHA);
	WRITE_BYTE(ZOMBIE_SKILL_AKSHA3);
	MESSAGE_END();
}

void CZombieClass_AkshaR::ResetMaxSpeed() const
{
	m_pPlayer->pev->maxspeed = 310;
}

float CZombieClass_AkshaR::AdjustDamageTaken(entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType) const
{
	flDamage *= 0.9;
	return CBaseZombieClass_ZB2R::AdjustDamageTaken(pevInflictor, pevAttacker, flDamage, bitsDamageType);
}

bool CZombieClass_AkshaR::ApplyKnockback(CBasePlayer *attacker, const KnockbackData &kbd)
{
	if(attacker->m_pActiveItem && attacker->m_pActiveItem->iItemSlot() == KNIFE_SLOT)
		return CBaseZombieClass_ZB2R::ApplyKnockback(attacker, { 550, 750, 400, 300, 0.85f });

	if(m_pPlayer->m_iZombieLevel == ZOMBIE_LEVEL_HOST)
	{
		return CBaseZombieClass_ZB2R::ApplyKnockback(attacker, {
			kbd.flOnGround * 3.f,
			kbd.flNotOnGround * 2.f,
			kbd.flFlying * 2.f,
			kbd.flDucking * 3.f,
			kbd.flVelocityModifier * 0.9f,

		});
	}

	return CBaseZombieClass_ZB2R::ApplyKnockback(attacker, {
			kbd.flOnGround * 2.4f,
			kbd.flNotOnGround * 1.7f,
			kbd.flFlying * 1.7f,
			kbd.flDucking * 2.4f,
			kbd.flVelocityModifier * 0.9f,
	});
}

void CZombieClass_AkshaR::Pain_Zombie(int m_LastHitGroup, bool HasArmour)
{
	switch (RANDOM_LONG(1,2))
	{
	case 1:
		EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_AUTO, "zb3/akshazombie_hurt1.wav", VOL_NORM, ATTN_NORM);
		break;
	case 2:
		EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_AUTO, "zb3/akshazombie_hurt1.wav", VOL_NORM, ATTN_NORM);
		break;
	}
}

void CZombieClass_AkshaR::DeathSound_Zombie()
{
	switch (RANDOM_LONG(1, 2))
	{
	case 1:
		EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_AUTO, "zb3/akshazombie_death1.wav", VOL_NORM, ATTN_NORM);
		break;
	case 2:
		EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_AUTO, "zb3/akshazombie_death2.wav", VOL_NORM, ATTN_NORM);
		break;
	}
}

void CZombieClass_AkshaR::OnThink()
{
	Zombie_HealthRecoveryThink();
	return CZombieClass_AkshaR::OnThink();
}

void CZombieClass_AkshaR::Zombie_HealthRecoveryThink()
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