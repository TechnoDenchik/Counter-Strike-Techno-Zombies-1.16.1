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

#include "zb2_zclass_heal.h"

void CZombieClass_Heal::Precache()
{
	PRECACHE_SOUND("sound/zb3/zombi_hurt_01.wav");
	PRECACHE_SOUND("sound/zb3/zombi_hurt_02.wav");
	PRECACHE_SOUND("sound/zb3/zombi_death_1.wav");
	PRECACHE_SOUND("sound/zb3/zombi_death_2.wav");
}

CZombieClass_Heal::CZombieClass_Heal(CBasePlayer *player, ZombieLevel iEvolutionLevel) : CBaseZombieClass_ZB2(player, iEvolutionLevel)
{
	Precache();
	m_pZombieSkill.reset(new CZombieSkill_ZombieCrazy(m_pPlayer));

	const char *szModel = iEvolutionLevel ? "heal_zombi_origin" : "heal_zombi_host";
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
	m_pPlayer->GiveNamedItem("Knife_Zombi_heal");
	m_pPlayer->m_bIsZombieHeal = true;
}

void CZombieClass_Heal::InitHUD() const
{
	MESSAGE_BEGIN(MSG_ONE, gmsgZB2Msg, nullptr, m_pPlayer->pev);
	WRITE_BYTE(ZB2_MESSAGE_SKILL_INIT);
	WRITE_BYTE(ZOMBIE_CLASS_HEAL);
	WRITE_BYTE(ZOMBIE_SKILL_HEAL);
	MESSAGE_END();
}

void CZombieClass_Heal::ResetMaxSpeed() const
{
	m_pPlayer->pev->maxspeed = 310;
}

float CZombieClass_Heal::AdjustDamageTaken(entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType) const
{
	flDamage *= 0.9;
	return CBaseZombieClass_ZB2::AdjustDamageTaken(pevInflictor, pevAttacker, flDamage, bitsDamageType);
}

bool CZombieClass_Heal::ApplyKnockback(CBasePlayer *attacker, const KnockbackData &kbd)
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

void CZombieClass_Heal::Pain_Zombie(int m_LastHitGroup, bool HasArmour)
{
	switch (RANDOM_LONG(0, 1))
	{
		case 0: EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_AUTO, "zb3/zombi_hurt_01.wav", VOL_NORM, ATTN_NORM); break;
		case 1: EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_AUTO, "zb3/zombi_hurt_02.wav", VOL_NORM, ATTN_NORM); break;
		default:break;
	}
}

void CZombieClass_Heal::DeathSound_Zombie()
{
	switch (RANDOM_LONG(1, 2))
	{
		case 1: EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_AUTO, "zb3/zombi_death_1.wav", VOL_NORM, ATTN_NORM); break;
		case 2: EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_AUTO, "zb3/zombi_death_2.wav", VOL_NORM, ATTN_NORM); break;
		default:break;
	}
}

void CZombieClass_Heal::OnThink()
{
	Zombie_HealthRecoveryThink();
	return CZombieClass_Heal::OnThink();
}

void CZombieClass_Heal::Zombie_HealthRecoveryThink()
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