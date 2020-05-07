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

	// 상속가능으로 설정하고 파이프 핸들을 상속한다.
	saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
	saAttr.bInheritHandle = TRUE;
	saAttr.lpSecurityDescriptor = NULL;

	// 자식 프로세스의 STDOUT에 대한 Pipe 생성
	if (!CreatePipe(&g_hChildStd_OUT_Rd, &g_hChildStd_OUT_Wr, &saAttr, 0))
		ErrorExit(TEXT("StdoutRd CreatePipe"));

	// STDOUT의 read handle에 대해서 상속되지 않았는지 확인
	if (!SetHandleInformation(g_hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0))
		ErrorExit(TEXT("Stdout SetHandleInformation"));

	// 자식 프로세스의 STDIN에 대한 Pipe 생성 
	if (!CreatePipe(&g_hChildStd_IN_Rd, &g_hChildStd_IN_Wr, &saAttr, 0))
		ErrorExit(TEXT("Stdin CreatePipe"));

	// STDIN의 write handle에 대해서 상속되지 않았는지 확인
	if (!SetHandleInformation(g_hChildStd_IN_Wr, HANDLE_FLAG_INHERIT, 0))
		ErrorExit(TEXT("Stdin SetHandleInformation"));

	// STDIN/OUT 핸들을 넣어서 자식 프로세스 생성
	CreateChildProcess();
	CHAR fileName[BUFSIZE] = { 0, };
	while (true)
	{
		//scanf("%s", fileName);
		printf("[Parent][Input File Name] :");
		gets(fileName);
		// 자식의 STDIN의 read handle에 write
		WriteToPipe(fileName);

		// 자식의 STDOUT의 write handle로 부터 받은 read data
		ReadFromPipe();

		if (strcmp(fileName, "quit") == 0)
			break;

	}

	// 자식 프로세스에 다보냈으면 핸들 닫기
	if (!CloseHandle(g_hChildStd_IN_Wr))
		ErrorExit(TEXT("StdInWr CloseHandle"));


	wprintf(L"\n->End of parent execution.\n");
	getch();
	return 0;
}

void CreateChildProcess()
{
	TCHAR szCmdline[] = TEXT("stdC");
	PROCESS_INFORMATION piProcInfo = { 0, }; // PROCESS_INFORMATION 0으로 초기화
	STARTUPINFO siStartInfo = { sizeof(STARTUPINFO), }; // STARTUPINFO 0으로 초기화
	BOOL bSuccess = FALSE;

	// STARTUPINFO 값 설정
	siStartInfo.hStdError = g_hChildStd_OUT_Wr; // stdout에 대한 pipe의 write 부분 지정
	siStartInfo.hStdOutput = g_hChildStd_OUT_Wr; // stdout에 대한 pipe의 write 부분 지정
	siStartInfo.hStdInput = g_hChildStd_IN_Rd; // stdin에 대한 pipe의 read 부분 지정
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
		// 자식 프로세스가 바로 종료 되도록 Process, Thread, stdout/in
		CloseHandle(piProcInfo.hProcess);
		CloseHandle(piProcInfo.hThread);
		CloseHandle(g_hChildStd_OUT_Wr);
		CloseHandle(g_hChildStd_IN_Rd);
	}
}

void WriteToPipe(PCHAR chBuf) // 자식 프로세스로 STDIN으로 정보 던지기
{
	DWORD dwRead, dwWritten;
	BOOL bSuccess = FALSE;

	printf("[Parent][Send] : %s\n", chBuf);
	// stdin pipe의 write handle에 버퍼 내용과 크기 넣고 쓴 크기 dwWritten에 반환
	bSuccess = WriteFile(g_hChildStd_IN_Wr, chBuf, strlen(chBuf), &dwWritten, NULL);
}

void ReadFromPipe(void) // 자식에서 STDOUT으로 나오는 정보 가져오기
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