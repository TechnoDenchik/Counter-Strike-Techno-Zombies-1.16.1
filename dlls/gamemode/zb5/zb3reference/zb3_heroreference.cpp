/* =================================================================================== *
			 * =================== TechnoSoftware =================== *
 * =================================================================================== */

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"

#include "zb3_heroreference.h"
#include "gamemode/zb3/zb3_const.h"
#include "gamemode/interface/interface_const.h"
#include "gamemode/zb2/zb2_const.h"
#include "gamemode/zb5/zb2reference/zb2_zclassreference.h"
#include "gamemode/zb5/zb2reference/zb2_skillreference.h"

constexpr bool ENABLE_SPRINT = true;
constexpr bool ENABLE_HEADSHOT = true;
constexpr bool ENABLE_KNIFE2X = true;

void HeroSkill_PrecacheR()
{
	PRECACHE_SOUND("zb3/speedup.wav");
	PRECACHE_SOUND("zb3/human_breath_male.wav");
	PRECACHE_SOUND("zb3/speedup_heartbeat.wav");
	PRECACHE_MODEL("sprites/zb5_death_effect.spr");
	PRECACHE_MODEL("sprites/zb_meleeup.spr");
	PRECACHE_MODEL("sprites/zb_skill_headshot.spr");
	PRECACHE_MODEL("sprites/zb5_itemdrop_effect.spr");
}

class CHero_ZB1R::impl_t : public BasePlayerExtra
{
public:
	explicit impl_t(CBasePlayer* player) : BasePlayerExtra(player), spd(player), hs(player), k2x(player) {}
	SprintSkillR spd;
	HeadShotSkillR hs;
	Knife2xSkillR k2x;
};

CHero_ZB1R::CHero_ZB1R(CBasePlayer* player) : BasePlayerExtra(player), pimpl(std::unique_ptr<impl_t>(new impl_t(player)))
{
	HeroSkill_PrecacheR();

	m_pPlayer->m_bIsZombie = false;
	m_pPlayer->m_bNotKilled = false;
	m_pPlayer->m_bIsVIP = true;
	m_pPlayer->pev->body = 0;

	const char* szModel = "hero";
	SET_CLIENT_KEY_VALUE(m_pPlayer->entindex(), GET_INFO_BUFFER(m_pPlayer->edict()), "model", const_cast<char*>(szModel));

	static char szModelPath[64];
	Q_snprintf(szModelPath, sizeof(szModelPath), "models/player/%s/%s.mdl", szModel, szModel);
	m_pPlayer->SetNewPlayerModel(szModelPath);

	UTIL_LogPrintf("\"%s<%i><%s><CT>\" triggered \"Became_Hero\"\n", STRING(m_pPlayer->pev->netname), GETPLAYERUSERID(m_pPlayer->edict()), GETPLAYERAUTHID(m_pPlayer->edict()));

	// remove guns & give nvg
	m_pPlayer->m_bNightVisionOn = false;

	// set default property
	m_pPlayer->pev->health = m_pPlayer->pev->max_health = 3000;
	m_pPlayer->pev->armortype = ARMOR_TYPE_HELMET;
	m_pPlayer->pev->armorvalue = 2000;
	m_pPlayer->pev->gravity = 0.78f;
	m_pPlayer->ResetMaxSpeed();
}

void CHero_ZB1R::ActivateSkill(ZombieSkillSlot which)
{
	if (which == SKILL_SLOT_1 && ENABLE_SPRINT)
		pimpl->spd.Activate();
	else if (which == SKILL_SLOT_2 && ENABLE_HEADSHOT)
		pimpl->hs.Activate();
	else if (which == SKILL_SLOT_3 && ENABLE_KNIFE2X)
		pimpl->k2x.Activate();
}

void CHero_ZB1R::ResetMaxSpeed() const
{
	m_pPlayer->pev->maxspeed = 360;
}

void CHero_ZB1R::InitHUD() const
{
	MESSAGE_BEGIN(MSG_ONE, gmsgZB2Msg, NULL, m_pPlayer->pev);
	WRITE_BYTE(ZB2_MESSAGE_SKILL_INIT);
	WRITE_BYTE(ZOMBIE_CLASS_HUMAN);
	WRITE_BYTE(ENABLE_SPRINT ? ZOMBIE_SKILL_SPRINT : ZOMBIE_SKILL_EMPTY);
	WRITE_BYTE(ENABLE_HEADSHOT ? ZOMBIE_SKILL_HEADSHOT : ZOMBIE_SKILL_EMPTY);
	MESSAGE_END();
}

void CHero_ZB1R::Think()
{
	if (ENABLE_SPRINT)
		pimpl->spd.Think();

	if (ENABLE_HEADSHOT)
		pimpl->hs.Think();

	if (ENABLE_KNIFE2X)
		pimpl->k2x.Think();
}

void CHero_ZB1R::DeathSound_Hero()
{
	switch (RANDOM_LONG(1, 3))
	{
	case 1: EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_VOICE, "player/die1.wav", VOL_NORM, ATTN_NORM); break;
	case 2: EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_VOICE, "player/die2.wav", VOL_NORM, ATTN_NORM); break;
	case 3: EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_VOICE, "player/die3.wav", VOL_NORM, ATTN_NORM); break;

	default:break;
	}
}

void CHero_ZB1R::Pain_Hero(int m_LastHitGroup, bool HasArmour)
{
	switch (RANDOM_LONG(0, 3))
	{
	case 0: EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_VOICE, "player/bhit_flesh-1.wav", VOL_NORM, ATTN_NORM); break;
	case 1: EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_VOICE, "player/bhit_flesh-2.wav", VOL_NORM, ATTN_NORM); break;
	case 2: EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_VOICE, "player/bhit_flesh-3.wav", VOL_NORM, ATTN_NORM); break;
	case 3: EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_VOICE, "player/bhit_kevlar-1.wav", VOL_NORM, ATTN_NORM); break;

	default:break;
	}
}

ZombieSkillStatus CHero_ZB1R::GetSkillStatus(ZombieSkillSlot which) const
{
	if (which == SKILL_SLOT_1 && ENABLE_SPRINT)
		return pimpl->spd.GetStatus();
	if (which == SKILL_SLOT_2 && ENABLE_HEADSHOT)
		return pimpl->hs.GetStatus();
	if (which == SKILL_SLOT_3 && ENABLE_KNIFE2X)
		return pimpl->k2x.GetStatus();
	return SKILL_STATUS_USED;
}