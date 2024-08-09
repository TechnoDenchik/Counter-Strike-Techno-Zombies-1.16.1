/*
imgui_console.h
Copyright (C) 2020 Moemod Haoyuan

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
*/

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"

void ImGui_Console_Init();
void ImGui_Console_OnGUI(void);
void ImGui_Console_Print(const char* txt);
void ImGui_Console_Clear();
void ImGui_Console_UpdateCommandList();

#ifdef __cplusplus
}
#endif
