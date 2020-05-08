#include <stdio.h>
#include <Windows.h>
#include <tchar.h>

#define PIPE_NAME L"\\\\.\\pipe\\Hpipe" // 파이프 이름 설정

int ConnectClient(HANDLE hNamePipe)
{
	TCHAR receiveBuf[100];
	TCHAR sendBuf[100];
	DWORD dwRecvSize;
	DWORD dwSendSize;

	while (1)
	{
		_tprintf(_T("[Server][Send] : "));
		_tscanf(_T("%s"), sendBuf);

		//dwSendSize -> NULL 포함한 바이트 수
		if (!(WriteFile(
			hNamePipe,
			sendBuf,
			(_tcslen(sendBuf) + 1)*sizeof(TCHAR),
			&dwSendSize,
			NULL
			)))          // 
		{
			_tprintf(_T("Send error! \n"));
			return -1;
		}
		FlushFileBuffers(hNamePipe);

		//dwRecvSize -> NULL 포함한 바이트 수
		if (!(ReadFile(
			hNamePipe,
			receiveBuf,
			sizeof(receiveBuf) - sizeof(TCHAR) * 1,
			&dwRecvSize,
			NULL
			)))
		{
			printf("Receive error! \n");
			return -1;
		}
		receiveBuf[dwRecvSize / sizeof(TCHAR) - 1] = '\0';

		_tprintf(_T("[Server][Receive] : %s \n"), receiveBuf);
	}

	return 1;
}


int main(void)
{
	HANDLE hNamedPipe;
	WCHAR pipe_name[] = PIPE_NAME;     // #define 한 파이프 이름

	puts("==== Server ==== ");
	//NamedPipe 생성
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
		printf("CreateNamePipe error! \n");
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

	DisconnectNamedPipe(hNamedPipe); // 연결 해제
	CloseHandle(hNamedPipe); // HANDLE 반환
	return 1;
}