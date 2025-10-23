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
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shellapi.h>
#include <tchar.h>
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
#endif

#ifndef USE_WINMAIN
#define _inline static inline
#endif

#ifdef WIN32
__declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
#endif

#define GAME_PATH    "Counter-Strike-TZ"    
#define MAX_GAMEDIR_LENGTH 127

typedef void (*pfnChangeGame)(const char* progname);
typedef int  (*pfnInit)(int argc, char** argv, const char* progname, int bChangeGame, pfnChangeGame func);
typedef void (*pfnShutdown)(void);
typedef enum { false, true } qboolean;

static pfnInit     TechnoEngine_Main;
static pfnShutdown TechnoEngine_Shutdown = NULL;
static char        szGameDir[MAX_GAMEDIR_LENGTH + 1];
static int         szArgc;
static char** szArgv;
static HINSTANCE   hEngine;
static HANDLE      hGameMutex = NULL;

static void TechnoEngine_Error(const char* szFmt, ...)
{
    static char    buffer[16384];
    va_list        args;

    va_start(args, szFmt);
    vsnprintf(buffer, sizeof(buffer), szFmt, args);
    va_end(args);

#ifdef XASH_SDL
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "TechnoEngine Error", buffer, NULL);
#elif defined( _WIN32 )
    MessageBoxA(NULL, buffer, "TechnoEngine Error", MB_ICONERROR | MB_OK);
#else
    fprintf(stderr, "TechnoEngine Error: %s\n", buffer);
#endif
    exit(1);
}

static void TechnoEngine_Warning(const char* szFmt, ...)
{
    static char    buffer[16384];
    va_list        args;

    va_start(args, szFmt);
    vsnprintf(buffer, sizeof(buffer), szFmt, args);
    va_end(args);

#ifdef XASH_SDL
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING, "TechnoEngine Warning", buffer, NULL);
#elif defined( _WIN32 )
    MessageBoxA(NULL, buffer, "TechnoEngine Warning", MB_ICONWARNING | MB_OK);
#else
    fprintf(stderr, "TechnoEngine Warning: %s\n", buffer);
#endif
}

static void TechnoEngine_Info(const char* szFmt, ...)
{
    static char    buffer[16384];
    va_list        args;

    va_start(args, szFmt);
    vsnprintf(buffer, sizeof(buffer), szFmt, args);
    va_end(args);

#ifdef XASH_SDL
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "TechnoEngine Information", buffer, NULL);
#elif defined( _WIN32 )
    MessageBoxA(NULL, buffer, "TechnoEngine Information", MB_ICONINFORMATION | MB_OK);
#else
    fprintf(stderr, "TechnoEngine Information: %s\n", buffer);
#endif
}

#ifdef _WIN32
static const char* GetStringLastError()
{
    static char buf[1024];
    DWORD dwError = GetLastError();

    if (FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, dwError, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        buf, sizeof(buf) - 1, NULL))
    {
        // Убираем перевод строки в конце
        char* p = buf + strlen(buf) - 1;
        while (p >= buf && (*p == '\n' || *p == '\r'))
            *p-- = '\0';
        return buf;
    }

    snprintf(buf, sizeof(buf), "Error code: %lu", dwError);
    return buf;
}
#endif

static HANDLE CheckInstance(const char* Name)
{
    HANDLE hMutex = CreateMutexA(NULL, TRUE, Name);
    if (hMutex == NULL)
        return NULL;

    if (GetLastError() == ERROR_ALREADY_EXISTS)
    {
        CloseHandle(hMutex);
        return NULL;
    }

    return hMutex;
}

static BOOL DirIsExist(const char* dir)
{
    if (!dir || !dir[0])
        return FALSE;

    DWORD dwAttrib = GetFileAttributesA(dir);
    if (dwAttrib == INVALID_FILE_ATTRIBUTES)
        return FALSE;

    return (dwAttrib & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

static void Sys_LoadEngine(void)
{
    const char* NamedMutex = "Counter-Strike T:Z";

    // Проверяем существование директории игры
    if (!DirIsExist("Counter-Strike-TZ"))
    {
        TechnoEngine_Error("Game directory 'Counter-Strike-TZ' not found.\nPlease make sure the game is installed correctly.");
        return;
    }

    // Проверяем, не запущена ли игра уже
    hGameMutex = CheckInstance(NamedMutex);
    if (!hGameMutex)
    {
        // Игра уже запущена - активируем существующее окно
        HWND hWnd = FindWindowA(NULL, "Counter-Strike T:Z");
        if (hWnd)
        {
            SetForegroundWindow(hWnd);
            ShowWindow(hWnd, SW_RESTORE);
        }
        TechnoEngine_Info("Game is already running. Bringing existing window to foreground.");
        exit(0);
    }

    // Загружаем движок
    hEngine = dlmount(TECHNOENGINELIB);
    if (hEngine == NULL)
    {
        if (hGameMutex)
        {
            ReleaseMutex(hGameMutex);
            CloseHandle(hGameMutex);
            hGameMutex = NULL;
        }
        TechnoEngine_Error("Failed to load engine module: %s\nError: %s", TECHNOENGINELIB, dlerror());
    }

    // Получаем точку входа
    TechnoEngine_Main = (pfnInit)dlsym(hEngine, "Host_Main");
    if (TechnoEngine_Main == NULL)
    {
        if (hGameMutex)
        {
            ReleaseMutex(hGameMutex);
            CloseHandle(hGameMutex);
            hGameMutex = NULL;
        }
        dlclose(hEngine);
        hEngine = NULL;
        TechnoEngine_Error("Entry point 'Host_Main' not found in: %s", TECHNOENGINELIB);
    }

    // Получаем функцию завершения
    TechnoEngine_Shutdown = (pfnShutdown)dlsym(hEngine, "Host_Shutdown");

    // Мьютекс не освобождаем - он будет освобожден при завершении процесса
}

static void Sys_UnloadEngine(void)
{
    if (TechnoEngine_Shutdown)
    {
        TechnoEngine_Shutdown();
        TechnoEngine_Shutdown = NULL;
    }

    if (hEngine)
    {
        dlclose(hEngine);
        hEngine = NULL;
    }

    if (hGameMutex)
    {
        ReleaseMutex(hGameMutex);
        CloseHandle(hGameMutex);
        hGameMutex = NULL;
    }

    TechnoEngine_Main = NULL;
}

static void Sys_ChangeGame(const char* progname)
{
    if (!progname || !progname[0])
    {
        TechnoEngine_Error("Invalid game directory specified.");
        return;
    }

    if (!TechnoEngine_Shutdown)
    {
        TechnoEngine_Error("Engine shutdown function not available.");
        return;
    }

    // Безопасное копирование пути к игре
    strncpy(szGameDir, progname, MAX_GAMEDIR_LENGTH);
    szGameDir[MAX_GAMEDIR_LENGTH] = '\0';

    // Перезагружаем движок с новой игрой
    Sys_UnloadEngine();
    Sys_LoadEngine();

    if (TechnoEngine_Main)
    {
        TechnoEngine_Main(szArgc, szArgv, szGameDir, true, Sys_ChangeGame);
    }
    else
    {
        TechnoEngine_Error("Failed to restart engine with new game directory.");
    }
}

_inline int Sys_Start(void)
{
    int ret = 1; // Значение по умолчанию при ошибке

    // Загружаем движок
    Sys_LoadEngine();

    // Запускаем основную функцию
    if (TechnoEngine_Main)
    {
        pfnChangeGame changeGameFunc = TechnoEngine_Shutdown ? Sys_ChangeGame : NULL;
        ret = TechnoEngine_Main(szArgc, szArgv, GAME_PATH, false, changeGameFunc);
    }
    else
    {
        TechnoEngine_Error("Engine main function not available.");
    }

    // Выгружаем движок
    Sys_UnloadEngine();

    return ret;
}

#ifndef USE_WINMAIN
int main(int argc, char** argv)
{
    szArgc = argc;
    szArgv = argv;

    return Sys_Start();
}
#else
int __stdcall WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR cmdLine, int nShow)
{
    LPWSTR* lpArgv = NULL;
    int ret = 1;
    int i;

    // Парсим командную строку
    lpArgv = CommandLineToArgvW(GetCommandLineW(), &szArgc);
    if (!lpArgv || szArgc <= 0)
    {
        TechnoEngine_Error("Failed to parse command line.");
        return 1;
    }

    // Выделяем память для аргументов
    szArgv = (char**)malloc(szArgc * sizeof(char*));
    if (!szArgv)
    {
        LocalFree(lpArgv);
        TechnoEngine_Error("Out of memory.");
        return 1;
    }

    // Конвертируем аргументы в UTF-8
    for (i = 0; i < szArgc; ++i)
    {
        int size = WideCharToMultiByte(CP_UTF8, 0, lpArgv[i], -1, NULL, 0, NULL, NULL);
        if (size <= 0)
        {
            szArgv[i] = _strdup("");
            continue;
        }

        szArgv[i] = (char*)malloc(size);
        if (szArgv[i])
        {
            WideCharToMultiByte(CP_UTF8, 0, lpArgv[i], -1, szArgv[i], size, NULL, NULL);
        }
        else
        {
            szArgv[i] = _strdup("");
        }
    }

    LocalFree(lpArgv);

    // Запускаем приложение
    ret = Sys_Start();

    // Освобождаем память
    for (i = 0; i < szArgc; ++i)
    {
        if (szArgv[i])
            free(szArgv[i]);
    }
    free(szArgv);

    return ret;
}
#endif