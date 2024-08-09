/* =================================================================================== *
			 * =================== TechnoSoftware =================== *
 * =================================================================================== */

#pragma once

class CHudGunDeath : public CHudBase
{
public:
	int Init() override;
	int VidInit() override;
	int Draw(float time) override;
	void Think() override;
	void Reset() override;
	void InitHUDData() override;		// called every time a server is connected to
	void Shutdown() override;
public:
	bool m_bCanDraw;
	CHudMsgFunc(GDMsg);
	CHudMsgFunc(GDMsgLV);



public:
	class impl_t;
	impl_t* pimpl;
};