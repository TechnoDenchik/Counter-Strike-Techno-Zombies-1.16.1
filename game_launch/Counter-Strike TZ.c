/* =================================================================================== *
	 * =================== TechnoSoftware & Uncle Mike Developing =================== *
 * =================================================================================== */

#ifdef XASH_SDL
#include <SDL_main.h>
#include <SDL_messagebox.h>
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#ifdef __APPLE__
	#include <dlfcn.h>
	#include <errno.h>
	#define TECHNOENGINELIB    "technoengine2.dylib"
	#define dlmount(x) dlopen(x, RTLD_NOW)
	#define HINSTANCE  void*
#elif __unix__
	#include <dlfcn.h>
	#include <errno.h>
	#define TECHNOENGINELIB    "technoengine2.so"
	#define dlmount(x) dlopen(x, RTLD_NOW)
	#define HINSTANCE  void*
#elif _WIN32
	#define dlmount(x) LoadLibraryA(x)
	#define dlclose(x) FreeLibrary(x)
	#define dlsym(x,y) GetProcAddress(x,y)
	#define dlerror()  GetStringLastError()
	#if !__MINGW32__ && _MSC_VER >= 1200
		#define USE_WINMAIN
	#endif
	#ifndef XASH_DEDICATED
		#define TECHNOENGINELIB "technoengine2.dll"
	#else
		#define TECHNOENGINELIB "technoengine2.dll"
	#endif
	#include "windows.h"
#endif

#ifndef USE_WINMAIN
#define _inline static inline
#endif

#ifdef WIN32
__declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
__declspec(dllexport) void glGetString(GL_VENDOR);
__declspec(dllexport) void glGetString(GL_RENDERER);
#endif

#define GAME_PATH	"Counter-Strike-TZ"	

typedef void (*pfnChangeGame)( const char *progname );
typedef int  (*pfnInit)( int argc, char **argv, const char *progname, int bChangeGame, pfnChangeGame func );
typedef void (*pfnShutdown)( void );
typedef enum { false, true } qboolean;

static pfnInit     TechnoEngine_Main;
static pfnShutdown TechnoEngine_Shutdown = NULL;
static char        szGameDir[128];
static int         szArgc;
static char        **szArgv;
static HINSTANCE	hEngine;
BOOL gamegir;

static void TechnoEngine_Error( const char *szFmt, ... )
{
	static char	buffer[16384];
	va_list		args;

	va_start( args, szFmt );
	vsnprintf( buffer, sizeof(buffer), szFmt, args );
	va_end( args );

#ifdef XASH_SDL
	SDL_ShowSimpleMessageBox( SDL_MESSAGEBOX_ERROR, "TechnoEngine Error", buffer, NULL );
#elif defined( _WIN32 )
	MessageBoxA( NULL, buffer, "TechnoEngine Error", MB_ICONERROR);
#else
	fprintf( stderr, "TechnoEngine Error: %s\n", buffer );
#endif
	exit( 1 );
	_exit(1);
}

static void TechnoEngine_Warning(const char* szFmt, ...)
{
	static char	buffer[16384];
	va_list		args;

	va_start(args, szFmt);
	vsnprintf(buffer, sizeof(buffer), szFmt, args);
	va_end(args);

#ifdef XASH_SDL
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "TechnoEngine Message", buffer, NULL);
#elif defined( _WIN32 )
	MessageBoxA(NULL, buffer, "TechnoEngine Message", MB_ICONINFORMATION);
#else
	fprintf(stderr, "TechnoEngine Message: %s\n", buffer);
#endif
	exit(1);
	_exit(1);
}

static void TechnoEngine_Warning2(const char* szFmt, ...)
{
	static char	buffer[16384];
	va_list		args;

	va_start(args, szFmt);
	vsnprintf(buffer, sizeof(buffer), szFmt, args);
	va_end(args);

#ifdef XASH_SDL
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "TechnoEngine Message", buffer, NULL);
#elif defined( _WIN32 )
	MessageBoxA(NULL, buffer, "TechnoEngine Message", MB_ICONWARNING);
#else
	fprintf(stderr, "TechnoEngine Message: %s\n", buffer);
#endif
	exit(1);
	_exit(1);
}

#ifdef _WIN32
static const char *GetStringLastError()
{
	static char buf[1024];

	FormatMessageA( FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, GetLastError(), MAKELANGID( LANG_ENGLISH, SUBLANG_DEFAULT ),
		buf, sizeof( buf ), NULL );

	return buf;
}
#endif
HANDLE CheckInstance(const char* Name)
{
	HANDLE Mutex = CreateMutex(NULL, true, Name);
	int er = GetLastError();
	if (er) return 0;
	return Mutex;
}

BOOL DirIsExist(const TCHAR* dir) 
{
	DWORD flag = GetFileAttributes(dir);
	if (flag == 0xFFFFFFFFUL) {
		if (GetLastError() == ERROR_FILE_NOT_FOUND)
			return false;
	}
	if (!(flag & FILE_ATTRIBUTE_DIRECTORY))
		return false;
	return true;
}

static void Sys_LoadEngine( void )
{
	const char* NamedMutex = "Counter-Strike T:Z";

	if (DirIsExist("Counter-Strike-TZ"))
	{
		if (DirIsExist("Counter-Strike-TZ/cl_dlls"))
		{
			if (DirIsExist("Counter-Strike-TZ/dlls"))
			{
				if (DirIsExist("Counter-Strike-TZ/media"))
				{
					HANDLE Mutex = CheckInstance(NamedMutex);

					if (!Mutex)
					{
						ReleaseMutex(Mutex);
						TechnoEngine_Warning("Игра запущена!");
						HWND hWnd = FindWindow(0, "Counter-Strike T:Z");
						SetForegroundWindow(hWnd);
						return 1;
					}
					else
					{
						if ((hEngine = dlmount(TECHNOENGINELIB)) == NULL)
						{
							TechnoEngine_Error("Системе не удалось обнаружить модуль "TECHNOENGINELIB" в директории игры. Error:0x000027t", dlerror());
						}

						if ((TechnoEngine_Main = (pfnInit)dlsym(hEngine, "Host_Main")) == NULL)
						{
							TechnoEngine_Error("Точка входа в функции $@Sys_start(void),$@Sys_unloadEndine(void) не выполнена. Error:0x000127t"TECHNOENGINELIB"", dlerror());
						}

						TechnoEngine_Shutdown = (pfnShutdown)dlsym(hEngine, "Host_Shutdown");
					}
				}
				else
				{
					TechnoEngine_Warning2("Папка медиаплеера Counter-Strike-TZ/media не обнаружена Error:0x000427g", dlerror());
				}
			}
			else
			{
				TechnoEngine_Warning2("Папка сервера Counter-Strike-TZ/dlls не найдена Error:0x000327g", dlerror());
			}
		}
		else
		{
			TechnoEngine_Warning2("Папка клиента Counter-Strike-TZ/cl_dlls не найдена Error:0x000127g", dlerror());
		}
	}
	else
	{
		TechnoEngine_Warning2("Папка с ресурсами игры не была обнаружена Error:0x000027g", dlerror());
	}
}

static void Sys_UnloadEngine( void )
{
	if(TechnoEngine_Shutdown ) TechnoEngine_Shutdown( );
	if( hEngine ) dlclose( hEngine );

	TechnoEngine_Main = NULL;
	TechnoEngine_Shutdown = NULL;
}

static void Sys_ChangeGame( const char *progname )
{
	if( !progname || !progname[0] )
		TechnoEngine_Error( "Папка с ресурсами игры не была обнаружена" );

	if(TechnoEngine_Shutdown == NULL )
		TechnoEngine_Error( "Точка входа в 'Host_Shutdown' client.dll не найдена" );

	strncpy(szGameDir, progname, sizeof(szGameDir) - 1);

	Sys_UnloadEngine ();
	Sys_LoadEngine ();

	TechnoEngine_Main( szArgc, szArgv, szGameDir, true, Sys_ChangeGame);
}

_inline int Sys_Start( void )
{
	int ret;

	Sys_LoadEngine();
	ret = TechnoEngine_Main( szArgc, szArgv, GAME_PATH, false, TechnoEngine_Shutdown ? Sys_ChangeGame : NULL );
	Sys_UnloadEngine();

	return ret;
}

#ifndef USE_WINMAIN
int main( int argc, char **argv )
{
	szArgc = argc;
	szArgv = argv;

	return Sys_Start();
}
#else
//#pragma comment(lib, "shell32.lib")
int __stdcall WinMain( HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR cmdLine, int nShow )
{
	LPWSTR* lpArgv;
	int ret, i;

	lpArgv = CommandLineToArgvW( GetCommandLineW(), &szArgc );
	szArgv = ( char** )malloc( szArgc * sizeof( char* ));

	for( i = 0; i < szArgc; ++i )
	{
		int size = wcslen(lpArgv[i]) + 1;
		szArgv[i] = ( char* )malloc( size );
		wcstombs( szArgv[i], lpArgv[i], size );
	}

	LocalFree( lpArgv );

	ret = Sys_Start();

	for( ; i < szArgc; ++i )
		free( szArgv[i] );
	free( szArgv );

	return ret;
}
#endif
