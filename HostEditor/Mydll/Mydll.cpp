// Mydll.cpp : DLL 응용 프로그램을 위해 내보낸 함수를 정의합니다.
//

#include "stdafx.h"
#include "Mydll.h"

#define MyPipe L"\\\\.\\pipe\\MyPipe"

// Server API
EXPORT int Server(void)
{
	HANDLE hNamedPipe;
	WCHAR pipe_name[] = MyPipe;
	hNamedPipe = CreateNamedPipe(
		pipe_name,
		PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,
		PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
		PIPE_UNLIMITED_INSTANCES,
		0,
		0,
		20000,       // 대기 Timeout 시간
		NULL
	);
	if (hNamedPipe == INVALID_HANDLE_VALUE)
	{
		//printf("CreateNamePipe error! \n");
		return -1;
	}
	while (1)
	{
		//생성한 Named Pipe의 핸들을 누군가 얻어갈 때까지 대기..
		if (!(ConnectNamedPipe(hNamedPipe, NULL)))
		{
			CloseHandle(hNamedPipe);
			return -1;
		}
		else
		{
			if (ConnectClient(hNamedPipe) == -1)
				break;
		}
	}
	CloseHandle(hNamedPipe);
	return 0;
}


int ConnectClient(HANDLE hNamePipe)
{
	TCHAR receiveBuf[100];
	TCHAR sendBuf[100];
	DWORD dwRecvSize;
	DWORD dwSendSize;
	CFStruct myCF;
	while (1)
	{
		memset(&myCF, 0, sizeof(myCF)); // 데이터 수신 전 초기화
		//dwRecvSize -> NULL 포함한 바이트 수
		if (!(ReadFile(
			hNamePipe,
			&myCF,
			sizeof(myCF),
			&dwRecvSize,
			NULL
			)))
		{
			_tprintf(_T("Receive error! \n"));
			return -1;
		}

		//dwSendSize -> NULL 포함한 바이트 수
		if (!(WriteFile(
			hNamePipe,
			&myCF,
			sizeof(int),
			&dwSendSize,
			NULL
			)))          // 
		{
			_tprintf(_T("Send error! \n"));
			return -1;
		}
		FlushFileBuffers(hNamePipe);


	}

	return 1;
}

//////////////////////////////////////////////////////////////////////////////////////////////

// Client API
EXPORT int Client(CFStruct inputSt)
{
	HANDLE hNamePipe;
	WCHAR pipe_name[] = MyPipe;
	
	// 서버에서 생성한 파이프 이름으로 핸들 열기 CallNamedPipe
	hNamePipe = CreateFile(pipe_name, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (hNamePipe == INVALID_HANDLE_VALUE)
	{
		_tprintf(_T("CreateFile error! \n"));
		return -1;
	}

	DWORD pipeMode = PIPE_READMODE_MESSAGE | PIPE_WAIT;
	if (!(SetNamedPipeHandleState(hNamePipe, &pipeMode, NULL, NULL)))
	{
		_tprintf(_T("SetNamedPipeHandleState error! \n"));
		CloseHandle(hNamePipe);
		return -1;
	}
	ConnectServer(hNamePipe, inputSt);
	CloseHandle(hNamePipe);
	return 0;
}

int ConnectServer(HANDLE hNamePipe, CFStruct myCF)
{
	TCHAR receiveBuf[100];
	TCHAR sendBuf[100];
	DWORD dwRecvSize;
	DWORD dwSendSize;
	HANDLE myProcess = OpenProcess(PROCESS_ALL_ACCESS, TRUE, myCF.processId);
	if (!(ReadFile(
		hNamePipe,
		&myCF,
		sizeof(myCF),
		&dwRecvSize,
		NULL
		)))
	{
		_tprintf(_T("Receive error! \n"));
		return -1;
	}

	//receiveBuf[dwRecvSize / sizeof(TCHAR) - 1] = _T('\x00');
	//_tprintf(_T("[Client][Receive] : %d, %d \n"), tmpS.index, tmpS.nextIndex);
	//_tprintf(_T("[Client][Send] : "));
	//_tscanf(_T("%d %d"), &tmpS.index, &tmpS.nextIndex);

	//DuplicateHandle(GetCurrentProcess(), );

	if (!(WriteFile(
		hNamePipe,
		&myCF,
		sizeof(myCF),
		&dwSendSize,
		NULL
		)))
	{
		_tprintf(_T("Send error! \n"));
		return -1;
	}
	FlushFileBuffers(hNamePipe);

	return 1;
}
