#pragma once
#ifndef DISCORD_CLIENT_H
#define DISCORD_CLIENT_H

#ifdef __cplusplus
extern "C" {
#endif

	typedef int HSPRITE;	// handle to a graphic

	// ==================== PUBLIC C INTERFACE ====================
	void DISCORD_Init(void);
	void DISCORD_UpdateMainMenu(void);
	void DISCORD_UpdateLoading(const char* mapName);
	void DISCORD_UpdateInGame(const char* mapName, const char* gameMode);
	void DISCORD_UpdateConnecting(const char* serverName);
	void DISCORD_RunFrame(void);
	void DISCORD_Shutdown(void);
	bool DISCORD_IsAvailable(void);

#ifdef __cplusplus
}
#endif

#endif // DISCORD_CLIENT_H