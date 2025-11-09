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

#include "zb5_zclass_meatwall.h"

void CZombieClass_MeatWall::Precache()
{
	PRECACHE_SOUND("sound/zb5/meatwallzombie_hurt1.wav");
	PRECACHE_SOUND("sound/zb5/meatwallzombie_hurt2.wav");
	PRECACHE_SOUND("sound/zb5/meatwallzombie_death1.wav");
	PRECACHE_SOUND("sound/zb5/meatwallzombie_death2.wav");

	PRECACHE_MODEL("sprites/deathres_zombie.spr");
}

CZombieClass_MeatWall::CZombieClass_MeatWall(CBasePlayer *player, ZombieLevel iEvolutionLevel) : CBaseZombieClass_ZB2R(player, iEvolutionLevel)
{
	m_pZombieSkillR.reset(new CZombieSkill_ZombieCrazyR(m_pPlayer));

	const char *szModel = "zombi_meatwall";
	SET_CLIENT_KEY_VALUE(m_pPlayer->entindex(), GET_INFO_BUFFER(m_pPlayer->edict()), "model", szModel);

	static char szModelPath[64];
	Q_snprintf(szModelPath, sizeof(szModelPath), "models/player/%s/%s.mdl", szModel, szModel);
	m_pPlayer->SetNewPlayerModel(szModelPath);

	// set default property
	m_pPlayer->pev->health = m_pPlayer->pev->max_health = 50000.0f;
	m_pPlayer->pev->armortype = ARMOR_TYPE_HELMET;
	m_pPlayer->pev->armorvalue = 32000.0f;
	m_pPlayer->pev->gravity = 1.03f;
	m_pPlayer->pev->renderfx = kRenderFxNone;
	m_pPlayer->pev->rendermode = kRenderNormal;
	m_pPlayer->ResetMaxSpeed();
	m_pPlayer->GiveNamedItem("knife_zombi_meatwall");
	m_pPlayer->GiveNamedItem("weapon_zombibomb_meatwall");
	
	m_pPlayer->m_bIsZombieMeatWall = true;
}

void CZombieClass_MeatWall::InitHUD() const
{
	MESSAGE_BEGIN(MSG_ONE, gmsgZB2Msg, nullptr, m_pPlayer->pev);
	WRITE_BYTE(ZB2_MESSAGE_SKILL_INIT);
	WRITE_BYTE(ZOMBIE_CLASS_MEATWALL);
	WRITE_BYTE(ZOMBIE_SKILL_MW);
	WRITE_BYTE(ZOMBIE_SKILL_MW2);
	MESSAGE_END();
}

void CZombieClass_MeatWall::ResetMaxSpeed() const
{
	m_pPlayer->pev->maxspeed = 190;
}

float CZombieClass_MeatWall::AdjustDamageTaken(entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType) const
{
	flDamage *= 0.9;
	return CBaseZombieClass_ZB2R::AdjustDamageTaken(pevInflictor, pevAttacker, flDamage, bitsDamageType);
}

bool CZombieClass_MeatWall::ApplyKnockback(CBasePlayer *attacker, const KnockbackData &kbd)
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

void CZombieClass_MeatWall::Pain_Zombie(int m_LastHitGroup, bool HasArmour)
{
	switch (RANDOM_LONG(0, 1))
	{
		case 0: EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_AUTO, "zb5/meatwallzombie_hurt1.wav", VOL_NORM, ATTN_NORM); break;
		case 1: EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_AUTO, "zb5/meatwallzombie_hurt2.wav", VOL_NORM, ATTN_NORM); break;
		default:break;
	}
}

void CZombieClass_MeatWall::DeathSound_Zombie()
{
	switch (RANDOM_LONG(1, 2))
	{
		case 1: EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_AUTO, "zb5/meatwallzombie_death1.wav", VOL_NORM, ATTN_NORM); break;
		case 2: EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_AUTO, "zb5/meatwallzombie_death2.wav", VOL_NORM, ATTN_NORM); break;
		default:break;
	}
}

void CZombieClass_MeatWall::OnThink()
{
	Zombie_HealthRecoveryThink();
	return CZombieClass_MeatWall::OnThink();
}


void CZombieClass_MeatWall::Zombie_HealthRecoveryThink()
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
			CLIENT_COMMAND(m_pPlayer->edict(), "spk zb5/zombi_heal_meatwall.wav\n");

			MESSAGE_BEGIN(MSG_ONE, gmsgZB2Msg, nullptr, m_pPlayer->pev);
			WRITE_BYTE(ZB2_MESSAGE_HEALTH_RECOVERY);
			MESSAGE_END();
		}
	}
}
