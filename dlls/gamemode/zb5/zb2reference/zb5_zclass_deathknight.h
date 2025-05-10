/* =================================================================================== *
			 * =================== TechnoSoftware =================== *
 * =================================================================================== */

#ifndef PROJECT_ZB2_ZCLASS_DEATHKNIGHT_H
#define PROJECT_ZB2_ZCLASS_DEATHKNIGHT_H

#include "gamemode/zb5/zb2reference/zb2_zclassreference.h"

#include "gamemode/zb5/mod_zb1reference.h"

class CZombieClass_DeathKnight : public CBaseZombieClass_ZB2R
{
public:
	explicit CZombieClass_DeathKnight(CBasePlayer *player, ZombieLevel lv);
	void InitHUD() const override;
	void ResetMaxSpeed() const override;
	float AdjustDamageTaken(entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType) const override;
	bool ApplyKnockback(CBasePlayer *attacker, const KnockbackData & kbd) override;
	void Pain_Zombie(int m_LastHitGroup, bool HasArmour) override;
	void DeathSound_Zombie() override;
	virtual void Zombie_HealthRecoveryThink();
	virtual void Precache();
	void OnThink() override;
	std::shared_ptr<IZombieModeCharacter_ZB2_ExtraR> m_pCharacter_ZB2;
	float m_flTimeNextZombieHealthRecovery;
	entvars_t* pev;
};

#endif
