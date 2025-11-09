#pragma once

class CHudDeathMatch : public CHudBase
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
	CHudMsgFunc(RespawnBarDm);

public:
	class CHudDeathMatch_impl_t* pimpl = nullptr;
};