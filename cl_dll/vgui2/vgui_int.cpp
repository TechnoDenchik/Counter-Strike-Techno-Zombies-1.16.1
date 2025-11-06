#include <vgui/vgui.h>
#include <vgui/ISurface.h>
#include <vgui/ilocalize.h>
#include <..\vgui2_support\SourceSDK\public\tier1\KeyValues.h>

#include <vgui_controls/Controls.h>

#include <IEngineVGui.h>
#include <vgui/IPanel.h>

enum HudSizeMode
{
    HUD_SIZE_ACTUAL,     // Точный размер HUD
    HUD_SIZE_SAFE_AREA,  // С учетом безопасной области
    HUD_SIZE_FULLSCREEN  // Полный экран
};

void GetHudSize(int& w, int& h, HudSizeMode mode = HUD_SIZE_ACTUAL)
{
    if (!engineVgui() || !vgui2::ipanel() || !vgui2::surface())
    {
        vgui2::surface()->GetScreenSize(w, h);
        return;
    }

    vgui2::VPANEL hudParent = engineVgui()->GetPanel(PANEL_CLIENTDLL);

    switch (mode)
    {
    case HUD_SIZE_ACTUAL:
        if (hudParent && vgui2::ipanel()->IsVisible(hudParent))
            vgui2::ipanel()->GetSize(hudParent, w, h);
        else
            vgui2::surface()->GetScreenSize(w, h);
        break;

    case HUD_SIZE_SAFE_AREA:
    {
        // С учетом безопасной области (для TV и мобильных устройств)
        int safeW, safeH;
        vgui2::surface()->GetScreenSize(safeW, safeH);

        // Уменьшаем на 10% для безопасной области
        w = safeW * 0.9f;
        h = safeH * 0.9f;
    }
    break;

    case HUD_SIZE_FULLSCREEN:
        vgui2::surface()->GetScreenSize(w, h);
        break;
    }
}