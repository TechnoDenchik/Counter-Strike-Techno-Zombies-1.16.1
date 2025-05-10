/* =================================================================================== *
			 * =================== TechnoSoftware =================== *
 * =================================================================================== */

#ifndef PROJECT_ZB3_HEROR_H
#define PROJECT_ZB3_HEROR_H

#include "player/player_mod_strategy.h"

class IHeroModeCharacterR
{
public:
	virtual ~IHeroModeCharacterR() = default;

	virtual void Think() = 0;
	virtual void ResetMaxSpeed() const = 0;
	virtual bool ApplyKnockback(CBasePlayer *attacker, const KnockbackData & kbd) = 0;
	virtual float AdjustDamageTaken(entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType) const = 0;
	virtual void Pain_Hero(int m_LastHitGroup, bool HasArmour) = 0;
	virtual void DeathSound_Hero() = 0;
};


class CHero_ZB1R : public BasePlayerExtra, public IHeroModeCharacterR
{
public:
	explicit CHero_ZB1R(CBasePlayer *player); // player_zombie.cpp
	void Think() override {}
	bool ApplyKnockback(CBasePlayer *attacker, const KnockbackData & data) override { ApplyKnockbackData(m_pPlayer, m_pPlayer->pev->origin - attacker->pev->origin, data); return true; }
	float AdjustDamageTaken(entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType) const override { return flDamage; }
	void ResetMaxSpeed() const override;
	void Pain_Hero(int m_LastHitGroup, bool HasArmour) override;
	void DeathSound_Hero() override;
};

#endif
