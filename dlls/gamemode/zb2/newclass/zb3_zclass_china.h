/* =================================================================================== *
			 * =================== TechnoSoftware =================== *
 * =================================================================================== */

#ifndef PROJECT_ZB3_ZCLASS_CHINA_H
#define PROJECT_ZB3_ZCLASS_CHINA_H

#include "../zb2_zclass.h"

class CZombieClass_China : public CBaseZombieClass_ZB2
{
public:
	explicit CZombieClass_China(CBasePlayer *player, ZombieLevel lv);
	void InitHUD() const override;
	void ResetMaxSpeed() const override;
	float AdjustDamageTaken(entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType) const override;
	bool ApplyKnockback(CBasePlayer *attacker, const KnockbackData & kbd) override;
	void Pain_Zombie(int m_LastHitGroup, bool HasArmour) override;
	void OnThink() override;
	virtual void Precache();
	virtual void Zombie_HealthRecoveryThink();
	void DeathSound_Zombie() override;
	std::shared_ptr<IZombieModeCharacter_ZB2_Extra> m_pCharacter_ZB2;
	float m_flTimeNextZombieHealthRecovery;
};
#endif