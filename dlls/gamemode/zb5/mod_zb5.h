/* =================================================================================== *
			 * =================== TechnoSoftware =================== *
 * =================================================================================== */

#ifndef PROJECT_MOD_ZB5_H
#define PROJECT_MOD_ZB5_H

#include "mod_zb2reference.h"
#include "zb5_const.h"
#include "gamemode/zb5/zb3reference/zb3_moralereference.h"
#include "gamemode/zb5/zb3reference/zb3_heroreference.h"

class CZb5EvoBox;

class CMod_ZombieEvolution : public CMod_ZombieMod2R
{
public:
	CMod_ZombieEvolution();
	void InstallPlayerModStrategy(CBasePlayer *) override;
	void UpdateGameMode(CBasePlayer *pPlayer) override;
	void RestartRound() override;
	void PlayerKilled(CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pInflictor) override;
	void CheckWinConditions() override;
	
	int kills;

	const CZB3HumanMoraleR &HumanMorale() const { return m_Morale; }

	EventDispatcher<void()> m_eventRoundStart;

public:
	void MakeEvoboxThink(Vector x, Vector y);
	void RemoveAllEvobox();
	CZb5EvoBox* CreateEvobox(Vector x, Vector y);
	int EvoboxCount();
	int evobox;

protected:
	CZB3HumanMoraleR m_Morale;
};

class CPlayerModStrategy_ZB5 : public CPlayerModStrategy_ZB2R
{
public:
	CPlayerModStrategy_ZB5(CBasePlayer *player, CMod_ZombieEvolution*mp);

	void OnSpawn() override;
	bool CanUseZombieSkill() override { return true; }
	void CheckEvolution() override;
	void BecomeZombie(ZombieLevel iEvolutionLevel) override;
	void Event_OnInfection(CBasePlayer *victim, CBasePlayer *attacker) override;
	void UpdatePlayerEvolutionHUD() override;
	float AdjustDamageTaken(entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType) override;
	void Event_AdjustHumanDamage(CBasePlayer* attacker, float& flDamage) override;
	void OnThink() override;
	void OnKilled(entvars_t *pKiller, entvars_t *pInflictor) override;
	void addevofrombox(CBaseEntity* pOther);
	bool ClientCommand2(const char* pcmd) override;
	void BecomeHuman() override;
	void Event_AdjustHumanHitgroup(CBasePlayer* attacker, HitBoxGroup& iHitgroup) override;
	virtual void Event_OnRoundStart();
private:

	std::shared_ptr<IZombieModeCharacter_ZB2_ExtraR> m_pCharacter_ZB5;

	CMod_ZombieEvolution* const m_pModZB5;
	CZB3HumanMoraleR m_Morale;
	const EventListener m_eventRoundStartListener;
	
	float m_flRagePercent;
	float m_flDeadTime;
	float m_flBackupMaxHealth;
	float m_flBackupArmor;
};

#endif
