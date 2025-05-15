/* =================================================================================== *
			 * =================== TechnoSoftware =================== *
 * =================================================================================== */

#ifndef PROJECT_ZCLASSR_H
#define PROJECT_ZCLASSR_H

#include "gamemode/zb5/zb1reference/zb1_zclassreference.h"
#include "gamemode/zb2/zb2_const.h"

class IZombieModeCharacter_ZB2_ExtraR
{
public:
	virtual float HealthRecoveryAmount() const = 0;
	virtual void ActivateSkill(ZombieSkillSlot which) = 0;
	virtual void InitHUD() const = 0;
	virtual void InitHUD2() const = 0;
	virtual ZombieSkillStatus GetSkillStatus(ZombieSkillSlot which) const = 0;
};

class CHuman_ZB2R : public CHuman_ZB1R, public IZombieModeCharacter_ZB2_ExtraR
{
public:
	explicit CHuman_ZB2R(CBasePlayer *player);
	~CHuman_ZB2R() override;
	void ActivateSkill(ZombieSkillSlot which) override;
	float HealthRecoveryAmount() const override { return 0.0f; }
	void InitHUD() const override;
	void InitHUD2() const override;
	void Think() override;
	void ResetMaxSpeed() const override;
	ZombieSkillStatus GetSkillStatus(ZombieSkillSlot which) const override;

	void Pain_Zombie(int m_LastHitGroup, bool HasArmour) override {}
	void DeathSound_Zombie() override {}

protected:
	class impl_t;
	const std::unique_ptr<impl_t> pimpl;
};

class CHuman_ZB5 : public CHuman_ZB2R, public IZombieModeCharacter_ZB2_ExtraR
{
public:
	explicit CHuman_ZB5(CBasePlayer* player);
	~CHuman_ZB5() override;
	void ActivateSkill(ZombieSkillSlot which) override;
	float HealthRecoveryAmount() const override { return 0.0f; }
	void InitHUD() const override;
	void InitHUD2() const override;
	void Think() override;
	void ResetMaxSpeed() const override;
	ZombieSkillStatus GetSkillStatus(ZombieSkillSlot which) const override;

	void Pain_Zombie(int m_LastHitGroup, bool HasArmour) override {}
	void DeathSound_Zombie() override {}

protected:
	class impl_t;
	const std::unique_ptr<impl_t> pimpl;
};

class IZombieSkillR;

class CBaseZombieClass_ZB2R : public CZombie_ZB1R, public IZombieModeCharacter_ZB2_ExtraR
{
public:
	explicit CBaseZombieClass_ZB2R(CBasePlayer *player, ZombieLevel lv) : CZombie_ZB1R(player, lv) {}
	~CBaseZombieClass_ZB2R() override;
	float HealthRecoveryAmount() const override;
	void ActivateSkill(ZombieSkillSlot which) override;
	void InitHUD() const override;
	void InitHUD2() const override;
	void Think() override;
	virtual void OnThink() = 0;
	void ResetMaxSpeed() const override;
	bool ApplyKnockback(CBasePlayer *attacker, const KnockbackData & kbd) override;
	float AdjustDamageTaken(entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType) const override;
	ZombieSkillStatus GetSkillStatus(ZombieSkillSlot which) const override;

protected:
	std::unique_ptr<IZombieSkillR> m_pZombieSkillR;
};

void ZombieSkill_PrecacheR();
void HumanSkill_PrecacheR();

std::shared_ptr<CBaseZombieClass_ZB2R> ZombieClassFactoryR(CBasePlayer *player, ZombieLevel lv, const char *name = nullptr);


#endif
