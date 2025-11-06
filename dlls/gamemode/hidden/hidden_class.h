
#ifndef PROJECT_HIDDEN_CLASS_H
#define PROJECT_HIDDEN_CLASS_H

#include "player/player_mod_strategy.h"

class IJokerModeCharacter
{
public:
	virtual ~IJokerModeCharacter() = default;

	virtual void Think() = 0;
	virtual void ResetMaxSpeed() const = 0;
	virtual bool ApplyKnockback(CBasePlayer *attacker, const KnockbackData & kbd) = 0;
	virtual float AdjustDamageTaken(entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType) const = 0;
	virtual void Pain_Joker(int m_LastHitGroup, bool HasArmour) = 0;
	virtual void DeathSound_Joker() = 0;
};

class CHuman_Hidden : public BasePlayerExtra, public IJokerModeCharacter
{
public:
	explicit CHuman_Hidden(CBasePlayer *player) ;
	void Think() override {}
	void ResetMaxSpeed() const override {}
	bool ApplyKnockback(CBasePlayer *attacker, const KnockbackData & kbd) override { return false;}
	float AdjustDamageTaken(entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType) const override { return flDamage; }
	void Pain_Joker(int m_LastHitGroup, bool HasArmour) override {}
	void DeathSound_Joker() override {}
};

class CJoker_Hidden : public BasePlayerExtra, public IJokerModeCharacter
{
public:
	explicit CJoker_Hidden(CBasePlayer* player);
	void Think() override {}
	void ResetMaxSpeed() const override;
	bool ApplyKnockback(CBasePlayer* attacker, const KnockbackData& data) override { ApplyKnockbackData(m_pPlayer, m_pPlayer->pev->origin - attacker->pev->origin, data); return true; }
	float AdjustDamageTaken(entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage, int bitsDamageType) const override { return flDamage; }
	void Pain_Joker(int m_LastHitGroup, bool HasArmour) override;
	void DeathSound_Joker() override;
};

class CTratior_Hidden : public BasePlayerExtra, public IJokerModeCharacter
{
public:
	explicit CTratior_Hidden(CBasePlayer *player);
	bool ApplyKnockback(CBasePlayer *attacker, const KnockbackData & data) override { ApplyKnockbackData(m_pPlayer, m_pPlayer->pev->origin - attacker->pev->origin, data); return true; }
	float AdjustDamageTaken(entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType) const override { return flDamage; }
	void ResetMaxSpeed() const override {}
	void Pain_Joker(int m_LastHitGroup, bool HasArmour) override {}
	void DeathSound_Joker() override {}
};

#endif