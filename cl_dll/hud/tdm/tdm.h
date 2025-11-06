#pragma once

class CHudTeamDeathMatch : public CHudBase
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
	CHudMsgFunc(RespawnBarTdm);

public:
	class CHudTeamDeathMatch_impl_t* pimpl = nullptr;
};