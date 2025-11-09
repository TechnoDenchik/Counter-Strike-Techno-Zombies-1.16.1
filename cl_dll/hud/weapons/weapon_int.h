#pragma once

class CWeaponInt: public CHudBase
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
	CHudMsgFunc(HudTwinAxesMsg);
	CHudMsgFunc(HudQuantumMsg);
	CHudMsgFunc(ArbalestMsg);

public:
	class CWeaponInt_impl_t *pimpl = nullptr;
};