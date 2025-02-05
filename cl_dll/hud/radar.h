#pragma once
#ifndef RADAR_H
#define RADAR_H

#include <memory>

class CHudRadar : public CHudBase
{
public:
	CHudRadar();
	~CHudRadar();
	int Init(void) override;
	int VidInit(void) override;
	int Draw(float time) override;
	void Think(void) override;
	void Reset(void) override;
	void InitHUDData(void) override;		// called every time a server is connected to
	void Shutdown(void) override;

	int GetRadarSize() const;

	void UserCmd_ShowRadar();
	void UserCmd_HideRadar();

	int MsgFunc_Radar(const char* pszName, int iSize, void* pbuf);
	int MsgFunc_BombDrop(const char* pszName, int iSize, void* pbuf);
	int MsgFunc_BombPickup(const char* pszName, int iSize, void* pbuf);
	int MsgFunc_HostagePos(const char* pszName, int iSize, void* pbuf);
	int MsgFunc_HostageK(const char* pszName, int iSize, void* pbuf);
	int MsgFunc_Location(const char* pszName, int iSize, void* pbuf);

	int MsgFunc_ShelterPos(const char* pszName, int iSize, void* pbuf);
	int MsgFunc_ShelterK(const char* pszName, int iSize, void* pbuf);

	int MsgFunc_WoodPos(const char* pszName, int iSize, void* pbuf);
	int MsgFunc_WoodK(const char* pszName, int iSize, void* pbuf);

	int MsgFunc_MetalPos(const char* pszName, int iSize, void* pbuf);
	int MsgFunc_MetalK(const char* pszName, int iSize, void* pbuf);

	int MsgFunc_ZombiePos(const char* pszName, int iSize, void* pbuf);
	int MsgFunc_ZombieK(const char* pszName, int iSize, void* pbuf);

	int MsgFunc_BuyZonePos(const char* pszName, int iSize, void* pbuf);
	int MsgFunc_BuyZoneK(const char* pszName, int iSize, void* pbuf);

private:
	struct impl_t;
	std::unique_ptr<impl_t> pimpl;
};

extern char g_szLocation[2048];

#endif // RADAR_H
