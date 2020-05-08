#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <tchar.h>

#define PIPE_NAME	"\\\\.\\pipe\\Hpipe"     // 서버의 파이프 이름과 동일한 파이프 사용

int ConnectServer(HANDLE hNamePipe)
{
	TCHAR receiveBuf[100];
	TCHAR sendBuf[100];
	DWORD dwRecvSize;
	DWORD dwSendSize;

	while (1)
	{
		if (!(ReadFile(
			hNamePipe,
			receiveBuf,
			sizeof(receiveBuf) - sizeof(TCHAR) * 1,
			&dwRecvSize,
			NULL
			)))
		{
			_tprintf(_T("Receive error! \n"));
			return -1;
		}

		receiveBuf[dwRecvSize / sizeof(TCHAR) - 1] = _T('\x00');
		_tprintf(_T("[Client][Receive] : %s \n"), receiveBuf);
		_tprintf(_T("[Client][Send] : "));
		_tscanf(_T("%s"), sendBuf);

		if (!(WriteFile(
			hNamePipe,
			sendBuf,
			(_tcslen(sendBuf) + 1)*sizeof(TCHAR),
			&dwSendSize,
			NULL
			)))
		{
			_tprintf(_T("Send error! \n"));
			return -1;
		}
		FlushFileBuffers(hNamePipe);
	}

	return 1;
}



int main(void)
{
	TCHAR pipe_name[] = _T(PIPE_NAME);
	HANDLE hNamePipe;

	_tprintf(_T("==== Client ==== \n"));
	while (1)
	{
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
		else
		{
			if (ConnectServer(hNamePipe) == -1)
				break;
		}
	}
	CloseHandle(hNamePipe);

	return 0;
}