/* =================================================================================== *
			 * =================== TechnoSoftware =================== *
 * =================================================================================== */

#pragma once

class CHudPR : public CHudBase
{
public:
	int Init() override;
	int VidInit() override;
	int Draw(float time) override;
	void Think() override;
	void Reset() override;
	void InitHUDData() override;		// called every time a server is connected to
	void Shutdown() override;
	void ClearSelector();
	void SetSelectorDrawTime(float flTime, float flDisplayTime);
	void SetSelectorIcon(int slot, const char* name);
	void SetSelectorIconBan(int slot);
	void SetSelectorIconLevel(int slot, int level);
	bool Selector(int i);
	bool SelectorCanDraw();
	void SendAlarmState(int iType, float flValue, int iValue2);
public:
	bool ActivateSkill(int iSlot);
	float m_flAliveTime;
	float m_flPlayerMoveDis = 0.0;
	float m_flRecoveryAmount = 0.0;

	bool m_bCanDraw;
	CHudMsgFunc(PRMsg);
	CHudMsgFunc(PRRenMsg);
	CHudMsgFunc(PRRespMsg);
	CHudMsgFunc(PRHero);

public:
	class CHudPR_impl_t *pimpl = nullptr;
};