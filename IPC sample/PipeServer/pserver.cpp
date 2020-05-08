#include <stdio.h>
#include <Windows.h>
#include <tchar.h>

#define PIPE_NAME L"\\\\.\\pipe\\Hpipe" // ������ �̸� ����

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

		//dwSendSize -> NULL ������ ����Ʈ ��
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

		//dwRecvSize -> NULL ������ ����Ʈ ��
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
	WCHAR pipe_name[] = PIPE_NAME;     // #define �� ������ �̸�

	puts("==== Server ==== ");
	//NamedPipe ����
	hNamedPipe = CreateNamedPipe(
		pipe_name,
		PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,
		PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
		PIPE_UNLIMITED_INSTANCES,
		0,
		0,
		20000,       // ��� Timeout �ð�
		NULL
		);
	if (hNamedPipe == INVALID_HANDLE_VALUE)
	{
		printf("CreateNamePipe error! \n");
		return -1;
	}

	while (1)
	{
		//������ Named Pipe�� �ڵ��� ������ �� ������ ���..
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

	DisconnectNamedPipe(hNamedPipe); // ���� ����
	CloseHandle(hNamedPipe); // HANDLE ��ȯ
	return 1;
}