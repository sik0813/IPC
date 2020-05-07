#include <windows.h> 
#include <tchar.h>
#include <stdio.h> 
#include <strsafe.h>
#include <conio.h>

#define BUFSIZE 4096 

HANDLE g_hChildStd_IN_Rd = NULL;
HANDLE g_hChildStd_IN_Wr = NULL;
HANDLE g_hChildStd_OUT_Rd = NULL;
HANDLE g_hChildStd_OUT_Wr = NULL;

HANDLE g_hInputFile = NULL;

void CreateChildProcess(void);
void WriteToPipe(PCHAR fileName);
void ReadFromPipe(void);
void ErrorExit(PTSTR);

int _tmain(int argc, TCHAR *argv[])
{
	SECURITY_ATTRIBUTES saAttr;

	printf("\n->Start of parent execution.\n");

	// ��Ӱ������� �����ϰ� ������ �ڵ��� ����Ѵ�.
	saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
	saAttr.bInheritHandle = TRUE;
	saAttr.lpSecurityDescriptor = NULL;

	// �ڽ� ���μ����� STDOUT�� ���� Pipe ����
	if (!CreatePipe(&g_hChildStd_OUT_Rd, &g_hChildStd_OUT_Wr, &saAttr, 0))
		ErrorExit(TEXT("StdoutRd CreatePipe"));

	// STDOUT�� read handle�� ���ؼ� ��ӵ��� �ʾҴ��� Ȯ��
	if (!SetHandleInformation(g_hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0))
		ErrorExit(TEXT("Stdout SetHandleInformation"));

	// �ڽ� ���μ����� STDIN�� ���� Pipe ���� 
	if (!CreatePipe(&g_hChildStd_IN_Rd, &g_hChildStd_IN_Wr, &saAttr, 0))
		ErrorExit(TEXT("Stdin CreatePipe"));

	// STDIN�� write handle�� ���ؼ� ��ӵ��� �ʾҴ��� Ȯ��
	if (!SetHandleInformation(g_hChildStd_IN_Wr, HANDLE_FLAG_INHERIT, 0))
		ErrorExit(TEXT("Stdin SetHandleInformation"));

	// STDIN/OUT �ڵ��� �־ �ڽ� ���μ��� ����
	CreateChildProcess();
	CHAR fileName[BUFSIZE] = { 0, };
	while (true)
	{
		//scanf("%s", fileName);
		printf("[Parent][Input File Name] :");
		gets(fileName);
		// �ڽ��� STDIN�� read handle�� write
		WriteToPipe(fileName);

		// �ڽ��� STDOUT�� write handle�� ���� ���� read data
		ReadFromPipe();

		if (strcmp(fileName, "quit") == 0)
			break;

	}

	// �ڽ� ���μ����� �ٺ������� �ڵ� �ݱ�
	if (!CloseHandle(g_hChildStd_IN_Wr))
		ErrorExit(TEXT("StdInWr CloseHandle"));


	wprintf(L"\n->End of parent execution.\n");
	getch();
	return 0;
}

void CreateChildProcess()
{
	TCHAR szCmdline[] = TEXT("stdC");
	PROCESS_INFORMATION piProcInfo = { 0, }; // PROCESS_INFORMATION 0���� �ʱ�ȭ
	STARTUPINFO siStartInfo = { sizeof(STARTUPINFO), }; // STARTUPINFO 0���� �ʱ�ȭ
	BOOL bSuccess = FALSE;

	// STARTUPINFO �� ����
	siStartInfo.hStdError = g_hChildStd_OUT_Wr; // stdout�� ���� pipe�� write �κ� ����
	siStartInfo.hStdOutput = g_hChildStd_OUT_Wr; // stdout�� ���� pipe�� write �κ� ����
	siStartInfo.hStdInput = g_hChildStd_IN_Rd; // stdin�� ���� pipe�� read �κ� ����
	siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

	// Create the child process. 
	bSuccess = CreateProcess(NULL,
		szCmdline,     // command line 
		NULL,          // process security attributes
		NULL,          // primary thread security attributes 
		TRUE,          // handles are inherited 
		0, //CREATE_NEW_CONSOLE,             // creation flags 
		NULL,          // use parent's environment 
		NULL,          // use parent's current directory 
		&siStartInfo,  // STARTUPINFO
		&piProcInfo);  // PROCESS_INFORMATION 

	if (!bSuccess)
		ErrorExit(TEXT("CreateProcess"));
	else
	{
		// �ڽ� ���μ����� �ٷ� ���� �ǵ��� Process, Thread, stdout/in
		CloseHandle(piProcInfo.hProcess);
		CloseHandle(piProcInfo.hThread);
		CloseHandle(g_hChildStd_OUT_Wr);
		CloseHandle(g_hChildStd_IN_Rd);
	}
}

void WriteToPipe(PCHAR chBuf) // �ڽ� ���μ����� STDIN���� ���� ������
{
	DWORD dwRead, dwWritten;
	BOOL bSuccess = FALSE;

	printf("[Parent][Send] : %s\n", chBuf);
	// stdin pipe�� write handle�� ���� ����� ũ�� �ְ� �� ũ�� dwWritten�� ��ȯ
	bSuccess = WriteFile(g_hChildStd_IN_Wr, chBuf, strlen(chBuf), &dwWritten, NULL);
}

void ReadFromPipe(void) // �ڽĿ��� STDOUT���� ������ ���� ��������
{
	DWORD dwRead, dwWritten;
	CHAR chBuf[BUFSIZE];
	BOOL bSuccess = FALSE;
	HANDLE hParentStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD quitFlag = FALSE;
	printf("[Parent][Receive] : ");
	for (;;)
	{
		bSuccess = ReadFile(g_hChildStd_OUT_Rd, chBuf, BUFSIZE, &dwRead, NULL);
		if (!bSuccess) break;
		if ((chBuf[dwRead - 4] == 'C'
			&& chBuf[dwRead - 3] == 'E'
			&& chBuf[dwRead - 2] == 'N'
			&& chBuf[dwRead - 1] == 'D')){
			dwRead -= 4;
			quitFlag = TRUE;
		}

		bSuccess = WriteFile(hParentStdOut, chBuf, dwRead, &dwWritten, NULL);
		if (!bSuccess || quitFlag) break;
	}
}

void ErrorExit(PTSTR lpszFunction)
{
	DWORD dw = GetLastError();
	printf("%s | error code : %d", lpszFunction, dw);
	getch();
	ExitProcess(1);
}