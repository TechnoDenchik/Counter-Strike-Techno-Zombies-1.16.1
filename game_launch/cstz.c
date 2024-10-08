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
	#define XASHLIB    "technoengine2.dylib"
	#define dlmount(x) dlopen(x, RTLD_NOW)
	#define HINSTANCE  void*
#elif __unix__
	#include <dlfcn.h>
	#include <errno.h>
	#define XASHLIB    "technoengine2.so"
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
		#define XASHLIB "technoengine2.dll"
	#else
		#define XASHLIB "technoengine2.dll"
	#endif
	#include "windows.h"
#endif

#ifndef USE_WINMAIN // use for MSVC check here
#define _inline static inline
#endif

#ifdef WIN32
// Enable NVIDIA High Performance Graphics while using Integrated Graphics.
__declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;

// Enable AMD High Performance Graphics while using Integrated Graphics.
__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
#endif

#define GAME_PATH	"cstz"	// default dir to start from

typedef void (*pfnChangeGame)( const char *progname );
typedef int  (*pfnInit)( int argc, char **argv, const char *progname, int bChangeGame, pfnChangeGame func );
typedef void (*pfnShutdown)( void );
typedef enum { false, true } qboolean;

static pfnInit     Xash_Main;
static pfnShutdown Xash_Shutdown = NULL;
static char        szGameDir[128]; // safe place to keep gamedir
static int         szArgc;
static char        **szArgv;
static HINSTANCE	hEngine;

static void Xash_Error( const char *szFmt, ... )
{
	static char	buffer[16384];	// must support > 1k messages
	va_list		args;

	va_start( args, szFmt );
	vsnprintf( buffer, sizeof(buffer), szFmt, args );
	va_end( args );

#ifdef XASH_SDL
	SDL_ShowSimpleMessageBox( SDL_MESSAGEBOX_ERROR, "Engine Error", buffer, NULL );
#elif defined( _WIN32 )
	MessageBoxA( NULL, buffer, "Engine Error", MB_OK );
#else
	fprintf( stderr, "Engine Error: %s\n", buffer );
#endif
	exit( 1 );
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

static void Sys_LoadEngine( void )
{
	if(( hEngine = dlmount( XASHLIB )) == NULL )
	{
		Xash_Error("������ �� ��������, ����������� "XASHLIB": %s", dlerror() );
	}

	if(( Xash_Main = (pfnInit)dlsym( hEngine, "Host_Main" )) == NULL )
	{
		Xash_Error("�� ������� ����� ����� ����� �: "XASHLIB":  %s", dlerror() );
	}

	// this is non-fatal for us but change game will not working
	Xash_Shutdown = (pfnShutdown)dlsym( hEngine, "Host_Shutdown" );
}

static void Sys_UnloadEngine( void )
{
	if( Xash_Shutdown ) Xash_Shutdown( );
	if( hEngine ) dlclose( hEngine );

	Xash_Main = NULL;
	Xash_Shutdown = NULL;
}

static void Sys_ChangeGame( const char *progname )
{
	if( !progname || !progname[0] )
		Xash_Error( "����� � ��������� ���� �� ���� ����������" );

	if( Xash_Shutdown == NULL )
		Xash_Error( "����� ����� � 'Host_Shutdown' client.dll �� �������" );

	strncpy( szGameDir, progname, sizeof( szGameDir ) - 1 );

	Sys_UnloadEngine ();
	Sys_LoadEngine ();

	Xash_Main( szArgc, szArgv, szGameDir, true, Sys_ChangeGame );
}

_inline int Sys_Start( void )
{
	int ret;

	Sys_LoadEngine();
	ret = Xash_Main( szArgc, szArgv, GAME_PATH, false, Xash_Shutdown ? Sys_ChangeGame : NULL );
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
