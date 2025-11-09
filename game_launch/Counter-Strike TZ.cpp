/* =================================================================================== *
	 * =================== TechnoSoftware & Uncle Mike Developing =================== *
 * =================================================================================== */

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <stdexcept>
#include <algorithm>
#include <cstring>
#include <cstdlib>
#include <atomic>
#include <thread>
#include <chrono>
#include <ctime>
#include <filesystem>

#ifdef XASH_SDL
#include <SDL_main.h>
#include <SDL_messagebox.h>
#endif

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shellapi.h>
#include <fileapi.h>
#include <tlhelp32.h>
#elif defined(__APPLE__) || defined(__unix__)
#include <dlfcn.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <csignal>
#endif

#ifdef __linux__
#include <sys/file.h>
#endif

class SingleInstanceGuard {
private:
    static constexpr const char* APP_ID = "CounterStrikeTZ_v1.0";

#ifdef _WIN32
    HANDLE hMutex = nullptr;
#elif defined(__APPLE__) || defined(__unix__)
    int lockFileDescriptor = -1;
    std::string lockFilePath;
#endif

    std::atomic<bool> isFirstInstance{ false };

public:
    SingleInstanceGuard() {
        isFirstInstance = AcquireLock();
    }

    ~SingleInstanceGuard() {
        ReleaseLock();
    }

    SingleInstanceGuard(const SingleInstanceGuard&) = delete;
    SingleInstanceGuard& operator=(const SingleInstanceGuard&) = delete;

    bool IsFirstInstance() const {
        return isFirstInstance;
    }

private:
    bool AcquireLock() {
#ifdef _WIN32
        std::string mutexName = "Global\\" + std::string(APP_ID) + "_InstanceMutex";
        hMutex = CreateMutexA(nullptr, TRUE, mutexName.c_str());

        if (!hMutex) {
            return false;
        }

        if (GetLastError() == ERROR_ALREADY_EXISTS) {
            CloseHandle(hMutex);
            hMutex = nullptr;
            return false;
        }

        return true;

#else
        std::string tempDir = "/tmp";
        const char* xdg_runtime_dir = std::getenv("XDG_RUNTIME_DIR");
        if (xdg_runtime_dir && xdg_runtime_dir[0] != '\0') {
            tempDir = xdg_runtime_dir;
        }

        lockFilePath = tempDir + "/." + std::string(APP_ID) + ".lock";
        lockFileDescriptor = open(lockFilePath.c_str(), O_RDWR | O_CREAT, 0644);

        if (lockFileDescriptor == -1) {
            return false;
        }

#ifdef __linux__
        if (flock(lockFileDescriptor, LOCK_EX | LOCK_NB) == -1) {
            close(lockFileDescriptor);
            lockFileDescriptor = -1;
            return false;
        }
#else
        struct flock fl;
        fl.l_type = F_WRLCK;
        fl.l_whence = SEEK_SET;
        fl.l_start = 0;
        fl.l_len = 0;

        if (fcntl(lockFileDescriptor, F_SETLK, &fl) == -1) {
            close(lockFileDescriptor);
            lockFileDescriptor = -1;
            return false;
        }
#endif

        std::string pid = std::to_string(getpid());
        write(lockFileDescriptor, pid.c_str(), pid.length());
        ftruncate(lockFileDescriptor, pid.length());

        return true;
#endif
    }

    void ReleaseLock() {
#ifdef _WIN32
        if (hMutex) {
            ReleaseMutex(hMutex);
            CloseHandle(hMutex);
            hMutex = nullptr;
        }
#else
        if (lockFileDescriptor != -1) {
#ifdef __linux__
            flock(lockFileDescriptor, LOCK_UN);
#else
            struct flock fl;
            fl.l_type = F_UNLCK;
            fl.l_whence = SEEK_SET;
            fl.l_start = 0;
            fl.l_len = 0;
            fcntl(lockFileDescriptor, F_SETLK, &fl);
#endif
            close(lockFileDescriptor);
            lockFileDescriptor = -1;

            if (!lockFilePath.empty()) {
                unlink(lockFilePath.c_str());
            }
        }
#endif
    }
};

class TechnoEngineLauncher {
private:
    using pfnChangeGame = void(*)(const char* progname);
    using pfnInit = int(*)(int argc, char** argv, const char* progname, int bChangeGame, pfnChangeGame func);
    using pfnShutdown = void(*)();

    static constexpr size_t MAX_GAMEDIR_LENGTH = 127;
    static constexpr const char* GAME_PATH = "Counter-Strike-TZ";

#ifdef _WIN32
    static constexpr const char* TECHNOENGINELIB = "technoengine2.dll";
    HMODULE hEngine = nullptr;
#elif defined(__APPLE__)
    static constexpr const char* TECHNOENGINELIB = "technoengine2.dylib";
    void* hEngine = nullptr;
#elif defined(__unix__)
    static constexpr const char* TECHNOENGINELIB = "technoengine2.so";
    void* hEngine = nullptr;
#endif

    pfnInit TechnoEngine_Main = nullptr;
    pfnShutdown TechnoEngine_Shutdown = nullptr;
    std::string szGameDir;
    std::vector<std::string> arguments;

    static TechnoEngineLauncher* currentInstance;
    SingleInstanceGuard instanceGuard;

    std::atomic<bool> discordInitialized{ false };
    std::time_t gameStartTime = 0;

public:
    TechnoEngineLauncher() {
        currentInstance = this;
    }

    ~TechnoEngineLauncher() {
        UnloadEngine();
        if (currentInstance == this) {
            currentInstance = nullptr;
        }
    }

    TechnoEngineLauncher(const TechnoEngineLauncher&) = delete;
    TechnoEngineLauncher& operator=(const TechnoEngineLauncher&) = delete;

private:
    static void StaticChangeGame(const char* progname) {
        if (currentInstance && progname) {
            currentInstance->InstanceChangeGame(progname);
        }
    }

    void InstanceChangeGame(const char* progname) {
        if (!progname || !progname[0]) {
            throw std::invalid_argument("Дериктория игры повреждена");
        }

        szGameDir = std::string(progname).substr(0, MAX_GAMEDIR_LENGTH);
        UnloadEngine();
        LoadEngine();

        if (TechnoEngine_Main) {
            auto argv = GetArgv();
            TechnoEngine_Main(
                static_cast<int>(argv.size()),
                argv.data(),
                szGameDir.c_str(),
                true,
                StaticChangeGame
            );
        }
        else {
            throw std::runtime_error("Игровой каталог не обнаружен");
        }
    }

    void ShowError(const std::string& message) {
#ifdef XASH_SDL
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "TechnoEngine Error", message.c_str(), nullptr);
#elif defined(_WIN32)
        MessageBoxA(nullptr, message.c_str(), "TechnoEngine Error", MB_ICONERROR | MB_OK);
#else
        std::cerr << "TechnoEngine Error: " << message << std::endl;
#endif
    }

    void ShowWarning(const std::string& message) {
#ifdef XASH_SDL
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING, "TechnoEngine Warning", message.c_str(), nullptr);
#elif defined(_WIN32)
        MessageBoxA(nullptr, message.c_str(), "TechnoEngine Warning", MB_ICONWARNING | MB_OK);
#else
        std::cerr << "TechnoEngine Warning: " << message << std::endl;
#endif
    }

#ifdef _WIN32
    std::string GetLastErrorString() {
        DWORD errorCode = GetLastError();
        char buffer[1024];

        if (FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            nullptr, errorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            buffer, sizeof(buffer) - 1, nullptr)) {

            std::string result(buffer);
            while (!result.empty() && (result.back() == '\n' || result.back() == '\r')) {
                result.pop_back();
            }
            return result;
        }

        return "Error code: " + std::to_string(errorCode);
    }
#endif

    bool DirectoryExists(const std::string& path) {
        if (path.empty()) return false;

#ifdef _WIN32
        DWORD attrib = GetFileAttributesA(path.c_str());
        return (attrib != INVALID_FILE_ATTRIBUTES) && (attrib & FILE_ATTRIBUTE_DIRECTORY);
#else
        struct stat info;
        return stat(path.c_str(), &info) == 0 && S_ISDIR(info.st_mode);
#endif
    }

    void LoadEngine() {
        if (!DirectoryExists(GAME_PATH)) {
            throw std::runtime_error("Игровая дериктория '" + std::string(GAME_PATH) + "' не найдена");
        }

#ifdef _WIN32
        hEngine = LoadLibraryA(TECHNOENGINELIB);
        if (!hEngine) {
            throw std::runtime_error("Отсутствует модуль движка " + std::string(TECHNOENGINELIB) +
                "\nError: " + GetLastErrorString());
        }

        TechnoEngine_Main = reinterpret_cast<pfnInit>(GetProcAddress(hEngine, "Host_Main"));
        if (!TechnoEngine_Main) {
            FreeLibrary(hEngine);
            hEngine = nullptr;
            throw std::runtime_error("Точка входа не обнаружена" + std::string(TECHNOENGINELIB));
        }

        TechnoEngine_Shutdown = reinterpret_cast<pfnShutdown>(GetProcAddress(hEngine, "Host_Shutdown"));
#else
        hEngine = dlopen(TECHNOENGINELIB, RTLD_NOW);
        if (!hEngine) {
            const char* error = dlerror();
            throw std::runtime_error("Failed to load engine module: " + std::string(TECHNOENGINELIB) +
                "\nError: " + (error ? error : "Unknown error"));
        }

        TechnoEngine_Main = reinterpret_cast<pfnInit>(dlsym(hEngine, "Host_Main"));
        if (!TechnoEngine_Main) {
            dlclose(hEngine);
            hEngine = nullptr;
            throw std::runtime_error("Entry point 'Host_Main' not found in: " + std::string(TECHNOENGINELIB));
        }

        TechnoEngine_Shutdown = reinterpret_cast<pfnShutdown>(dlsym(hEngine, "Host_Shutdown"));
#endif
    }

    void UnloadEngine() {
        if (TechnoEngine_Shutdown) {
            TechnoEngine_Shutdown();
            TechnoEngine_Shutdown = nullptr;
        }

        if (hEngine) {
#ifdef _WIN32
            FreeLibrary(hEngine);
#else
            dlclose(hEngine);
#endif
            hEngine = nullptr;
        }

        TechnoEngine_Main = nullptr;
    }

    std::vector<char*> GetArgv() {
        std::vector<char*> argv;
        argv.reserve(arguments.size());
        for (auto& arg : arguments) {
            argv.push_back(arg.data());
        }
        return argv;
    }

    void ShowAlreadyRunningMessage() {
        std::string message = "Counter-Strike T:Z is already running!\n\n";
        message += "You cannot start multiple instances of the game.\n\n";
        message += "Please close the existing instance before starting a new one.";

        ShowError(message);
    }

public:
    void SetArguments(int argc, char** argv) {
        arguments.clear();
        arguments.reserve(argc);
        for (int i = 0; i < argc; ++i) {
            if (argv[i]) {
                arguments.emplace_back(argv[i]);
            }
        }
    }

#ifdef _WIN32
    void SetArgumentsFromCommandLine() {
        int argc;
        LPWSTR* argvW = CommandLineToArgvW(GetCommandLineW(), &argc);
        if (!argvW || argc <= 0) {
            throw std::runtime_error("Failed to parse command line.");
        }

        arguments.clear();
        arguments.reserve(argc);

        for (int i = 0; i < argc; ++i) {
            int size = WideCharToMultiByte(CP_UTF8, 0, argvW[i], -1, nullptr, 0, nullptr, nullptr);
            if (size > 0) {
                std::string arg;
                arg.resize(size - 1);
                WideCharToMultiByte(CP_UTF8, 0, argvW[i], -1, arg.data(), size, nullptr, nullptr);
                arguments.push_back(std::move(arg));
            }
        }

        LocalFree(argvW);
    }
#endif

    int Run() {
        try {
            // Проверка на уже запущенный экземпляр
            if (!instanceGuard.IsFirstInstance()) {
                ShowAlreadyRunningMessage();
                return 1;
            }

            LoadEngine();

            if (TechnoEngine_Main) {
                auto argv = GetArgv();
                return TechnoEngine_Main(
                    static_cast<int>(argv.size()),
                    argv.data(),
                    GAME_PATH,
                    false,
                    StaticChangeGame
                );
            }
            else {
                throw std::runtime_error("Основная функция движка недоступна.");
            }
        }
        catch (const std::exception& e) {
            ShowError(e.what());
            return 1;
        }
    }

};

TechnoEngineLauncher* TechnoEngineLauncher::currentInstance = nullptr;

// ==================== ТОЧКИ ВХОДА ====================

#ifdef _WIN32
int __stdcall WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR cmdLine, int nShow) {
    TechnoEngineLauncher launcher;
    try {
        launcher.SetArgumentsFromCommandLine();
        return launcher.Run();
    }
    catch (const std::exception& e) {
        MessageBoxA(nullptr, e.what(), "Error", MB_ICONERROR | MB_OK);
        return 1;
    }
}
#else
int main(int argc, char** argv) {
    TechnoEngineLauncher launcher;
    launcher.SetArguments(argc, argv);
    return launcher.Run();
}
#endif