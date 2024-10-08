#include <vector>
#include <string.h>
#include <vgui_api.h>
#include "wrect.h"
#include "cl_dll.h"

#include "entity_state.h"
#include "usercmd.h"
#include "ref_params.h"
#include "cl_entity.h"
#include "cdll_exp.h"

#include "CBaseUI.h"
#include <FileSystem.h>
#include "vgui/IVGui.h"
#include "vgui/IPanel.h"
#include "vgui/ILocalize.h"
#include "vgui/IScheme.h"
#include "vgui/ISystem.h"
#include "vgui/IClientVGUI.h"
#include "vgui/IInputInternal.h"
#include "vgui_controls_simple/controls.h"
#include "vgui_controls_simple/Panel.h"
#include "BaseUISurface.h"

extern cldll_func_t gClDllFuncs;

IClientVGUI *staticClient;
vgui2::IInputInternal *g_pInputInternal;
BaseUISurface *staticSurface;

class CStaticPanel *staticPanel;
class CEnginePanel *staticClientDLLPanel;

class CEnginePanel : public vgui2::Panel {
public:
	CEnginePanel() {
		vgui2::Panel::Panel();
		m_bCanFocus = true;
	}

	vgui2::VPANEL IsWithinTraverse(int x, int y, bool traversePopups) {
		if (!m_bCanFocus) {
			return 0;
		}

		vgui2::VPANEL result = vgui2::Panel::IsWithinTraverse(x, y, traversePopups);

		if (result == GetVPanel()) {
			return 0;
		}

		return result;
	}

private:
	bool m_bCanFocus;
};

class CStaticPanel : public vgui2::Panel {
public:
	vgui2::VPANEL GetCurrentKeyFocus() {
		return staticClientDLLPanel->GetCurrentKeyFocus();
	}

	vgui2::VPANEL IsWithinTraverse(int x, int y, bool traversePopups) {
		vgui2::VPANEL result = vgui2::Panel::IsWithinTraverse(x, y, traversePopups);

		if (result == GetVPanel()) {
			return 0;
		}

		return result;
	}
};

void CBaseUI::Initialize(CreateInterfaceFn* factories, int count) {
	if (staticClient) {
		return;
	}

	m_hFileSystemModule = Sys_LoadModule("FileSystem_stdio.dll");
	m_hVGuiModule = Sys_LoadModule("vgui2.dll");
	m_hChromeModule = Sys_LoadModule("chromehtml.dll");
	m_FactoryList[0] = factories[0];
	m_FactoryList[1] = Sys_GetFactory(m_hVGuiModule);
	m_FactoryList[2] = Sys_GetFactory(m_hFileSystemModule);
	m_FactoryList[3] = Sys_GetFactory(m_hChromeModule);
	m_iNumFactories = 4;
	g_pInputInternal = (vgui2::IInputInternal *)m_FactoryList[1](VGUI_INPUTINTERNAL_INTERFACE_VERSION, NULL);
	vgui2::VGuiControls_Init("BaseUI", m_FactoryList, m_iNumFactories);

	vgui2::filesystem()->AddSearchPath(".", "BASE");
	vgui2::filesystem()->AddSearchPath("platform", "PLATFORM");
	vgui2::filesystem()->AddSearchPath(gEngfuncs.pfnGetGameDirectory(), "GAME");
	vgui2::filesystem()->AddSearchPath(gEngfuncs.pfnGetGameDirectory(), "GAMECONFIG");
	vgui2::filesystem()->AddSearchPath("valve", "GAME_FALLBACK");

	char szClientDLLPath[_MAX_PATH];
	vgui2::filesystem()->GetLocalPath("cl_dlls/client.dll", szClientDLLPath, sizeof(szClientDLLPath));
	m_hClientModule = Sys_LoadModule(szClientDLLPath);
	m_FactoryList[4] = Sys_GetFactory(m_hClientModule);
	m_iNumFactories = 5;
	staticClient = (IClientVGUI *)m_FactoryList[4](CLIENTVGUI_INTERFACE_VERSION, NULL);
	gClDllFuncs.pfnInitialize(&gEngfuncs, CLDLL_INTERFACE_VERSION);
}

void CBaseUI::Start(struct cl_enginefuncs_s *engineFuncs, int interfaceVersion) {
	staticPanel = new CStaticPanel();
	staticPanel->SetVisible(false);
	staticPanel->SetZPos(0);

	staticSurface = (BaseUISurface *)m_FactoryList[0](VGUI_SURFACE_INTERFACE_VERSION, NULL);
	IHTMLChromeController *chromeController = nullptr;
	if(m_FactoryList[3])
		chromeController = (IHTMLChromeController *)m_FactoryList[3](HTML_CHROME_CONTROLLER_INTERFACE_VERSION, NULL);
	staticSurface->Init(staticPanel->GetVPanel(), chromeController);
	staticSurface->SetLanguage("english");
	staticSurface->IgnoreMouseVisibility(false);

	vgui2::scheme()->LoadSchemeFromFile("resource/TrackerScheme.res", "BaseUI");
	vgui2::localize()->AddFile(vgui2::filesystem(), "resource/tracker_%language%.txt");
	vgui2::localize()->AddFile(vgui2::filesystem(), "resource/valve_%language%.txt");

	const char *szGameDir = gEngfuncs.pfnGetGameDirectory();

	if (strcmp(szGameDir, "cstz")) {
		char szModLocalizeFile[_MAX_PATH];
		snprintf(szModLocalizeFile, _MAX_PATH, "resource/%s_%%language%%.txt", szGameDir);
		vgui2::localize()->AddFile(vgui2::filesystem(), szModLocalizeFile);
	}

	vgui2::ivgui()->Start();
	vgui2::ivgui()->SetSleep(false);

	staticClientDLLPanel = new CEnginePanel();
	staticClientDLLPanel->SetVisible(false);
	staticClientDLLPanel->SetZPos(25);

	if (staticClient) {
		staticClient->Initialize(m_FactoryList, m_iNumFactories);
	}

	staticPanel->SetScheme("ClientScheme");

	if (staticClient) {
		staticClient->Start();
	}

	int wide, tall;
	staticSurface->GetScreenSize(wide, tall);
	staticPanel->SetBounds(0, 0, wide, tall);
	staticClientDLLPanel->SetBounds(0, 0, wide, tall);

	staticSurface->IgnoreMouseVisibility(false);
}

void CBaseUI::Shutdown() {
	vgui2::ivgui()->RunFrame();

	if (staticClient) {
		staticClient->Shutdown();
	}

	vgui2::system()->SaveUserConfigFile();
	Sys_UnloadModule(m_hClientModule);
	m_hClientModule = NULL;
	staticSurface->Shutdown();
	Sys_UnloadModule(m_hChromeModule);
	m_hChromeModule = NULL;
	Sys_UnloadModule(m_hVGuiModule);
	m_hVGuiModule = NULL;
	Sys_UnloadModule(m_hFileSystemModule);
	m_hFileSystemModule = NULL;
}

int CBaseUI::Key_Event(int down, int keynum, const char * pszCurrentBinding) {
	return 0;
}

void CBaseUI::CallEngineSurfaceAppHandler(void * event, void * userData) {
}

void CBaseUI::Paint(int x, int y, int right, int bottom) {
	if (!staticSurface || !staticSurface->GetEmbeddedPanel()) {
		return;
	}

	vgui2::ivgui()->RunFrame();
	staticSurface->SetScreenBounds(x, y, right - x, bottom - y);
	staticPanel->SetBounds(0, 0, right, bottom);
	staticClientDLLPanel->SetBounds(0, 0, right, bottom);
	staticPanel->PerformApplySchemeSettings();
	vgui2::surface()->PaintTraverse(staticSurface->GetEmbeddedPanel());
}

void CBaseUI::HideGameUI() {
}

void CBaseUI::ActivateGameUI() {
}

void CBaseUI::HideConsole() {
}

void CBaseUI::ShowConsole() {
}

EXPOSE_SINGLE_INTERFACE(CBaseUI, IBaseUI, BASEUI_INTERFACE_VERSION);