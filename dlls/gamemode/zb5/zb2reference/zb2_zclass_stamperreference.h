/* =================================================================================== *
			 * =================== TechnoSoftware =================== *
 * =================================================================================== */

#ifndef PROJECT_ZB2_ZCLASS_STAMPERR_H
#define PROJECT_ZB2_ZCLASS_STAMPERR_H

#include "gamemode/zb5/zb2reference/zb2_zclassreference.h"

class CZombieClass_StamperR : public CBaseZombieClass_ZB2R
{
public:
	explicit CZombieClass_StamperR(CBasePlayer *player, ZombieLevel lv);
	void InitHUD() const override;
	void ResetMaxSpeed() const override;
	float AdjustDamageTaken(entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType) const override;
	bool ApplyKnockback(CBasePlayer *attacker, const KnockbackData & kbd) override;
	void Pain_Zombie(int m_LastHitGroup, bool HasArmour) override;
	void OnThink() override;
	virtual void Precache();
	virtual void Zombie_HealthRecoveryThink();
	void DeathSound_Zombie() override;
	std::shared_ptr<IZombieModeCharacter_ZB2_ExtraR> m_pCharacter_ZB2;
	float m_flTimeNextZombieHealthRecovery;
};
#endif