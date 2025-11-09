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

#include "zb2_zclass_tankreference.h"
#include "zb2_zclass_speedreference.h"
#include "zb2_zclass_heavyreference.h"
#include "zb2_zclass_healreference.h"
#include "zb2_zclass_pcreference.h"
#include "zb2_zclass_deimosreference.h"
#include "zb2_zclass_ganimedreference.h"
#include "zb2_zclass_bancheereference.h"
#include "zb2_zclass_stamperreference.h"

#include "newclass/zb3_zclass_akshaR.h"
#include "newclass/zb3_zclass_boomerR.h"
#include "newclass/zb3_zclass_boosterR.h"
#include "newclass/zb3_zclass_chinaR.h"
#include "newclass/zb3_zclass_flyR.h"
#include "newclass/zb3_zclass_residentR.h"

#include "zb5_zclass_meatwall.h"
#include "zb5_zclass_deathknight.h"
#include "zb5_zclass_spider.h"

#include <random>

template<class T>
std::shared_ptr<CBaseZombieClass_ZB2R> MakeZombieClass(CBasePlayer *player, ZombieLevel lv)
{
	return std::make_shared<T>(player, lv);
}

const std::pair<const char *, std::shared_ptr<CBaseZombieClass_ZB2R>(*)(CBasePlayer *, ZombieLevel)> g_FindList[] = 
{
	{ "tank", MakeZombieClass <CZombieClass_DefaultR> },
	{ "speed", MakeZombieClass <CZombieClass_SpeedR> },
	{ "heavy", MakeZombieClass <CZombieClass_HeavyR> },
	{ "heal", MakeZombieClass <CZombieClass_HealR> },
	{ "psycho", MakeZombieClass <CZombieClass_PsychoR> },
	{ "deimos", MakeZombieClass <CZombieClass_DeimosR> },
	{ "ganimed", MakeZombieClass <CZombieClass_GanimedR> },
	{ "banchee", MakeZombieClass <CZombieClass_BancheeR> },
	{ "stamper", MakeZombieClass <CZombieClass_StamperR> },
	{ "meatwall", MakeZombieClass <CZombieClass_MeatWall> },
	{ "deathknight", MakeZombieClass <CZombieClass_DeathKnight> },
	{ "spider", MakeZombieClass <CZombieClass_Spider> },
	{ "aksha", MakeZombieClass <CZombieClass_AkshaR> },
	{ "boomer", MakeZombieClass <CZombieClass_BoomerR> },
	{ "booster", MakeZombieClass <CZombieClass_BoosterR> },
	{ "china", MakeZombieClass <CZombieClass_ChinaR> },
	{ "fly", MakeZombieClass <CZombieClass_FlyR> },
	{ "resident", MakeZombieClass <CZombieClass_ResidentR> },
};
constexpr auto NUM_ZCLASSES = std::extent<decltype(g_FindList)>::value;

std::shared_ptr<CBaseZombieClass_ZB2R> ZombieClassFactoryR(CBasePlayer* player, ZombieLevel lv, const char* name)
{
	if (name == nullptr)
		return g_FindList[0].second(player, lv);

	if (!stricmp(name, "random"))
	{
		std::random_device rd;
		const std::size_t N = std::uniform_int_distribution<size_t>(0, NUM_ZCLASSES - 1)(rd);
		return g_FindList[N].second(player, lv);
	}
	else if (!stricmp(name, "tank"))
	{
		return g_FindList[0].second(player, lv);
	}
	else if (!stricmp(name, "speed"))
	{
		return g_FindList[1].second(player, lv);		
	}
	else if (!stricmp(name, "heavy"))
	{
		return g_FindList[2].second(player, lv);		
	}
	else if (!stricmp(name, "heal"))
	{
		return g_FindList[3].second(player, lv);		
	}
	else if (!stricmp(name, "psycho"))
	{
		return g_FindList[4].second(player, lv);		
	}
	else if (!stricmp(name, "deimos"))
	{
		return g_FindList[5].second(player, lv);		
	}
	else if (!stricmp(name, "ganimed"))
	{
		return g_FindList[6].second(player, lv);		
	}
	else if (!stricmp(name, "banchee"))
	{
		return g_FindList[7].second(player, lv);		
	}
	else if (!stricmp(name, "stamper"))
	{
		return g_FindList[8].second(player, lv);		
	}
	else if (!stricmp(name, "meatwall"))
	{
		return g_FindList[9].second(player, lv);
	}
	else if (!stricmp(name, "deathknight"))
	{
		return g_FindList[10].second(player, lv);
	}
	else if (!stricmp(name, "spider"))
	{
		return g_FindList[11].second(player, lv);
	}
	else if (!stricmp(name, "aksha"))
	{
		return g_FindList[12].second(player, lv);
	}
	else if (!stricmp(name, "boomer"))
	{
		return g_FindList[13].second(player, lv);
	}
	else if (!stricmp(name, "booster"))
	{
		return g_FindList[14].second(player, lv);
	}
	else if (!stricmp(name, "china"))
	{
		return g_FindList[15].second(player, lv);
	}
	else if (!stricmp(name, "fly"))
	{
		return g_FindList[16].second(player, lv);
	}
	else if (!stricmp(name, "resident"))
	{
		return g_FindList[17].second(player, lv);
	}

	//return g_FindList[0].second(player, lv);
}

void CBaseZombieClass_ZB2R::InitHUD() const
{
	MESSAGE_BEGIN(MSG_ONE, gmsgZB2Msg, nullptr, m_pPlayer->pev);
	WRITE_BYTE(ZB2_MESSAGE_SKILL_INIT);
	MESSAGE_END();
}

void CBaseZombieClass_ZB2R::InitHUD2() const
{
	MESSAGE_BEGIN(MSG_ONE, gmsgZB2Msg, nullptr, m_pPlayer->pev);
	WRITE_BYTE(ZB2_MESSAGE_SKILL_INIT);
	MESSAGE_END();
}

void CBaseZombieClass_ZB2R::ActivateSkill(ZombieSkillSlot which)
{
	if (m_pZombieSkillR && which == SKILL_SLOT_1)
		m_pZombieSkillR->Activate();
}

void CBaseZombieClass_ZB2R::Think()
{
	if (m_pZombieSkillR)
		m_pZombieSkillR->Think();
	return CZombie_ZB1R::Think();
}

void CBaseZombieClass_ZB2R::ResetMaxSpeed() const
{
	CZombie_ZB1R::ResetMaxSpeed();
	if (m_pZombieSkillR)
		m_pZombieSkillR->ResetMaxSpeed(); // cans replace result of CZombie_ZB1::ResetMaxSpeed()
}

bool CBaseZombieClass_ZB2R::ApplyKnockback(CBasePlayer *attacker, const KnockbackData &kbd)
{
	// TODO : knockback adjustment by skill
	return CZombie_ZB1R::ApplyKnockback(attacker, kbd);
}

float CBaseZombieClass_ZB2R::AdjustDamageTaken(entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType) const
{
	if (m_pZombieSkillR)
		flDamage *= m_pZombieSkillR->GetDamageRatio();
	return CZombie_ZB1R::AdjustDamageTaken(pevInflictor, pevAttacker, flDamage, bitsDamageType);
}

ZombieSkillStatus CBaseZombieClass_ZB2R::GetSkillStatus(ZombieSkillSlot which) const
{
	return m_pZombieSkillR && which == SKILL_SLOT_1 ? m_pZombieSkillR->GetStatus() : SKILL_STATUS_USED;
}

float CBaseZombieClass_ZB2R::HealthRecoveryAmount() const
{
	return m_pZombieSkillR->GetStatus() == SKILL_STATUS_USING ? 0 : m_pPlayer->m_iZombieLevel == ZOMBIE_LEVEL_HOST ? 200 : 500;
}

CBaseZombieClass_ZB2R::~CBaseZombieClass_ZB2R() = default;