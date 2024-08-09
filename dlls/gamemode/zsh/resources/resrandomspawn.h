#ifndef RES_RANDOMSPAWN_H
#define RES_RANDOMSPAWN_H
#ifdef _WIN32
#pragma once
#endif

#include <utility>
#include "vector.h"

class CBaseEntity;
void RES_LoadSpawnPoints();

bool RES_DoRandomSpawn(CBaseEntity *pEntity);

#endif