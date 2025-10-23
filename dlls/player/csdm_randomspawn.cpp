// Translated from
// https://github.com/MoeMod/BTE-AMXX/blob/master/BTE%20Codename%20Z4E/z4e_random_spawn.sma
// or random CSDM .sma, IDK where this code came from...
// MoeMod 2018/7/1

#include "extdll.h"
#include "common.h"
#include "util.h"
#include "cbase.h"

#include "game_shared2/steam_util.h"

#include <string>
#include <algorithm>
#include <iterator>
#include <sstream>
#include <vector>
#include <array>
#include <functional>
#include <random>

#include <gamerules.h>

#define SPAWN_DATA_ORIGIN_X 0
#define SPAWN_DATA_ORIGIN_Y 1
#define SPAWN_DATA_ORIGIN_Z 2
#define SPAWN_DATA_ANGLES_X 3
#define SPAWN_DATA_ANGLES_Y 4
#define SPAWN_DATA_ANGLES_Z 5
#define SPAWN_DATA_V_ANGLES_X 6
#define SPAWN_DATA_V_ANGLES_Y 7
#define SPAWN_DATA_V_ANGLES_Z 8

struct SpawnPointData
{
	Vector origin;
	Vector angles;
	Vector v_angle;
};
std::vector<SpawnPointData> g_vecSpawnCSDM;

template<class Arr>
inline Vector PackVector(const Arr &arr, size_t N)
{
	return { arr[N * 3 + 0], arr[N * 3 + 1], arr[N * 3 + 2] };
}
SpawnPointData MakeSpawnPointData(const std::array<float, 9> &arr)
{
	return { PackVector(arr, 0), PackVector(arr, 1), PackVector(arr, 2) };
}

BOOL CSDM_IsSpawnPointSafe(CBaseEntity* pPlayer, const Vector& vecOrigin)
{
    if (vecOrigin.IsZero())
        return FALSE;

    TraceResult tr;
    Vector vecTestOrigin = vecOrigin;

    // Проверка 1: Не внутри geometry (проверка hull)
    UTIL_TraceHull(vecTestOrigin, vecTestOrigin, ignore_monsters, human_hull, pPlayer ? pPlayer->edict() : nullptr, &tr);
    if (tr.fStartSolid)
    {
        ALERT(at_console, "CSDM: Spawn point inside solid\n");
        return FALSE;
    }

    // Проверка 2: Есть ли земля под ногами
    UTIL_TraceLine(vecTestOrigin, vecTestOrigin - Vector(0, 0, 72), ignore_monsters, pPlayer ? pPlayer->edict() : nullptr, &tr);
    if (tr.flFraction == 1.0) // Не нашли землю
    {
        ALERT(at_console, "CSDM: No ground below spawn point\n");
        return FALSE;
    }

    // Проверка 3: Земля не слишком крутая (нормаль Z > 0.7)
    if (tr.vecPlaneNormal.z < 0.7f)
    {
        ALERT(at_console, "CSDM: Ground too steep (normal Z=%.2f)\n", tr.vecPlaneNormal.z);
        return FALSE;
    }

    // Проверка 4: Достаточно места над головой
    UTIL_TraceLine(vecTestOrigin, vecTestOrigin + Vector(0, 0, 72), ignore_monsters, pPlayer ? pPlayer->edict() : nullptr, &tr);
    if (tr.flFraction < 1.0)
    {
        ALERT(at_console, "CSDM: Not enough headroom\n");
        return FALSE;
    }

    // Проверка 5: Нет игроков слишком близко (если проверяем для конкретного игрока)
    if (pPlayer)
    {
        CBaseEntity* pOther = nullptr;
        while ((pOther = UTIL_FindEntityInSphere(pOther, vecOrigin, 128.0)) != nullptr)
        {
            if (pOther->IsPlayer() && pOther != pPlayer && pOther->IsAlive())
            {
                ALERT(at_console, "CSDM: Player too close to spawn point\n");
                return FALSE;
            }
        }
    }
    else
    {
        // При генерации точек проверяем что нет никаких игроков nearby
        CBaseEntity* pOther = nullptr;
        while ((pOther = UTIL_FindEntityInSphere(pOther, vecOrigin, 96.0)) != nullptr)
        {
            if (pOther->IsPlayer() && pOther->IsAlive())
            {
                ALERT(at_console, "CSDM: Player at spawn location during generation\n");
                return FALSE;
            }
        }
    }

    // Проверка 7: Нет опасных объектов nearby (бомба, и т.д.)
    CBaseEntity* pDanger = nullptr;
    while ((pDanger = UTIL_FindEntityInSphere(pDanger, vecOrigin, 256.0)) != nullptr)
    {
        const char* classname = STRING(pDanger->pev->classname);

        // Проверяем опасные entities
        if (strstr(classname, "grenade") ||
            strstr(classname, "bomb") ||
            strstr(classname, "explo") ||
            strstr(classname, "fire"))
        {
            ALERT(at_console, "CSDM: Dangerous entity near spawn point\n");
            return FALSE;
        }
    }

    return TRUE;
}

BOOL CSDM_IsSpawnPointValid(CBaseEntity* pPlayer, const SpawnPointData& data)
{
    return CSDM_IsSpawnPointSafe(pPlayer, data.origin);
}

void CSDM_ApplyRandomSpawnPoint(CBaseEntity *pEntity, const SpawnPointData & data)
{
	pEntity->pev->origin = data.origin + Vector(0, 0, 1);
	pEntity->pev->velocity = g_vecZero;
	pEntity->pev->angles = data.angles;
	pEntity->pev->v_angle = data.v_angle;
}

bool CSDM_DoRandomSpawn(CBaseEntity *pEntity)
{
	if (g_vecSpawnCSDM.empty())
		return false;
	// randomize those fucking spawn points
	std::random_shuffle(g_vecSpawnCSDM.begin(), g_vecSpawnCSDM.end());
	using namespace std::placeholders;
	// find the first availble one
	auto iter = std::find_if(g_vecSpawnCSDM.begin(), g_vecSpawnCSDM.end(), std::bind(CSDM_IsSpawnPointValid, pEntity, _1));
	if (iter == g_vecSpawnCSDM.end())
		return false;

	// sets these item
	CSDM_ApplyRandomSpawnPoint(pEntity, *iter);
	return true;
}

void CSDM_GetMapBounds(Vector& vecMin, Vector& vecMax)
{
    // Способ 1: Пытаемся получить границы из worldspawn
    CBaseEntity* pWorld = UTIL_FindEntityByClassname(nullptr, "worldspawn");
    if (pWorld && !pWorld->pev->mins.IsZero() && !pWorld->pev->maxs.IsZero())
    {
        vecMin = pWorld->pev->mins;
        vecMax = pWorld->pev->maxs;
        ALERT(at_console, "CSDM: Using worldspawn bounds\n");
        return;
    }

    // Способ 2: Определяем границы по entities на карте
    vecMin = Vector(999999, 999999, 999999);
    vecMax = Vector(-999999, -999999, -999999);

    // Ищем различные entities чтобы определить границы карты
    const char* searchEntities[] = {
        "info_player_start",
        "info_player_deathmatch",
        "info_player_terrorist",
        "info_player_counterterrorist",
        "func_wall",
        "func_door",
        "func_breakable"
    };

    int entitiesFound = 0;

    for (int i = 0; i < sizeof(searchEntities) / sizeof(searchEntities[0]); i++)
    {
        CBaseEntity* pEntity = nullptr;
        while ((pEntity = UTIL_FindEntityByClassname(pEntity, searchEntities[i])) != nullptr)
        {
            entitiesFound++;

            // Обновляем минимальные и максимальные координаты
            for (int j = 0; j < 3; j++)
            {
                if (pEntity->pev->origin[j] < vecMin[j])
                    vecMin[j] = pEntity->pev->origin[j];
                if (pEntity->pev->origin[j] > vecMax[j])
                    vecMax[j] = pEntity->pev->origin[j];
            }
        }
    }

    // Если не нашли entities, используем дефолтные значения
    if (entitiesFound == 0 || vecMin.x == 999999)
    {
        ALERT(at_console, "CSDM: Using default map bounds\n");
        vecMin = Vector(-2048, -2048, -1024);
        vecMax = Vector(2048, 2048, 1024);
    }
    else
    {
        // Добавляем отступы от границ
        vecMin = vecMin - Vector(512, 512, 128);
        vecMax = vecMax + Vector(512, 512, 256);

        // Ограничиваем разумными размерами
        for (int i = 0; i < 3; i++)
        {
            float size = vecMax[i] - vecMin[i];
            if (size > 8192) // Слишком большая карта
            {
                float center = (vecMin[i] + vecMax[i]) * 0.5f;
                vecMin[i] = center - 4096;
                vecMax[i] = center + 4096;
            }
            else if (size < 1024) // Слишком маленькая карта
            {
                float center = (vecMin[i] + vecMax[i]) * 0.5f;
                vecMin[i] = center - 512;
                vecMax[i] = center + 512;
            }
        }
    }

    ALERT(at_console, "CSDM: Map bounds: (%.0f,%.0f,%.0f) to (%.0f,%.0f,%.0f)\n",
        vecMin.x, vecMin.y, vecMin.z, vecMax.x, vecMax.y, vecMax.z);
}

Vector CSDM_FindGroundPosition(const Vector& vecStart)
{
    TraceResult tr;

    // Ищем землю сверху вниз
    UTIL_TraceLine(vecStart, vecStart - Vector(0, 0, 2048), ignore_monsters, nullptr, &tr);

    if (tr.flFraction < 1.0 && !tr.fStartSolid && tr.vecPlaneNormal.z > 0.7f)
    {
        // Нашли землю с нормальным углом (не стена)
        return tr.vecEndPos;
    }

    // Пробуем найти землю снизу вверх (для случаев когда начальная точка под землей)
    UTIL_TraceLine(vecStart - Vector(0, 0, 1024), vecStart + Vector(0, 0, 1024), ignore_monsters, nullptr, &tr);

    if (tr.flFraction < 1.0 && !tr.fStartSolid && tr.vecPlaneNormal.z > 0.7f)
    {
        return tr.vecEndPos;
    }

    return Vector(0, 0, 0); // Не нашли подходящую землю
}





// Улучшенная версия для генерации с более строгими проверками
BOOL CSDM_IsSpawnPointSafeForGeneration(const Vector& vecOrigin)
{
    if (vecOrigin.IsZero())
        return FALSE;

    TraceResult tr;

    // Более строгая проверка geometry
    UTIL_TraceHull(vecOrigin, vecOrigin, dont_ignore_monsters, human_hull, nullptr, &tr);
    if (tr.fStartSolid || tr.flFraction < 1.0)
        return FALSE;

    // Проверка земли
    UTIL_TraceLine(vecOrigin, vecOrigin - Vector(0, 0, 64), dont_ignore_monsters, nullptr, &tr);
    if (tr.flFraction == 1.0 || tr.vecPlaneNormal.z < 0.8f)
        return FALSE;

    // Проверка пространства над головой
    UTIL_TraceLine(vecOrigin, vecOrigin + Vector(0, 0, 64), dont_ignore_monsters, nullptr, &tr);
    if (tr.flFraction < 1.0)
        return FALSE;

    // Проверка что нет entities слишком близко
    CBaseEntity* pEntity = nullptr;
    while ((pEntity = UTIL_FindEntityInSphere(pEntity, vecOrigin, 48.0)) != nullptr)
    {
        // Игнорируем только worldspawn и триггеры
        if (!FStrEq(STRING(pEntity->pev->classname), "worldspawn") &&
            !FStrEq(STRING(pEntity->pev->classname), "trigger_multiple"))
        {
            return FALSE;
        }
    }

    return TRUE;
}

void CSDM_CollectMapSpawnPoints()
{
    const char* spawnEntities[] = {
        "info_player_start",
        "info_player_deathmatch",
        "info_player_terrorist",
        "info_player_counterterrorist",
        "info_deathmatch_spawn"
    };

    for (int i = 0; i < sizeof(spawnEntities) / sizeof(spawnEntities[0]); i++)
    {
        CBaseEntity* pEntity = nullptr;
        while ((pEntity = UTIL_FindEntityByClassname(pEntity, spawnEntities[i])) != nullptr)
        {
            if (CSDM_IsSpawnPointSafe(nullptr, pEntity->pev->origin))
            {
                SpawnPointData data;
                data.origin = pEntity->pev->origin;
                data.angles = pEntity->pev->angles;
                data.v_angle = pEntity->pev->v_angle;

                g_vecSpawnCSDM.push_back(data);
                ALERT(at_console, "CSDM: Found map spawn at (%.0f, %.0f, %.0f)\n",
                    data.origin.x, data.origin.y, data.origin.z);
            }
        }
    }
}

void CSDM_ShuffleSpawnPoints()
{
    // Перемешиваем точки для рандомизации
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(g_vecSpawnCSDM.begin(), g_vecSpawnCSDM.end(), g);
}

void CSDM_GenerateDynamicSpawnPoints(int numPoints)
{
    Vector vecMin, vecMax;
    CSDM_GetMapBounds(vecMin, vecMax);

    ALERT(at_console, "CSDM: Generating %d dynamic spawn points\n", numPoints);

    int pointsGenerated = 0;
    int attempts = 0;
    const int maxAttempts = numPoints * 10; // Ограничение попыток

    while (pointsGenerated < numPoints && attempts < maxAttempts)
    {
        attempts++;

        // Генерируем случайную точку в пределах карты
        Vector vecPoint;
        vecPoint.x = RANDOM_FLOAT(vecMin.x + 128, vecMax.x - 128);
        vecPoint.y = RANDOM_FLOAT(vecMin.y + 128, vecMax.y - 128);
        vecPoint.z = vecMax.z + 128;

        // Находим землю
        Vector vecGround = CSDM_FindGroundPosition(vecPoint);
        if (!vecGround.IsZero() && CSDM_IsSpawnPointSafe(nullptr, vecGround))
        {
            SpawnPointData data;
            data.origin = vecGround + Vector(0, 0, 36);
            data.angles = Vector(0, RANDOM_FLOAT(0, 360), 0);
            data.v_angle = data.angles;

            g_vecSpawnCSDM.push_back(data);
            pointsGenerated++;
        }
    }

    ALERT(at_console, "CSDM: Generated %d dynamic points after %d attempts\n", pointsGenerated, attempts);
}

void CSDM_GenerateSpawnPoints()
{
    g_vecSpawnCSDM.clear();

    // Сначала собираем существующие точки спавна карты
    CSDM_CollectMapSpawnPoints();

    // Если недостаточно точек, генерируем дополнительные
    if (g_vecSpawnCSDM.size() < 16) // Минимум 16 точек
    {
        CSDM_GenerateDynamicSpawnPoints(16 - g_vecSpawnCSDM.size());
    }

    // Перемешиваем все точки для рандомизации
    CSDM_ShuffleSpawnPoints();

    ALERT(at_console, "CSDM: Generated %d spawn points total\n", g_vecSpawnCSDM.size());
}

void LoadSpawnPointsFromFile(SteamFile& csdmFile)
{
    auto readline = [](SteamFile& sf) {
        std::string ret;
        char ch = '\0';
        while (sf.Read((void*)&ch, sizeof(char)))
        {
            if (ch == '\n')
                return std::make_pair(true, ret);
            ret.push_back(ch);
        }
        return std::make_pair(false, ret);
        };

    std::pair<bool, std::string> linedata;
    while ((linedata = readline(csdmFile)).first)
    {
        // Пропускаем пустые строки и комментарии
        if (linedata.second.empty() || linedata.second[0] == '#')
            continue;

        std::array<float, 9> arr;
        std::istringstream ss(linedata.second);

        bool validLine = true;
        for (int i = 0; i < 9; ++i)
        {
            if (ss && !ss.eof())
                ss >> arr[i];
            else
            {
                validLine = false;
                break;
            }
        }

        if (validLine)
        {
            g_vecSpawnCSDM.emplace_back(MakeSpawnPointData(arr));
        }
    }
}

void CSDM_CreateMapsDirectory()
{
    // Создаем директорию cfg/maps если ее нет
#ifdef _WIN32
    CreateDirectory("cfg", NULL);
    CreateDirectory("cfg/maps", NULL);
#else
    mkdir("cfg", 0755);
    mkdir("cfg/maps", 0755);
#endif
}

const char* CSDM_GetCurrentDateTime()
{
    static char timebuf[64];
    time_t now = time(nullptr);
    struct tm* t = localtime(&now);

    strftime(timebuf, sizeof(timebuf), "%Y-%m-%d %H:%M:%S", t);
    return timebuf;
}

void CSDM_SaveSpawnPointsToFile(const char* filename)
{
    if (g_vecSpawnCSDM.empty())
    {
        ALERT(at_console, "CSDM: No spawn points to save\n");
        return;
    }

    // Создаем директорию если нужно
    CSDM_CreateMapsDirectory();

    FILE* file = fopen(filename, "w");
    if (!file)
    {
        ALERT(at_console, "CSDM: Failed to create file %s\n", filename);
        return;
    }

    // Записываем заголовок
    fprintf(file, "# CSDM Spawn Points for map %s\n", STRING(gpGlobals->mapname));
    fprintf(file, "# Generated automatically on %s\n", CSDM_GetCurrentDateTime());
    fprintf(file, "# Format: origin_x origin_y origin_z angles_x angles_y angles_z v_angle_x v_angle_y v_angle_z\n");
    fprintf(file, "# Total points: %d\n\n", g_vecSpawnCSDM.size());

    // Записываем точки спавна
    for (const auto& spawn : g_vecSpawnCSDM)
    {
        fprintf(file, "%.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f\n",
            spawn.origin.x, spawn.origin.y, spawn.origin.z,
            spawn.angles.x, spawn.angles.y, spawn.angles.z,
            spawn.v_angle.x, spawn.v_angle.y, spawn.v_angle.z);
    }

    fclose(file);
    ALERT(at_console, "CSDM: Saved %d spawn points to %s\n", g_vecSpawnCSDM.size(), filename);
}

void CSDM_LoadSpawnPoints()
{
    g_vecSpawnCSDM.clear();

    // Проверяем существование файла спавнов
    char filename[256];
    Q_sprintf(filename, "cfg/maps/%s.spawns.cfg", STRING(gpGlobals->mapname));

    SteamFile csdmFile(filename);

    if (csdmFile.IsValid())
    {
        // Файл существует - загружаем из него
        ALERT(at_console, "CSDM: Loading spawn points from %s\n", filename);
        LoadSpawnPointsFromFile(csdmFile);

        if (!g_vecSpawnCSDM.empty())
        {
            ALERT(at_console, "CSDM: Loaded %d spawn points from file\n", g_vecSpawnCSDM.size());
            return;
        }
    }

    // Файла нет или он пустой - генерируем новые спавны
    ALERT(at_console, "CSDM: Generating new spawn points for map %s\n", STRING(gpGlobals->mapname));
    CSDM_GenerateSpawnPoints();

    // Сохраняем сгенерированные спавны в файл
    CSDM_SaveSpawnPointsToFile(filename);
}

















// Функция для принудительной перегенерации спавнов
void CSDM_RegenerateSpawnPoints()
{
    ALERT(at_console, "CSDM: Force regenerating spawn points\n");

    char filename[256];
    Q_sprintf(filename, "cfg/maps/%s.spawns.cfg", STRING(gpGlobals->mapname));

    // Удаляем старый файл
    remove(filename);

    // Генерируем новые спавны
    CSDM_LoadSpawnPoints();
}

// Функция для добавления точки вручную с сохранением в файл
void CSDM_AddCustomSpawnPoint(CBaseEntity* pEntity)
{
    if (!pEntity || !pEntity->IsPlayer())
        return;

    SpawnPointData data;
    data.origin = pEntity->pev->origin;
    data.angles = pEntity->pev->angles;
    data.v_angle = pEntity->pev->v_angle;

    g_vecSpawnCSDM.push_back(data);

    ALERT(at_console, "CSDM: Added custom spawn point at (%.0f, %.0f, %.0f)\n",
        data.origin.x, data.origin.y, data.origin.z);

    // Сохраняем в файл
    char filename[256];
    Q_sprintf(filename, "cfg/maps/%s.spawns.cfg", STRING(gpGlobals->mapname));
    CSDM_SaveSpawnPointsToFile(filename);
}
