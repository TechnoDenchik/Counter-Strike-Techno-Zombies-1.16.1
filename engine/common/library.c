/*
library.c - custom dlls loader
Copyright (C) 2008 Uncle Mike

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
*/

#define _GNU_SOURCE

#include "common.h"
#include "library.h"
#include "filesystem.h"
#include "server.h"

char lasterror[1024] = "";
const char *Com_GetLibraryError()
{
	return lasterror;
}

void Com_ResetLibraryError()
{
	lasterror[0] = 0;
}

void Com_PushLibraryError( const char *error )
{
	Q_strncat( lasterror, error, sizeof( lasterror ) );
	Q_strncat( lasterror, "\n", sizeof( lasterror ) );
}

void *Com_FunctionFromName_SR( void *hInstance, const char *pName )
{
#ifdef XASH_ALLOW_SAVERESTORE_OFFSETS
	if( !Q_memcmp( pName, "ofs:",4 ) )
		return (void*)((intptr_t)svgame.dllFuncs.pfnGameInit + (ptrdiff_t)Q_atoi(pName + 4));
#endif
	return Com_FunctionFromName( hInstance, pName );
}

#ifdef XASH_ALLOW_SAVERESTORE_OFFSETS
char *Com_OffsetNameForFunction( void *function )
{
	static string sname;
	Q_snprintf( sname, MAX_STRING, "ofs:%d", (int)((intptr_t)function - (intptr_t)svgame.dllFuncs.pfnGameInit) );
	MsgDev( D_NOTE, "Com_OffsetNameForFunction %s\n", sname );
	return sname;
}
#endif

#ifdef XASH_WINRT

void* Com_LoadLibrary(const char* dllname, int build_ordinals_table)
{
	wchar_t buffer[MAX_PATH];
	MultiByteToWideChar(CP_ACP, 0, dllname, -1, buffer, MAX_PATH);
	return LoadPackagedLibrary(buffer, 0);
}
void Com_FreeLibrary(void* hInstance)
{
	FreeLibrary(hInstance);
}


void* Com_GetProcAddress(void* hInstance, const char* name)
{
	return GetProcAddress(hInstance, name);
}

void* Com_FunctionFromName(void* hInstance, const char* name)
{
	return GetProcAddress(hInstance, name);
}

const char* Com_NameForFunction(void* hInstance, void* function)
{
	return NULL;
}

#elif !defined(_WIN32)

#ifdef __ANDROID__
#include "platform/android/dlsym-weak.h"
#endif


#ifdef NO_LIBDL

#ifndef DLL_LOADER
#error Enable at least one dll backend!!!
#endif

void *dlsym(void *handle, const char *symbol )
{
	MsgDev( D_NOTE, "dlsym( %p, \"%s\" ): stub\n", handle, symbol );
	return NULL;
}
void *dlopen(const char *name, int flag )
{
	MsgDev( D_NOTE, "dlopen( \"%s\", %d ): stub\n", name, flag );
	return NULL;
}
int dlclose(void *handle)
{
	MsgDev( D_NOTE, "dlsym( %p ): stub\n", handle );
	return 0;
}
char *dlerror( void )
{
	return "Loading ELF libraries not supported in this build!\n";
}
int dladdr( const void *addr, Dl_info *info )
{
	return 0;
}



#endif
#ifdef XASH_SDL
#include <SDL_filesystem.h>
#endif

#if TARGET_OS_IPHONE

static void *IOS_LoadLibraryInternal( const char *dllname )
{
	void *pHandle;
	string errorstring = "";
	char path[MAX_SYSPATH];
	
	// load frameworks from Documents directory
	// frameworks should be signed with same key with application
	// Useful for debug to prevent rebuilding app on every library update
	// NOTE: Apple polices forbids loading code from shared places
#ifdef ENABLE_FRAMEWORK_SIDELOAD
	Q_snprintf( path, MAX_SYSPATH, "%s.framework/lib", dllname );
	if( pHandle = dlopen( path, RTLD_NOW ) )
		return pHandle;
	Q_snprintf( errorstring, MAX_STRING, dlerror() );
#endif
	
#ifdef DLOPEN_FRAMEWORKS
	// load frameworks as it should be located in Xcode builds
	Q_snprintf( path, MAX_SYSPATH, "%s%s.framework/lib", SDL_GetBasePath(), dllname );
#else
	// load libraries from app root to allow re-signing ipa with custom utilities
	Q_snprintf( path, MAX_SYSPATH, "%s%s", SDL_GetBasePath(), dllname );
#endif
	pHandle = dlopen( path, RTLD_NOW );
	if( !pHandle )
	{
		Com_PushLibraryError(errorstring);
		Com_PushLibraryError(dlerror());
	}
	return pHandle;
}
extern char *g_szLibrarySuffix;
static void *IOS_LoadLibrary( const char *dllname )
{

	string name;
	char *postfix = g_szLibrarySuffix;
	char *pHandle;

	if( !postfix ) postfix = GI->gamefolder;

	Q_snprintf( name, MAX_STRING, "%s_%s", dllname, postfix );
	pHandle = IOS_LoadLibraryInternal( name );
	if( pHandle )
		return pHandle;
	return IOS_LoadLibraryInternal( dllname );
}

#endif

void *Com_LoadLibrary( const char *dllname, int build_ordinals_table )
{
	dll_user_t *hInst = NULL;
	void *pHandle = NULL;

	// platforms where gameinfo mechanism is impossible
	// or not implemented
#if TARGET_OS_IPHONE
	{
		return IOS_LoadLibrary( dllname );
	}
#elif defined( __EMSCRIPTEN__ )
	{
#ifdef EMSCRIPTEN_LIB_FS
		char path[MAX_SYSPATH];
		string prefix;
		Q_strcpy(prefix, getenv( "LIBRARY_PREFIX" ) );
		Q_snprintf( path, MAX_SYSPATH, "%s%s%s",  prefix, dllname, getenv( "LIBRARY_SUFFIX" ) );
		pHandle = dlopen( path, RTLD_NOW );
		if( !pHandle )
		{
			Com_PushLibraryError( va("Loading %s:\n", path ) );
			Com_PushLibraryError( dlerror() );
		}
		return pHandle;
#else
		// get handle of preloaded library outside fs
		return EM_ASM_INT( return DLFCN.loadedLibNames[Pointer_stringify($0)], (int)dllname );
#endif
	}
#elif defined( __ANDROID__ )
	{
		char path[MAX_SYSPATH];
		const char *libdir[2];
		int i;

		libdir[0] = getenv("XASH3D_GAMELIBDIR");
		libdir[1] = getenv("XASH3D_ENGLIBDIR");

		for( i = 0; i < 2; i++ )
		{
			Q_snprintf( path, MAX_SYSPATH, "%s/lib%s"POSTFIX"."OS_LIB_EXT, libdir[i], dllname );
			pHandle = dlopen( path, RTLD_NOW );
			if( pHandle )
				return pHandle;

			Com_PushLibraryError( dlerror() );
		}

		// HACKHACK: keep old behaviour for compatibility
		pHandle = dlopen( dllname, RTLD_NOW );
		if( pHandle )
			return pHandle;

		Com_PushLibraryError( dlerror() );
	}
#elif defined __SAILFISH__
	{
		char path[MAX_SYSPATH];
		const char *libdir[2];
		int i;

		// try from game library directory at first
		libdir[0] = va( LIBPATH"/gamelibs/%s", GI ? GI->gamefolder : "" );

		// try from engine path
		libdir[1] = LIBPATH;

		for( i = 0; i < 2; i++ )
		{
			Q_snprintf( path, MAX_SYSPATH, "%s/lib%s."OS_LIB_EXT, libdir[i], dllname );
			pHandle = dlopen( path, RTLD_NOW );
			if( pHandle )
				return pHandle;

			Com_PushLibraryError( dlerror() );
		}
	}
#elif defined __HAIKU__
	// First look for libraries in the mirror directory
 	const char *libdir = getenv( "XASH3D_MIRRORDIR" );
 	if( libdir ) {
 		char path[MAX_SYSPATH];
 		char game[MAX_SYSPATH] = { 0 };
 		if( GI && !Q_strstr( dllname, "menu" ) )
 			Q_snprintf( game, MAX_SYSPATH, "/%s", GI->gamefolder );
 		Q_snprintf( path, MAX_SYSPATH, "%s%s/%s", libdir, game, dllname );
 		// fprintf( stderr, "===> %s: %s\n", dllname, path );
 		pHandle = dlopen( path, RTLD_NOW );
 		if( pHandle )
 			return pHandle;

  		Com_PushLibraryError( dlerror() );
 	}
 	// Then through FS_FindLibrary() function in the gamebase directory
#endif

	// platforms where gameinfo mechanism is working goes here
	// and use FS_FindLibrary
	hInst = FS_FindLibrary( dllname, false );
	if( !hInst )
	{
		// HACKHACK: direct load dll
#ifdef DLL_LOADER
		if( host.enabledll && ( pHandle = Loader_LoadLibrary(dllname)) )
		{
			return pHandle;
		}
#endif

		// try to find by linker(LD_LIBRARY_PATH, DYLD_LIBRARY_PATH, LD_32_LIBRARY_PATH and so on...)
		if( !pHandle )
		{
			pHandle = dlopen( dllname, RTLD_NOW );
			if( pHandle )
				return pHandle;

			Com_PushLibraryError( va( "Failed to find library %s", dllname ));
			Com_PushLibraryError( dlerror() );
			return NULL;
		}
	}

	if( hInst->custom_loader )
	{
		Com_PushLibraryError( va( "Custom library loader is not available. Extract library %s and fix gameinfo.txt!", hInst->fullPath ));
		Mem_Free( hInst );
		return NULL;
	}

#ifdef DLL_LOADER
	if( host.enabledll && ( !Q_stricmp( FS_FileExtension( hInst->shortPath ), "dll" ) ) )
	{
		if( hInst->encrypted )
		{
			Com_PushLibraryError( va( "Library %s is encrypted. Cannot load", hInst->shortPath ) );
			Mem_Free( hInst );
			return NULL;
		}

		if( !( hInst->hInstance = Loader_LoadLibrary( hInst->fullPath ) ) )
		{
			Com_PushLibraryError( va( "Failed to load DLL with DLL loader: %s", hInst->shortPath ) );
			Mem_Free( hInst );
			return NULL;
		}
	}
	else
#endif
	{
		if( !( hInst->hInstance = dlopen( hInst->fullPath, RTLD_NOW ) ) )
		{
			Com_PushLibraryError( dlerror() );
			Mem_Free( hInst );
			return NULL;
		}
	}

	pHandle = hInst->hInstance;

	Mem_Free( hInst );

	return pHandle;
}

void Com_FreeLibrary( void *hInstance )
{
#if !defined __EMSCRIPTEN__ || defined EMSCRIPTEN_LIB_FS
#ifdef DLL_LOADER
	void *wm;
	if( host.enabledll && (wm = Loader_GetDllHandle( hInstance )) )
		return Loader_FreeLibrary( hInstance );
	else
#endif
		dlclose( hInstance );
#endif
}

void *Com_GetProcAddress( void *hInstance, const char *name )
{
#ifdef DLL_LOADER
	void *wm;
	if( host.enabledll && (wm = Loader_GetDllHandle( hInstance )) )
		return Loader_GetProcAddress(hInstance, name);
	else
#endif
	return dlsym( hInstance, name );
}

void *Com_FunctionFromName( void *hInstance, const char *pName )
{
	void *function;
#ifdef DLL_LOADER
	void *wm;
	if( host.enabledll && (wm = Loader_GetDllHandle( hInstance )) )
		return Loader_GetProcAddress(hInstance, pName);
	else
#endif
	if( !( function = dlsym( hInstance, pName ) ) )
	{
		MsgDev(D_ERROR, "FunctionFromName: Can't get symbol %s: %s\n", pName, dlerror());
	}
	return function;
}

#ifdef XASH_DYNAMIC_DLADDR
int d_dladdr( void *sym, Dl_info *info )
{
	static int (*dladdr_real) ( void *sym, Dl_info *info );

	if( !dladdr_real )
		dladdr_real = dlsym( (void*)(size_t)(-1), "dladdr" );

	Q_memset( info, 0, sizeof( *info ) );

	if( !dladdr_real )
		return -1;

	return dladdr_real(  sym, info );
}
#endif

const char *Com_NameForFunction( void *hInstance, void *function )
{
#ifdef DLL_LOADER
	void *wm;
	if( host.enabledll && (wm = Loader_GetDllHandle( hInstance )) )
		return Loader_GetFuncName_int(wm, function);
	else
#endif
	// Note: dladdr() is a glibc extension
	{
		Dl_info info = {0};
		dladdr((void*)function, &info);
		if(info.dli_sname)
			return info.dli_sname;
	}
#ifdef XASH_ALLOW_SAVERESTORE_OFFSETS
	return Com_OffsetNameForFunction( function );
#else
	return NULL;
#endif
}
#elif defined XASH_64BIT
#include <dbghelp.h>
void *Com_LoadLibrary( const char *dllname, int build_ordinals_table )
{
	dll_user_t *hInst;
	string fullPath;
	hInst = FS_FindLibrary(dllname, false);
	if (!hInst)
		return NULL;

	strcpy(fullPath, hInst->fullPath);
	Mem_Free(hInst);

	return LoadLibraryA(fullPath);
}

void Com_FreeLibrary( void *hInstance )
{
	FreeLibrary( hInstance );
}

void *Com_GetProcAddress( void *hInstance, const char *name )
{
	return GetProcAddress( hInstance, name );
}

void *Com_FunctionFromName( void *hInstance, const char *name )
{
	return GetProcAddress( hInstance, name );
}

const char *Com_NameForFunction( void *hInstance, void *function )
{
#if 0
	static qboolean initialized = false;
	if( initialized )
	{
		char message[1024];
		int len = 0;
		size_t i;
		HANDLE process = GetCurrentProcess();
		HANDLE thread = GetCurrentThread();
		IMAGEHLP_LINE64 line;
		DWORD dline = 0;
		DWORD options;
		CONTEXT context;
		STACKFRAME64 stackframe;
		DWORD image;
		char buffer[sizeof( IMAGEHLP_SYMBOL64) + MAX_SYM_NAME * sizeof(TCHAR)];
		PIMAGEHLP_SYMBOL64 symbol = ( PIMAGEHLP_SYMBOL64)buffer;
		memset( symbol, 0, sizeof(IMAGEHLP_SYMBOL64) + MAX_SYM_NAME );
		symbol->SizeOfStruct = sizeof( IMAGEHLP_SYMBOL64);
		symbol->MaxNameLength = MAX_SYM_NAME;
		DWORD displacement = 0;

		options = SymGetOptions();
		SymSetOptions( options );

		SymInitialize( process, NULL, TRUE );

		if( SymGetSymFromAddr64( process, function, &displacement, symbol ) )
		{
			Msg( "%s\n", symbol->Name );
			return copystring( symbol->Name );
		}

	}
#endif

#ifdef XASH_ALLOW_SAVERESTORE_OFFSETS
	return Com_OffsetNameForFunction( function );
#endif

	return NULL;
}

#else
/*
---------------------------------------------------------------

		Custom dlls loader

---------------------------------------------------------------
*/

#ifndef IMAGE_SIZEOF_BASE_RELOCATION
// Vista SDKs no longer define IMAGE_SIZEOF_BASE_RELOCATION!?
#define IMAGE_SIZEOF_BASE_RELOCATION (sizeof(IMAGE_BASE_RELOCATION))
#endif

#if defined(_M_X64)
#error "Xash's nonstandart loader will not work on Win64. Set target to Win32 or disable nonstandart loader"
#endif


typedef struct
{
	PIMAGE_NT_HEADERS	headers;
	byte		*codeBase;
	void		**modules;
	int		numModules;
	int		initialized;
	DWORD magic;           // Magic number для проверки
} MEMORYMODULE, *PMEMORYMODULE;

// Protection flags for memory pages (Executable, Readable, Writeable)
static int ProtectionFlags[2][2][2] =
{
{
{ PAGE_NOACCESS, PAGE_WRITECOPY },		// not executable
{ PAGE_READONLY, PAGE_READWRITE },
},
{
{ PAGE_EXECUTE, PAGE_EXECUTE_WRITECOPY },	// executable
{ PAGE_EXECUTE_READ, PAGE_EXECUTE_READWRITE },
},
};

typedef BOOL (WINAPI *DllEntryProc)( HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved );

#define GET_HEADER_DICTIONARY( module, idx )	&(module)->headers->OptionalHeader.DataDirectory[idx]
#define CALCULATE_ADDRESS( base, offset )	(((DWORD)(base)) + (offset))

// Добавьте эти определения в начало файла или в заголовочный файл
#define MAX_SECTION_SIZE (100 * 1024 * 1024) // 100MB максимум для секции
#define IS_VALID_POINTER(ptr) ((ptr) != NULL && !IsBadReadPtr((ptr), sizeof(*(ptr))))
#define IS_VALID_POINTER_RANGE(ptr, size) ((ptr) != NULL && !IsBadReadPtr((ptr), (size)))
#define MAX_EXPORT_NAME_LENGTH 1024

// Добавьте эти определения
#define MAX_DLL_NAME_LENGTH 256
#define MAX_FUNCTION_NAME_LENGTH 1024
#define MAX_IMPORT_TABLE_SIZE (64 * 1024) // 64KB максимум для таблицы импорта

// Добавьте в структуру MEMORYMODULE
#define MEMORYMODULE_MAGIC 0x4D4F444D // "MODM"

// Функции проверки
static BOOL IsValidCodeBase(void* codeBase)
{
	if (!codeBase) return FALSE;

	MEMORY_BASIC_INFORMATION mbi;
	if (VirtualQuery(codeBase, &mbi, sizeof(mbi)) == 0)
		return FALSE;

	// Проверяем что память была выделена нами (MEM_COMMIT)
	return (mbi.State == MEM_COMMIT && mbi.AllocationBase == codeBase);
}

static BOOL IsValidLibraryHandle(void* handle)
{
	if (!handle) return FALSE;

#ifdef _WIN32
	// Проверка HMODULE через GetModuleFileName
	CHAR path[MAX_PATH];
	DWORD length = GetModuleFileNameA((HMODULE)handle, path, sizeof(path));
	return (length > 0 && length < sizeof(path));
#else
	// Для Linux можно проверить через dlsym
	void* symbol = dlsym(handle, "dlsym"); // Любая известная функция
	return (symbol != NULL);
#endif
}

#define IS_VALID_CODE_POINTER(ptr) ((ptr) != NULL && !IsBadCodePtr( (FARPROC)(ptr) ))

// Безопасная версия FreeSections
static void FreeSections(PIMAGE_NT_HEADERS headers, MEMORYMODULE* module)
{
	if (!headers || !module || !module->codeBase)
		return;

	PIMAGE_SECTION_HEADER section = IMAGE_FIRST_SECTION(headers);

	for (int i = 0; i < headers->FileHeader.NumberOfSections; i++, section++)
	{
		if (section->Misc.PhysicalAddress != 0)
		{
			void* sectionAddress = (void*)(uintptr_t)section->Misc.PhysicalAddress;

			// Проверяем что адрес секции внутри нашего codeBase
			if (sectionAddress >= module->codeBase &&
				sectionAddress < (byte*)module->codeBase + headers->OptionalHeader.SizeOfImage)
			{
				// Освобождаем только если мы его выделяли
				SIZE_T sectionSize = max(section->SizeOfRawData, section->Misc.VirtualSize);

				// VirtualFree требует выравнивания по границе страницы
				void* pageBase = (void*)((uintptr_t)sectionAddress & ~(4096 - 1));
				SIZE_T pageSize = sectionSize + ((uintptr_t)sectionAddress - (uintptr_t)pageBase);

				VirtualFree(pageBase, pageSize, MEM_DECOMMIT);
			}

			section->Misc.PhysicalAddress = 0;
		}
	}
}

// Безопасная версия CALCULATE_ADDRESS
static void* SAFE_CALCULATE_ADDRESS(const void* base, DWORD offset, DWORD maxSize)
{
	if (!base || offset == 0 || offset > maxSize)
		return NULL;

	// Проверка на переполнение
	if ((DWORD_PTR)base + offset < (DWORD_PTR)base)
		return NULL;

	return (byte*)base + offset;
}

static qboolean CopySections(const byte* data, PIMAGE_NT_HEADERS old_headers, PMEMORYMODULE module)
{
	int i, size;
	byte* dest;
	byte* codeBase;
	PIMAGE_SECTION_HEADER section;

	// Проверка входных параметров
	if (!data || !old_headers || !module || !module->headers || !module->codeBase)
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return false;
	}

	codeBase = module->codeBase;
	section = IMAGE_FIRST_SECTION(module->headers);

	// Проверка количества секций
	if (module->headers->FileHeader.NumberOfSections < 0 ||
		module->headers->FileHeader.NumberOfSections > 1000) // Разумный предел
	{
		SetLastError(ERROR_INVALID_DATA);
		return false;
	}

	for (i = 0; i < module->headers->FileHeader.NumberOfSections; i++, section++)
	{
		// Проверка валидности указателя секции
		if (!IS_VALID_POINTER(section))
		{
			SetLastError(ERROR_INVALID_DATA);
			return false;
		}

		if (section->SizeOfRawData == 0)
		{
			// section doesn't contain data in the dll itself, but may define
			// uninitialized data
			size = old_headers->OptionalHeader.SectionAlignment;

			if (size > 0)
			{
				// Проверка выравнивания
				if (size > MAX_SECTION_SIZE) // Защита от слишком больших секций
				{
					SetLastError(ERROR_NOT_ENOUGH_MEMORY);
					return false;
				}

				// Проверка виртуального адреса
				if (section->VirtualAddress > module->headers->OptionalHeader.SizeOfImage)
				{
					SetLastError(ERROR_INVALID_DATA);
					return false;
				}

				dest = (byte*)VirtualAlloc(
					(byte*)CALCULATE_ADDRESS(codeBase, section->VirtualAddress),
					size,
					MEM_COMMIT,
					PAGE_READWRITE
				);

				if (!dest)
				{
					// VirtualAlloc уже установил LastError
					return false;
				}

				section->Misc.PhysicalAddress = (DWORD)(uintptr_t)dest; // Безопасное приведение
				Q_memset(dest, 0, size);
			}
			// section is empty
			continue;
		}

		// Проверка размера сырых данных
		if (section->SizeOfRawData > MAX_SECTION_SIZE ||
			section->SizeOfRawData < 0)
		{
			SetLastError(ERROR_INVALID_DATA);
			return false;
		}

		// Проверка указателя на сырые данные
		if (section->PointerToRawData >= old_headers->OptionalHeader.SizeOfHeaders ||
			section->PointerToRawData + section->SizeOfRawData > old_headers->OptionalHeader.SizeOfImage)
		{
			SetLastError(ERROR_INVALID_DATA);
			return false;
		}

		// Проверка виртуального адреса
		if (section->VirtualAddress >= module->headers->OptionalHeader.SizeOfImage ||
			section->VirtualAddress + section->SizeOfRawData > module->headers->OptionalHeader.SizeOfImage)
		{
			SetLastError(ERROR_INVALID_DATA);
			return false;
		}

		// commit memory block and copy data from dll
		dest = (byte*)VirtualAlloc(
			(byte*)CALCULATE_ADDRESS(codeBase, section->VirtualAddress),
			section->SizeOfRawData,
			MEM_COMMIT,
			PAGE_READWRITE
		);

		if (!dest)
		{
			// VirtualAlloc уже установил LastError
			return false;
		}

		// Проверка что данные не выходят за границы
		const byte* src = (byte*)CALCULATE_ADDRESS(data, section->PointerToRawData);
		if (!IS_VALID_POINTER_RANGE(src, section->SizeOfRawData))
		{
			VirtualFree(dest, 0, MEM_RELEASE);
			SetLastError(ERROR_INVALID_DATA);
			return false;
		}

		// Копирование данных с проверкой
		Q_memcpy(dest, src, section->SizeOfRawData);

		// Проверка целостности после копирования (опционально)
		if (Q_memcmp(dest, src, min(section->SizeOfRawData, 16)) != 0)
		{
			VirtualFree(dest, 0, MEM_RELEASE);
			SetLastError(ERROR_CRC);
			return false;
		}

		section->Misc.PhysicalAddress = (DWORD)(uintptr_t)dest; // Безопасное приведение
	}

	return true;
}

static void FinalizeSections( MEMORYMODULE *module )
{
	PIMAGE_SECTION_HEADER section = IMAGE_FIRST_SECTION( module->headers );
	int	i;
	
	// loop through all sections and change access flags
	for( i = 0; i < module->headers->FileHeader.NumberOfSections; i++, section++ )
	{
		DWORD	protect, oldProtect, size;
		int	executable = (section->Characteristics & IMAGE_SCN_MEM_EXECUTE) != 0;
		int	readable = (section->Characteristics & IMAGE_SCN_MEM_READ) != 0;
		int	writeable = (section->Characteristics & IMAGE_SCN_MEM_WRITE) != 0;

		if( section->Characteristics & IMAGE_SCN_MEM_DISCARDABLE )
		{
			// section is not needed any more and can safely be freed
			VirtualFree((LPVOID)section->Misc.PhysicalAddress, section->SizeOfRawData, MEM_DECOMMIT);
			continue;
		}

		// determine protection flags based on characteristics
		protect = ProtectionFlags[executable][readable][writeable];
		if( section->Characteristics & IMAGE_SCN_MEM_NOT_CACHED )
			protect |= PAGE_NOCACHE;

		// determine size of region
		size = section->SizeOfRawData;

		if( size == 0 )
		{
			if( section->Characteristics & IMAGE_SCN_CNT_INITIALIZED_DATA )
				size = module->headers->OptionalHeader.SizeOfInitializedData;
			else if( section->Characteristics & IMAGE_SCN_CNT_UNINITIALIZED_DATA )
				size = module->headers->OptionalHeader.SizeOfUninitializedData;
		}

		if( size > 0 )
		{         
			// change memory access flags
			if( !VirtualProtect((LPVOID)section->Misc.PhysicalAddress, size, protect, &oldProtect ))
				Sys_Error( "Com_FinalizeSections: error protecting memory page\n" );
		}
	}
}

static void PerformBaseRelocation( MEMORYMODULE *module, DWORD delta )
{
	DWORD	i;
	byte	*codeBase = module->codeBase;
	PIMAGE_DATA_DIRECTORY directory = GET_HEADER_DICTIONARY( module, IMAGE_DIRECTORY_ENTRY_BASERELOC );

	if( directory->Size > 0 )
	{
		PIMAGE_BASE_RELOCATION relocation = (PIMAGE_BASE_RELOCATION)CALCULATE_ADDRESS( codeBase, directory->VirtualAddress );
		for( ; relocation->VirtualAddress > 0; )
		{
			byte	*dest = (byte *)CALCULATE_ADDRESS( codeBase, relocation->VirtualAddress );
			word	*relInfo = (word *)((byte *)relocation + IMAGE_SIZEOF_BASE_RELOCATION );

			for( i = 0; i<((relocation->SizeOfBlock-IMAGE_SIZEOF_BASE_RELOCATION) / 2); i++, relInfo++ )
			{
				DWORD	*patchAddrHL;
				int	type, offset;

				// the upper 4 bits define the type of relocation
				type = *relInfo >> 12;
				// the lower 12 bits define the offset
				offset = *relInfo & 0xfff;
				
				switch( type )
				{
				case IMAGE_REL_BASED_ABSOLUTE:
					// skip relocation
					break;
				case IMAGE_REL_BASED_HIGHLOW:
					// change complete 32 bit address
					patchAddrHL = (DWORD *)CALCULATE_ADDRESS( dest, offset );
					*patchAddrHL += delta;
					break;
				default:
					MsgDev( D_ERROR, "PerformBaseRelocation: unknown relocation: %d\n", type );
					break;
				}
			}

			// advance to next relocation block
			relocation = (PIMAGE_BASE_RELOCATION)CALCULATE_ADDRESS( relocation, relocation->SizeOfBlock );
		}
	}
}

static FARPROC MemoryGetProcAddress(void* module, const char* name)
{
	int idx = -1;
	DWORD i;
	DWORD* nameRef;
	WORD* ordinal;
	PIMAGE_EXPORT_DIRECTORY exports;
	PMEMORYMODULE memModule;
	byte* codeBase;
	PIMAGE_DATA_DIRECTORY directory;

	// Проверка входных параметров
	if (!module || !name || !name[0])
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return NULL;
	}

	memModule = (PMEMORYMODULE)module;
	if (!memModule->codeBase || !memModule->headers)
	{
		SetLastError(ERROR_INVALID_HANDLE);
		return NULL;
	}

	codeBase = memModule->codeBase;
	directory = GET_HEADER_DICTIONARY(memModule, IMAGE_DIRECTORY_ENTRY_EXPORT);

	// Проверка директории экспорта
	if (!directory || directory->Size == 0)
	{
		// no export table found
		SetLastError(ERROR_PROC_NOT_FOUND);
		return NULL;
	}

	// Проверка виртуального адреса директории экспорта
	if (directory->VirtualAddress >= memModule->headers->OptionalHeader.SizeOfImage ||
		directory->VirtualAddress + directory->Size > memModule->headers->OptionalHeader.SizeOfImage)
	{
		SetLastError(ERROR_INVALID_DATA);
		return NULL;
	}

	exports = (PIMAGE_EXPORT_DIRECTORY)SAFE_CALCULATE_ADDRESS(codeBase, directory->VirtualAddress, directory->Size);
	if (!exports)
	{
		SetLastError(ERROR_INVALID_DATA);
		return NULL;
	}

	// Проверка структуры экспорта
	if (exports->NumberOfNames == 0 || exports->NumberOfFunctions == 0)
	{
		// DLL doesn't export anything
		SetLastError(ERROR_PROC_NOT_FOUND);
		return NULL;
	}

	// Проверка целостности данных экспорта
	if (exports->NumberOfNames > 0x10000 || exports->NumberOfFunctions > 0x10000) // Разумный предел
	{
		SetLastError(ERROR_INVALID_DATA);
		return NULL;
	}

	// Проверка указателей на таблицы имен и ординалов
	if (exports->AddressOfNames >= memModule->headers->OptionalHeader.SizeOfImage ||
		exports->AddressOfNameOrdinals >= memModule->headers->OptionalHeader.SizeOfImage ||
		exports->AddressOfFunctions >= memModule->headers->OptionalHeader.SizeOfImage)
	{
		SetLastError(ERROR_INVALID_DATA);
		return NULL;
	}

	// Вычисление адресов таблиц с проверкой
	nameRef = (DWORD*)SAFE_CALCULATE_ADDRESS(codeBase, exports->AddressOfNames,
		exports->NumberOfNames * sizeof(DWORD));
	ordinal = (WORD*)SAFE_CALCULATE_ADDRESS(codeBase, exports->AddressOfNameOrdinals,
		exports->NumberOfNames * sizeof(WORD));

	if (!nameRef || !ordinal)
	{
		SetLastError(ERROR_INVALID_DATA);
		return NULL;
	}

	// Поиск имени функции в списке экспортируемых имен
	for (i = 0; i < exports->NumberOfNames; i++)
	{
		const char* exportedName;

		// Проверка текущего указателя имени
		if (nameRef[i] >= memModule->headers->OptionalHeader.SizeOfImage)
		{
			continue; // Пропускаем поврежденные записи
		}

		exportedName = (const char*)SAFE_CALCULATE_ADDRESS(codeBase, nameRef[i], MAX_EXPORT_NAME_LENGTH);
		if (!exportedName)
		{
			continue; // Пропускаем поврежденные записи
		}

		// Проверка что строка нуль-терминирована в разумных пределах
		if (!IsValidString(exportedName, MAX_EXPORT_NAME_LENGTH))
		{
			continue;
		}

		// GetProcAddress case insensitive
		if (!Q_stricmp(name, exportedName))
		{
			// Проверка ординала в допустимых пределах
			if (i >= exports->NumberOfNames || ordinal[i] >= exports->NumberOfFunctions)
			{
				SetLastError(ERROR_INVALID_DATA);
				return NULL;
			}

			idx = ordinal[i];
			break;
		}
	}

	if (idx == -1)
	{
		// exported symbol not found
		SetLastError(ERROR_PROC_NOT_FOUND);
		return NULL;
	}

	// Проверка индекса функции
	if ((DWORD)idx >= exports->NumberOfFunctions)
	{
		// name <-> ordinal number don't match
		SetLastError(ERROR_INVALID_DATA);
		return NULL;
	}

	// Проверка таблицы адресов функций
	DWORD* addressOfFunctions = (DWORD*)SAFE_CALCULATE_ADDRESS(codeBase, exports->AddressOfFunctions,
		exports->NumberOfFunctions * sizeof(DWORD));
	if (!addressOfFunctions)
	{
		SetLastError(ERROR_INVALID_DATA);
		return NULL;
	}

	// Проверка конкретного адреса функции
	DWORD functionRVA = addressOfFunctions[idx];
	if (functionRVA == 0)
	{
		// Функция форвардена или не реализована
		SetLastError(ERROR_PROC_NOT_FOUND);
		return NULL;
	}

	// Проверка что адрес функции внутри образа
	if (functionRVA >= memModule->headers->OptionalHeader.SizeOfImage)
	{
		SetLastError(ERROR_INVALID_DATA);
		return NULL;
	}

	// addressOfFunctions contains the RVAs to the "real" functions
	FARPROC result = (FARPROC)SAFE_CALCULATE_ADDRESS(codeBase, functionRVA,
		memModule->headers->OptionalHeader.SizeOfImage - functionRVA);
	if (!result)
	{
		SetLastError(ERROR_INVALID_DATA);
		return NULL;
	}

	return result;
}

// Функция проверки валидности строки
static BOOL IsValidString(const char* str, size_t maxLen)
{
	if (!str) return FALSE;

	for (size_t i = 0; i < maxLen; i++)
	{
		if (str[i] == '\0')
			return TRUE; // Корректная нуль-терминированная строка
		if (str[i] < 32 || str[i] > 126) // Проверка на печатные ASCII символы
			return FALSE;
	}

	return FALSE; // Слишком длинная строка или не нуль-терминирована
}

// Функция проверки подозрительных имен DLL
static BOOL IsSuspiciousDllName(LPCSTR dllName)
{
	if (!dllName) return TRUE;

	// Проверка на абсолютные пути
	if (strchr(dllName, '\\') || strchr(dllName, '/') || strchr(dllName, ':'))
		return TRUE;

	// Проверка на попытку directory traversal
	if (strstr(dllName, ".."))
		return TRUE;

	// Проверка расширения
	const char* ext = strrchr(dllName, '.');
	if (ext && _stricmp(ext, ".dll") != 0)
		return TRUE;

	return FALSE;
}

static int BuildImportTable(MEMORYMODULE* module)
{
	int result = 1;
	byte* codeBase;
	PIMAGE_DATA_DIRECTORY directory;
	PIMAGE_IMPORT_DESCRIPTOR importDesc;

	// Проверка входных параметров
	if (!module || !module->codeBase || !module->headers)
	{
		MsgDev(D_ERROR, "BuildImportTable: invalid module parameter\n");
		return 0;
	}

	codeBase = module->codeBase;
	directory = GET_HEADER_DICTIONARY(module, IMAGE_DIRECTORY_ENTRY_IMPORT);

	if (!directory || directory->Size == 0)
	{
		// No import table - это нормально для некоторых модулей
		return 1;
	}

	// Проверка границ импортной директории
	if (directory->VirtualAddress >= module->headers->OptionalHeader.SizeOfImage ||
		directory->VirtualAddress + directory->Size > module->headers->OptionalHeader.SizeOfImage)
	{
		MsgDev(D_ERROR, "BuildImportTable: import directory outside image bounds\n");
		return 0;
	}

	importDesc = (PIMAGE_IMPORT_DESCRIPTOR)SAFE_CALCULATE_ADDRESS(
		codeBase,
		directory->VirtualAddress,
		directory->Size
	);

	if (!importDesc)
	{
		MsgDev(D_ERROR, "BuildImportTable: invalid import descriptor address\n");
		return 0;
	}

	// Ограничение количества импортируемых DLL для защиты от бесконечного цикла
	const int MAX_IMPORT_DLLS = 256;
	int dllCount = 0;

	for (; dllCount < MAX_IMPORT_DLLS; importDesc++, dllCount++)
	{
		// Проверка валидности дескриптора
		if (!IS_VALID_POINTER_RANGE(importDesc, sizeof(IMAGE_IMPORT_DESCRIPTOR)) ||
			IsBadReadPtr(importDesc, sizeof(IMAGE_IMPORT_DESCRIPTOR)))
		{
			MsgDev(D_WARN, "BuildImportTable: invalid import descriptor at index %d\n", dllCount);
			break;
		}

		// Конец таблицы импорта
		if (importDesc->Name == 0)
			break;

		// Проверка имени DLL
		if (importDesc->Name >= module->headers->OptionalHeader.SizeOfImage)
		{
			MsgDev(D_ERROR, "BuildImportTable: invalid DLL name RVA\n");
			result = 0;
			break;
		}

		LPCSTR libname = (LPCSTR)SAFE_CALCULATE_ADDRESS(
			codeBase,
			importDesc->Name,
			MAX_DLL_NAME_LENGTH
		);

		if (!libname || !IsValidString(libname, MAX_DLL_NAME_LENGTH))
		{
			MsgDev(D_ERROR, "BuildImportTable: invalid DLL name\n");
			result = 0;
			break;
		}

		// Проверка безопасности имени DLL
		if (IsSuspiciousDllName(libname))
		{
			MsgDev(D_ERROR, "BuildImportTable: suspicious DLL name '%s'\n", libname);
			result = 0;
			break;
		}

		// Загрузка библиотеки
		void* handle = Com_LoadLibraryExt(libname, false, true);
		if (handle == NULL)
		{
			MsgDev(D_ERROR, "BuildImportTable: couldn't load library %s\n", libname);
			result = 0;
			break;
		}

		// Добавление в список загруженных модулей
		void** newModules = (void**)Mem_Realloc(
			host.mempool,
			module->modules,
			(module->numModules + 1) * sizeof(void*)
		);

		if (!newModules)
		{
			Com_FreeLibrary(handle);
			MsgDev(D_ERROR, "BuildImportTable: out of memory for module list\n");
			result = 0;
			break;
		}

		module->modules = newModules;
		module->modules[module->numModules++] = handle;

		// Обработка таблицы импорта функций
		DWORD* thunkRef, * funcRef;

		if (importDesc->OriginalFirstThunk != 0)
		{
			thunkRef = (DWORD*)SAFE_CALCULATE_ADDRESS(
				codeBase,
				importDesc->OriginalFirstThunk,
				MAX_IMPORT_TABLE_SIZE
			);
			funcRef = (DWORD*)SAFE_CALCULATE_ADDRESS(
				codeBase,
				importDesc->FirstThunk,
				MAX_IMPORT_TABLE_SIZE
			);
		}
		else
		{
			// no hint table
			thunkRef = (DWORD*)SAFE_CALCULATE_ADDRESS(
				codeBase,
				importDesc->FirstThunk,
				MAX_IMPORT_TABLE_SIZE
			);
			funcRef = thunkRef;
		}

		if (!thunkRef || !funcRef)
		{
			MsgDev(D_ERROR, "BuildImportTable: invalid import thunk table\n");
			result = 0;
			break;
		}

		// Ограничение количества импортируемых функций
		const int MAX_IMPORT_FUNCTIONS = 4096;
		int funcCount = 0;

		for (; *thunkRef != 0 && funcCount < MAX_IMPORT_FUNCTIONS; thunkRef++, funcRef++, funcCount++)
		{
			// Проверка указателей
			if (!IS_VALID_POINTER(thunkRef) || !IS_VALID_POINTER(funcRef))
			{
				MsgDev(D_ERROR, "BuildImportTable: invalid thunk or func pointer\n");
				result = 0;
				break;
			}

			FARPROC procAddress = NULL;

			if (IMAGE_SNAP_BY_ORDINAL(*thunkRef))
			{
				// Импорт по ординалу
				WORD ordinal = IMAGE_ORDINAL(*thunkRef);
				if (ordinal == 0 || ordinal > 0xFFFF)
				{
					MsgDev(D_ERROR, "BuildImportTable: invalid ordinal %u\n", ordinal);
					result = 0;
					break;
				}

				procAddress = Com_GetProcAddress(handle, (LPCSTR)(uintptr_t)ordinal);
			}
			else
			{
				// Импорт по имени
				if (*thunkRef >= module->headers->OptionalHeader.SizeOfImage)
				{
					MsgDev(D_ERROR, "BuildImportTable: invalid import by name RVA\n");
					result = 0;
					break;
				}

				PIMAGE_IMPORT_BY_NAME thunkData = (PIMAGE_IMPORT_BY_NAME)SAFE_CALCULATE_ADDRESS(
					codeBase,
					*thunkRef,
					sizeof(IMAGE_IMPORT_BY_NAME) + MAX_FUNCTION_NAME_LENGTH
				);

				if (!thunkData || !IsValidString((LPCSTR)&thunkData->Name, MAX_FUNCTION_NAME_LENGTH))
				{
					MsgDev(D_ERROR, "BuildImportTable: invalid function name\n");
					result = 0;
					break;
				}

				LPCSTR funcName = (LPCSTR)&thunkData->Name;
				procAddress = Com_GetProcAddress(handle, funcName);
			}

			if (procAddress == NULL)
			{
				MsgDev(D_ERROR, "BuildImportTable: failed to get function address\n");
				result = 0;
				break;
			}

			// Запись адреса функции
			*funcRef = (DWORD)(uintptr_t)procAddress;
		}

		if (funcCount >= MAX_IMPORT_FUNCTIONS)
		{
			MsgDev(D_WARN, "BuildImportTable: too many functions in import table for %s\n", libname);
		}

		if (!result)
			break;
	}

	if (dllCount >= MAX_IMPORT_DLLS)
	{
		MsgDev(D_WARN, "BuildImportTable: import table too large, truncated\n");
	}

	return result;
}

// Безопасная версия для работы с ординалами
_inline WORD SafeImageOrdinal(DWORD thunk)
{
	if (IMAGE_SNAP_BY_ORDINAL(thunk))
	{
		WORD ordinal = IMAGE_ORDINAL(thunk);
		// Ординалы обычно начинаются с 1, 0 обычно невалиден
		if (ordinal > 0 && ordinal <= 0xFFFF)
			return ordinal;
	}
	return 0;
}

static void MemoryFreeLibrary(void* hInstance)
{
	MEMORYMODULE* module = (MEMORYMODULE*)hInstance;

	// Проверка валидности указателя
	if (module == NULL)
		return;

	// Проверка magic number или сигнатуры для избежания освобождения случайной памяти
	if (module->magic != MEMORYMODULE_MAGIC)
	{
		MsgDev(D_ERROR, "MemoryFreeLibrary: invalid module magic number\n");
		return;
	}

	// Сбрасываем magic number чтобы предотвратить повторное использование
	module->magic = 0;

	// Уведомление библиотеки об отключении от процесса
	if (module->initialized != 0 && module->codeBase != NULL && module->headers != NULL)
	{
		// Проверка точки входа
		if (module->headers->OptionalHeader.AddressOfEntryPoint != 0 &&
			module->headers->OptionalHeader.AddressOfEntryPoint < module->headers->OptionalHeader.SizeOfImage)
		{
			DllEntryProc DllEntry = (DllEntryProc)SAFE_CALCULATE_ADDRESS(
				module->codeBase,
				module->headers->OptionalHeader.AddressOfEntryPoint,
				module->headers->OptionalHeader.SizeOfImage - module->headers->OptionalHeader.AddressOfEntryPoint
			);

			if (DllEntry != NULL && IS_VALID_CODE_POINTER(DllEntry))
			{
				__try
				{
					// Вызов точки входа с DLL_PROCESS_DETACH
					BOOL result = (*DllEntry)((HINSTANCE)module->codeBase, DLL_PROCESS_DETACH, 0);
					if (!result)
					{
						MsgDev(D_WARN, "MemoryFreeLibrary: DllEntry(PROCESS_DETACH) returned FALSE\n");
					}
				}
				__except (EXCEPTION_EXECUTE_HANDLER)
				{
					MsgDev(D_ERROR, "MemoryFreeLibrary: exception in DllEntry(PROCESS_DETACH)\n");
				}
			}
			else
			{
				MsgDev(D_WARN, "MemoryFreeLibrary: invalid DllEntry point\n");
			}
		}
		module->initialized = 0;
	}

	// Освобождение ранее открытых библиотек
	if (module->modules != NULL)
	{
		for (int i = 0; i < module->numModules; i++)
		{
			if (module->modules[i] != NULL)
			{
				// Проверка что это валидный handle библиотеки
				if (IsValidLibraryHandle(module->modules[i]))
				{
					Com_FreeLibrary(module->modules[i]);
				}
				else
				{
					MsgDev(D_WARN, "MemoryFreeLibrary: invalid library handle at index %d\n", i);
				}
				module->modules[i] = NULL;
			}
		}

		// Освобождение массива модулей
		Mem_Free(module->modules);
		module->modules = NULL;
		module->numModules = 0;
	}

	// Освобождение секций
	if (module->headers != NULL)
	{
		FreeSections(module->headers, module);
	}

	// Освобождение памяти библиотеки
	if (module->codeBase != NULL)
	{
		// Дополнительная проверка что это действительно выделенная нами память
		if (IsValidCodeBase(module->codeBase))
		{
			SIZE_T regionSize = 0;

			// Получаем реальный размер региона
			MEMORY_BASIC_INFORMATION mbi;
			if (VirtualQuery(module->codeBase, &mbi, sizeof(mbi)) != 0)
			{
				regionSize = mbi.RegionSize;
			}

			// Освобождаем память
			if (!VirtualFree(module->codeBase, 0, MEM_RELEASE))
			{
				DWORD error = GetLastError();
				MsgDev(D_ERROR, "MemoryFreeLibrary: VirtualFree failed with error %lu\n", error);
			}
		}
		else
		{
			MsgDev(D_ERROR, "MemoryFreeLibrary: invalid codeBase pointer\n");
		}
		module->codeBase = NULL;
	}

	// Очистка оставшихся полей для безопасности
	if (module->headers != NULL)
	{
		module->headers = NULL;
	}

	// Освобождение структуры модуля
	if (!HeapFree(GetProcessHeap(), 0, module))
	{
		DWORD error = GetLastError();
		MsgDev(D_ERROR, "MemoryFreeLibrary: HeapFree failed with error %lu\n", error);
	}
}

void *MemoryLoadLibrary( const char *name )
{
	MEMORYMODULE	*result = NULL;
	PIMAGE_DOS_HEADER	dos_header;
	PIMAGE_NT_HEADERS	old_header;
	byte		*code, *headers;
	DWORD		locationDelta;
	DllEntryProc	DllEntry;
	string		errorstring;
	qboolean		successfull;
	void		*data = NULL;

	data = FS_LoadFile( name, NULL, false );

	if( !data )
	{
		Q_sprintf( errorstring, "couldn't load %s", name );
		goto library_error;
	}

	dos_header = (PIMAGE_DOS_HEADER)data;
	if( dos_header->e_magic != IMAGE_DOS_SIGNATURE )
	{
		Q_sprintf( errorstring, "%s it's not a valid executable file", name );
		goto library_error;
	}

	old_header = (PIMAGE_NT_HEADERS)&((const byte *)(data))[dos_header->e_lfanew];
	if( old_header->Signature != IMAGE_NT_SIGNATURE )
	{
		Q_sprintf( errorstring, "%s missing PE header", name );
		goto library_error;
	}

	// reserve memory for image of library
	code = (byte *)VirtualAlloc((LPVOID)(old_header->OptionalHeader.ImageBase), old_header->OptionalHeader.SizeOfImage, MEM_RESERVE, PAGE_READWRITE );

	if( code == NULL )
	{
		// try to allocate memory at arbitrary position
		code = (byte *)VirtualAlloc( NULL, old_header->OptionalHeader.SizeOfImage, MEM_RESERVE, PAGE_READWRITE );
	}    
	if( code == NULL )
	{
		Q_sprintf( errorstring, "%s can't reserve memory", name );
		goto library_error;
	}

	result = (MEMORYMODULE *)HeapAlloc( GetProcessHeap(), 0, sizeof( MEMORYMODULE ));
	result->codeBase = code;
	result->numModules = 0;
	result->modules = NULL;
	result->initialized = 0;

	// XXX: is it correct to commit the complete memory region at once?
	// calling DllEntry raises an exception if we don't...
	VirtualAlloc( code, old_header->OptionalHeader.SizeOfImage, MEM_COMMIT, PAGE_READWRITE );

	// commit memory for headers
	headers = (byte *)VirtualAlloc( code, old_header->OptionalHeader.SizeOfHeaders, MEM_COMMIT, PAGE_READWRITE );
	
	// copy PE header to code
	Q_memcpy( headers, dos_header, dos_header->e_lfanew + old_header->OptionalHeader.SizeOfHeaders );
	result->headers = (PIMAGE_NT_HEADERS)&((const byte *)(headers))[dos_header->e_lfanew];

	// update position
	result->headers->OptionalHeader.ImageBase = (DWORD)code;

	// copy sections from DLL file block to new memory location
	CopySections( data, old_header, result );

	// adjust base address of imported data
	locationDelta = (DWORD)(code - old_header->OptionalHeader.ImageBase);
	if( locationDelta != 0 ) PerformBaseRelocation( result, locationDelta );

	// load required dlls and adjust function table of imports
	if( !BuildImportTable( result ))
	{
		Q_sprintf( errorstring, "%s failed to build import table", name );
		goto library_error;
	}

	// mark memory pages depending on section headers and release
	// sections that are marked as "discardable"
	FinalizeSections( result );

	// get entry point of loaded library
	if( result->headers->OptionalHeader.AddressOfEntryPoint != 0 )
	{
		DllEntry = (DllEntryProc)CALCULATE_ADDRESS( code, result->headers->OptionalHeader.AddressOfEntryPoint );
		if( DllEntry == 0 )
		{
			Q_sprintf( errorstring, "%s has no entry point", name );
			goto library_error;
		}

		// notify library about attaching to process
		successfull = (*DllEntry)((HINSTANCE)code, DLL_PROCESS_ATTACH, 0 );
		if( !successfull )
		{
			Q_sprintf( errorstring, "can't attach library %s", name );
			goto library_error;
		}
		result->initialized = 1;
	}

	Mem_Free( data ); // release memory
	return (void *)result;
library_error:
	// cleanup
	if( data ) Mem_Free( data );
	MemoryFreeLibrary( result );
	Com_PushLibraryError( errorstring );
	MsgDev( D_ERROR, "LoadLibrary: %s\n", errorstring );

	return NULL;
}

/*
---------------------------------------------------------------

		Name for function stuff

---------------------------------------------------------------
*/
static void FsGetString( file_t *f, char *str )
{
	char	ch;

	while(( ch = FS_Getc( f )) != EOF )
	{
		*str++ = ch;
		if( !ch ) break;
	}
}

static void FreeNameFuncGlobals( dll_user_t *hInst )
{
	int	i;

	if( !hInst ) return;

	if( hInst->ordinals ) Mem_Free( hInst->ordinals );
	if( hInst->funcs ) Mem_Free( hInst->funcs );

	for( i = 0; i < hInst->num_ordinals; i++ )
	{
		if( hInst->names[i] )
			Mem_Free( hInst->names[i] );
	}

	hInst->num_ordinals = 0;
	hInst->ordinals = NULL;
	hInst->funcs = NULL;
}

char *GetMSVCName( const char *in_name )
{
	char	*pos, *out_name;

	if( in_name[0] == '?' )  // is this a MSVC C++ mangled name?
	{
		if(( pos = Q_strstr( in_name, "@@" )) != NULL )
		{
			int	len = pos - in_name;

			// strip off the leading '?'
			out_name = copystring( in_name + 1 );
			out_name[len-1] = 0; // terminate string at the "@@"
			return out_name;
		}
	}
	return copystring( in_name );
}

qboolean LibraryLoadSymbols( dll_user_t *hInst )
{
	file_t		*f;
	string		errorstring;
	DOS_HEADER	dos_header;
	LONG		nt_signature;
	PE_HEADER		pe_header;
	SECTION_HEADER	section_header;
	qboolean		rdata_found;
	OPTIONAL_HEADER	optional_header;
	long		rdata_delta = 0;
	EXPORT_DIRECTORY	export_directory;
	long		name_offset;
	long		exports_offset;
	long		ordinal_offset;
	long		function_offset;
	string		function_name;
	dword		*p_Names = NULL;
	int		i, index;

	// can only be done for loaded libraries
	if( !hInst ) return false;

	for( i = 0; i < hInst->num_ordinals; i++ )
		hInst->names[i] = NULL;

	f = FS_Open( hInst->shortPath, "rb", false );
	if( !f )
	{
		Q_sprintf( errorstring, "couldn't load %s", hInst->shortPath );
		goto table_error;
	}

	if( FS_Read( f, &dos_header, sizeof( dos_header )) != sizeof( dos_header ))
	{
		Q_sprintf( errorstring, "%s has corrupted EXE header", hInst->shortPath );
		goto table_error;
	}

	if( dos_header.e_magic != DOS_SIGNATURE )
	{
		Q_sprintf( errorstring, "%s does not have a valid dll signature", hInst->shortPath );
		goto table_error;
	}

	if( FS_Seek( f, dos_header.e_lfanew, SEEK_SET ) == -1 )
	{
		Q_sprintf( errorstring, "%s error seeking for new exe header", hInst->shortPath );
		goto table_error;
	}

	if( FS_Read( f, &nt_signature, sizeof( nt_signature )) != sizeof( nt_signature ))
	{
		Q_sprintf( errorstring, "%s has corrupted NT header", hInst->shortPath );
		goto table_error;
	}

	if( nt_signature != NT_SIGNATURE )
	{
		Q_sprintf( errorstring, "%s does not have a valid NT signature", hInst->shortPath );
		goto table_error;
	}

	if( FS_Read( f, &pe_header, sizeof( pe_header )) != sizeof( pe_header ))
	{
		Q_sprintf( errorstring, "%s does not have a valid PE header", hInst->shortPath );
		goto table_error;
	}

	if( !pe_header.SizeOfOptionalHeader )
	{
		Q_sprintf( errorstring, "%s does not have an optional header", hInst->shortPath );
		goto table_error;
	}

	if( FS_Read( f, &optional_header, sizeof( optional_header )) != sizeof( optional_header ))
	{
		Q_sprintf( errorstring, "%s optional header probably corrupted", hInst->shortPath );
		goto table_error;
	}

	rdata_found = false;

	for( i = 0; i < pe_header.NumberOfSections; i++ )
	{
		if( FS_Read( f, &section_header, sizeof( section_header )) != sizeof( section_header ))
		{
			Q_sprintf( errorstring, "%s error during reading section header", hInst->shortPath );
			goto table_error;
		}

		if((( optional_header.DataDirectory[0].VirtualAddress >= section_header.VirtualAddress ) && 
			(optional_header.DataDirectory[0].VirtualAddress < (section_header.VirtualAddress + section_header.Misc.VirtualSize))))
		{
			rdata_found = true;
			break;
		}
	}

	if( rdata_found )
	{
		rdata_delta = section_header.VirtualAddress - section_header.PointerToRawData; 
	}

	exports_offset = optional_header.DataDirectory[0].VirtualAddress - rdata_delta;

	if( FS_Seek( f, exports_offset, SEEK_SET ) == -1 )
	{
		Q_sprintf( errorstring, "%s does not have a valid exports section", hInst->shortPath );
		goto table_error;
	}

	if( FS_Read( f, &export_directory, sizeof( export_directory )) != sizeof( export_directory ))
	{
		Q_sprintf( errorstring, "%s does not have a valid optional header", hInst->shortPath );
		goto table_error;
	}

	hInst->num_ordinals = export_directory.NumberOfNames;	// also number of ordinals

	if( hInst->num_ordinals > MAX_LIBRARY_EXPORTS )
	{
		Q_sprintf( errorstring, "%s too many exports %i", hInst->shortPath, hInst->num_ordinals );
		hInst->num_ordinals = 0;
		goto table_error;
	}

	ordinal_offset = export_directory.AddressOfNameOrdinals - rdata_delta;

	if( FS_Seek( f, ordinal_offset, SEEK_SET ) == -1 )
	{
		Q_sprintf( errorstring, "%s does not have a valid ordinals section", hInst->shortPath );
		goto table_error;
	}

	hInst->ordinals = Mem_Alloc( host.mempool, hInst->num_ordinals * sizeof( word ));

	if( FS_Read( f, hInst->ordinals, hInst->num_ordinals * sizeof( word )) != (hInst->num_ordinals * sizeof( word )))
	{
		Q_sprintf( errorstring, "%s error during reading ordinals table", hInst->shortPath );
		goto table_error;
	}

	function_offset = export_directory.AddressOfFunctions - rdata_delta;

	if( FS_Seek( f, function_offset, SEEK_SET ) == -1 )
	{
		Q_sprintf( errorstring, "%s does not have a valid export address section", hInst->shortPath );
		goto table_error;
	}

	hInst->funcs = Mem_Alloc( host.mempool, hInst->num_ordinals * sizeof( dword ));

	if( FS_Read( f, hInst->funcs, hInst->num_ordinals * sizeof( dword )) != (hInst->num_ordinals * sizeof( dword )))
	{
		Q_sprintf( errorstring, "%s error during reading export address section", hInst->shortPath );
		goto table_error;
	}

	name_offset = export_directory.AddressOfNames - rdata_delta;

	if( FS_Seek( f, name_offset, SEEK_SET ) == -1 )
	{
		Q_sprintf( errorstring, "%s file does not have a valid names section", hInst->shortPath );
		goto table_error;
	}

	p_Names = Mem_Alloc( host.mempool, hInst->num_ordinals * sizeof( dword ));

	if( FS_Read( f, p_Names, hInst->num_ordinals * sizeof( dword )) != (hInst->num_ordinals * sizeof( dword )))
	{
		Q_sprintf( errorstring, "%s error during reading names table", hInst->shortPath );
		goto table_error;
	}

	for( i = 0; i < hInst->num_ordinals; i++ )
	{
		name_offset = p_Names[i] - rdata_delta;

		if( name_offset != 0 )
		{
			if( FS_Seek( f, name_offset, SEEK_SET ) != -1 )
			{
				FsGetString( f, function_name );
				hInst->names[i] = GetMSVCName( function_name );
			}
			else break;
		}
	}

	if( i != hInst->num_ordinals )
	{
		Q_sprintf( errorstring, "%s error during loading names section", hInst->shortPath );
		goto table_error;
	}
	FS_Close( f );

	for( i = 0; i < hInst->num_ordinals; i++ )
	{
		if( !Q_strcmp( "GiveFnptrsToDll", hInst->names[i] ))	// main entry point for user dlls
		{
			void	*fn_offset;

			index = hInst->ordinals[i];
			fn_offset = (void *)Com_GetProcAddress( hInst, "GiveFnptrsToDll" );
			hInst->funcBase = (dword)(fn_offset) - hInst->funcs[index];
			break;
		}
	}

	if( p_Names ) Mem_Free( p_Names );
	return true;
table_error:
	// cleanup
	if( f ) FS_Close( f );
	if( p_Names ) Mem_Free( p_Names );
	FreeNameFuncGlobals( hInst );
	Com_PushLibraryError( errorstring );
	MsgDev( D_ERROR, "LoadLibrary: %s\n", errorstring );

	return false;
}

/*
================
Com_LoadLibrary

smart dll loader - can loading dlls from pack or wad files
================
*/
void *Com_LoadLibraryExt( const char *dllname, int build_ordinals_table, qboolean directpath )
{
	dll_user_t *hInst;

	hInst = FS_FindLibrary( dllname, directpath );
	if( !hInst )
	{
		char errorstring[256];
		Q_snprintf( errorstring, 256, "LoadLibraryExt: could not find %s!", dllname );
		Com_PushLibraryError(errorstring);
		return NULL; // nothing to load
	}
		
	if( hInst->custom_loader )
	{
		if( hInst->encrypted )
		{
			char errorstring[256];
			Q_snprintf( errorstring, 256, "couldn't load encrypted library %s", dllname );
			Com_PushLibraryError(errorstring);
			MsgDev( D_ERROR, "Sys_LoadLibrary: couldn't load encrypted library %s\n", dllname );
			return NULL;
		}

		hInst->hInstance = MemoryLoadLibrary( hInst->fullPath );
	}
	else
	{
		UINT errorMode = SetErrorMode(SEM_FAILCRITICALERRORS);
		hInst->hInstance = LoadLibrary( hInst->fullPath );
		SetErrorMode(errorMode);
	}

	if( !hInst->hInstance )
	{
		string errorstring;
		Q_snprintf( errorstring, MAX_STRING, "LoadLibrary failed for %s:%d", dllname, GetLastError() );
		Com_PushLibraryError( errorstring );
		Com_FreeLibrary( hInst );
		return NULL;
	}

	// if not set - FunctionFromName and NameForFunction will not working
	if( build_ordinals_table )
	{
		if( !LibraryLoadSymbols( hInst ))
		{
			string errorstring;
			Q_snprintf( errorstring, MAX_STRING, "Failed to build ordinals table for %s", dllname );
			Com_PushLibraryError( errorstring );
			//MsgDev( D_NOTE, "Sys_LoadLibrary: Loading %s - failed\n", dllname );

			Com_FreeLibrary( hInst );
			return NULL;
		}
	}

	MsgDev( D_NOTE, "Sys_LoadLibrary: Loading %s - ok\n", dllname );

	return hInst;
}

void *Com_LoadLibrary( const char *dllname, int build_ordinals_table )
{
	return Com_LoadLibraryExt( dllname, build_ordinals_table, false );
}

void *Com_GetProcAddress( void *hInstance, const char *name )
{
	dll_user_t *hInst = (dll_user_t *)hInstance;

	if( !hInst || !hInst->hInstance )
		return NULL;

	if( hInst->custom_loader )
		return (void *)MemoryGetProcAddress( hInst->hInstance, name );
	return (void *)GetProcAddress( hInst->hInstance, name );
}

void Com_FreeLibrary( void *hInstance )
{
	dll_user_t *hInst = (dll_user_t *)hInstance;

	if( !hInst || !hInst->hInstance )
		return; // already freed

	if( host.state == HOST_CRASHED )
	{
		// we need to hold down all modules, while MSVC can find error
		MsgDev( D_NOTE, "Sys_FreeLibrary: hold %s for debugging\n", hInst->dllName );
		return;
	}
	else MsgDev( D_NOTE, "Sys_FreeLibrary: Unloading %s\n", hInst->dllName );
	
	if( hInst->custom_loader )
		MemoryFreeLibrary( hInst->hInstance );
	else FreeLibrary( hInst->hInstance );

	hInst->hInstance = NULL;

	if( hInst->num_ordinals )
		FreeNameFuncGlobals( hInst );
	Mem_Free( hInst );	// done
}

void *Com_FunctionFromName(void *hInstance, const char *pName)
{
	dll_user_t	*hInst = (dll_user_t *)hInstance;
	int		i, index;

	if( !hInst || !hInst->hInstance )
		return 0;

	for( i = 0; i < hInst->num_ordinals; i++ )
	{
		if( !Q_strcmp( pName, hInst->names[i] ))
		{
			index = hInst->ordinals[i];
			return (void*)(hInst->funcs[index] + hInst->funcBase);
		}
	}
	// couldn't find the function name to return address
	return 0;
}

const char *Com_NameForFunction( void *hInstance, void * function )
{
	dll_user_t	*hInst = (dll_user_t *)hInstance;
	int		i, index;

	if( !hInst || !hInst->hInstance )
		return NULL;

	for( i = 0; i < hInst->num_ordinals; i++ )
	{
		index = hInst->ordinals[i];

		// 32 bit only cast :(
		if( ( (dword)function - hInst->funcBase ) == hInst->funcs[index] )
			return hInst->names[i];
	}
	// couldn't find the function address to return name
	return NULL;
}
#endif
