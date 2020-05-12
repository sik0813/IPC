#include <Windows.h>
#include <tchar.h>

#ifdef MYDLL_EXPORTS
#define MYDLL_API __declspec(dllexport)
#else
#define MYDLL_API __declspec(dllimport)
#endif

#ifdef __cplusplus
#define EXPORT extern "C" MYDLL_API
#else
#define EXPORT extern MYDLL_API
#endif

EXPORT int Server(void);
EXPORT int Client(CFStruct inputSt);
int ConnectClient(HANDLE hNamePipe);
int ConnectServer(HANDLE hNamePipe, CFStruct SendSt);


typedef struct _CFStruct{
	DWORD processId;
	WCHAR FileName[1000];
	DWORD dwDesiredAccess;
	DWORD dwShareMode;
	SECURITY_ATTRIBUTES SecurityAttributes;
	DWORD dwCreationDisposition;
	DWORD dwFlagsAndAttributes;
	HANDLE hTemplateFile;
} CFStruct;