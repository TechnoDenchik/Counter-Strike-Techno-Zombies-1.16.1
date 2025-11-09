/* =================================================================================== *
			 * =================== TechnoSoftware =================== *
 * =================================================================================== */

#pragma once

class CHudZSH : public CHudBase
{
public:
	int Init(void) override;
	int VidInit(void) override;
	int Draw(float time) override;
	void Think(void) override;
	void Reset(void) override;
	void InitHUDData(void) override;
	void Shutdown(void) override;
	int MsgFunc_ZSHMsg(const char* pszName, int iSize, void* pbuf);
	int MsgFunc_ZSHUpdateDay(const char* pszName, int iSize, void* pbuf);
	int MsgFunc_ZSHUpdateTime(const char* pszName, int iSize, void* pbuf);
	int MsgFunc_ZSHUpdateRes(const char* pszName, int iSize, void* pbuf);
	int MsgFunc_ZSHUpdateResHome(const char* pszName, int iSize, void* pbuf);
	int MsgFunc_ZSHMsgText(const char* pszName, int iSize, void* pbuf);
	int MsgFunc_ZSHMsgTextNextDay(const char* pszName, int iSize, void* pbuf);
	int MsgFunc_ZSHMsgRound(const char* pszName, int iSize, void* pbuf);
	int MsgFunc_ZSHMentalityHealth(const char* pszName, int iSize, void* pbuf);
	int MsgFunc_ZSHResPos(const char* pszName, int iSize, void* pbuf);
	int MsgFunc_ZSHRes2Pos(const char* pszName, int iSize, void* pbuf);
	int MsgFunc_ZSHZmPos(const char* pszName, int iSize, void* pbuf);
protected:
	class impl_t;
	impl_t* pimpl;
};