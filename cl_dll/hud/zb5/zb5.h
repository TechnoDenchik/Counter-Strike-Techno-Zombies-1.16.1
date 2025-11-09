/* =================================================================================== *
			 * =================== TechnoSoftware =================== *
 * =================================================================================== */

#pragma once

class CHudZB5 : public CHudBase
{
public:
	int Init(void) override;
	int VidInit(void) override;
	int Draw(float time) override;
	void Think(void) override;
	void Reset(void) override;
	void InitHUDData(void) override;
	void Shutdown(void) override;
	CHudMsgFunc(ZB5Msg);
	CHudMsgFunc(ZB5SkillUsed);
	CHudMsgFunc(ZB5SkillUsed2);
	CHudMsgFunc(ZB5Inventory);
	CHudMsgFunc(ZB5Evolevel);
	CHudMsgFunc(ZB5Getlocationbox);
	CHudMsgFunc(ZB5GetlocationboxK);
	CHudMsgFunc(ZB5ExternEvo);
	CHudMsgFunc(ZB5UpdateEvoLv);
	CHudMsgFunc(HitDamageMsgZB5);
	CHudMsgFunc(ZB5RespawnBar);

	int DrawHeroIcon();

protected:

	UniqueTexture m_iHero;

	class impl_t;
	impl_t *pimpl;
};