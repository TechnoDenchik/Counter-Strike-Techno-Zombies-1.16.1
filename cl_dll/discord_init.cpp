#include <discord.h>

#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
#include "ctime"
#include "const.h"
#include <stdint.h>
// Discord SDK

#include <discord_init.h>

// ==================== PRIVATE IMPLEMENTATION ====================

namespace DiscordPrivate
{
    static const int64_t APP_ID = 1331410585665142886;

    static discord::Core* s_pCore = nullptr;
    static bool s_bInitialized = false;
    static time_point_t s_startTime = 0;
    static float s_flLastUpdate = 0.0f;

    // 🔴 ОПТИМИЗАЦИЯ: Inline хелперы
    inline bool IsTimeForUpdate(float flCurrentTime)
    {
        // Обновляем статус не чаще чем раз в 2 секунды
        return (flCurrentTime - s_flLastUpdate) >= 2.0f;
    }

    inline void UpdateTimestamp(float flCurrentTime)
    {
        s_flLastUpdate = flCurrentTime;
    }

    // 🔴 ОПТИМИЗАЦИЯ: Единая функция обновления активности
    static void UpdateActivity(const char* pszState, const char* pszDetails, const char* pszMapName = nullptr)
    {
        if (!s_bInitialized || !s_pCore) return;

        discord::Activity activity;
        activity.SetState(pszState);
        activity.SetDetails(pszDetails);
        activity.GetTimestamps().SetStart(s_startTime);
        activity.GetAssets().SetLargeImage("logo");
        activity.GetAssets().SetLargeText("Counter-Strike TZ");

        if (pszMapName && pszMapName[0])
        {
            activity.GetAssets().SetSmallImage(pszMapName);
            activity.GetAssets().SetSmallText(pszMapName);
        }

        activity.SetType(discord::ActivityType::Playing);

        // 🔴 ОПТИМИЗАЦИЯ: Без колбэка для производительности
        s_pCore->ActivityManager().UpdateActivity(activity, nullptr);
    }

    // 🔴 ОПТИМИЗАЦИЯ: Быстрая проверка доступности SDK
    static bool CheckSDKAvailability()
    {
#ifdef _WIN32
        HMODULE hDiscord = LoadLibraryA("discord_game_sdk.dll");
        if (hDiscord)
        {
            FreeLibrary(hDiscord);
            return true;
        }
#endif
        return false;
    }
}

// ==================== PUBLIC C INTERFACE IMPLEMENTATION ====================

void DISCORD_Init(void)
{
    using namespace DiscordPrivate;

    if (s_bInitialized) return;

    // Быстрая проверка доступности SDK
    if (!CheckSDKAvailability())
    {
        gEngfuncs.Con_DPrintf("[Discord] SDK not available\n");
        return;
    }

    discord::Core* pCore = nullptr;
    discord::Result result = discord::Core::Create(APP_ID, DiscordCreateFlags_Default, &pCore);

    if (pCore)
    {
        s_pCore = pCore;
        s_bInitialized = true;
        s_startTime = std::time(nullptr);
        s_flLastUpdate = 0.0f;

        // 🔴 ОПТИМИЗАЦИЯ: Минимальная настройка
        s_pCore->SetLogHook(discord::LogLevel::Error, [](discord::LogLevel level, const char* message) {
            gEngfuncs.Con_DPrintf("[Discord] %s\n", message);
            });

        DISCORD_UpdateMainMenu();
        gEngfuncs.Con_DPrintf("[Discord] Rich Presence initialized\n");
    }
    else
    {
        gEngfuncs.Con_DPrintf("[Discord] Init failed: %d\n", static_cast<int>(result));
    }
}

void DISCORD_UpdateMainMenu(void)
{
    DiscordPrivate::UpdateActivity("In Main Menu", "Counter-Strike TZ");
}

void DISCORD_UpdateLoading(const char* mapName)
{
    if (!mapName || !mapName[0]) return;

    char szDetails[48];
    snprintf(szDetails, sizeof(szDetails), "Loading: %s", mapName);
    DiscordPrivate::UpdateActivity("Loading...", szDetails, mapName);
}

void DISCORD_UpdateInGame(const char* mapName, const char* gameMode)
{
    if (!mapName || !mapName[0]) return;

    char szState[32];
    char szDetails[32];

    if (gameMode && gameMode[0])
        snprintf(szState, sizeof(szState), "%s", gameMode);
    else
        snprintf(szState, sizeof(szState), "Playing");

    snprintf(szDetails, sizeof(szDetails), "Map: %s", mapName);

    DiscordPrivate::UpdateActivity(szState, szDetails, mapName);
}

void DISCORD_UpdateConnecting(const char* serverName)
{
    if (!serverName || !serverName[0]) return;

    char szDetails[64];
    snprintf(szDetails, sizeof(szDetails), "Server: %s", serverName);
    DiscordPrivate::UpdateActivity("Connecting...", szDetails);
}

void DISCORD_RunFrame(void)
{
    using namespace DiscordPrivate;

    if (!s_bInitialized || !s_pCore) return;

    // 🔴 ОПТИМИЗАЦИЯ: Ограничение частоты обновления
    float flCurrentTime = gEngfuncs.GetClientTime();
    if (!IsTimeForUpdate(flCurrentTime)) return;

    s_pCore->RunCallbacks();
    UpdateTimestamp(flCurrentTime);
}

void DISCORD_Shutdown(void)
{
    using namespace DiscordPrivate;

    if (s_pCore)
    {
        // 🔴 ОПТИМИЗАЦИЯ: Быстрая очистка
        s_pCore->ActivityManager().ClearActivity(nullptr);
        delete s_pCore;
        s_pCore = nullptr;
    }

    s_bInitialized = false;
    gEngfuncs.Con_DPrintf("[Discord] Shutdown\n");
}

bool DISCORD_IsAvailable(void)
{
    return DiscordPrivate::s_bInitialized;
}