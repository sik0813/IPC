#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <strsafe.h>

#define BUFSIZE 4096 

int main(void)
{
	CHAR chBuf[BUFSIZE] = { 0 };
	DWORD dwRead, dwWritten;
	HANDLE hStdin, hStdout;
	BOOL bSuccess;
	HANDLE hInputFile = NULL;
	//자신의 STDIN/OUT 핸들 가지고 오기
	hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	hStdin = GetStdHandle(STD_INPUT_HANDLE);
	if ((hStdout == INVALID_HANDLE_VALUE) || (hStdin == INVALID_HANDLE_VALUE))
		ExitProcess(1);
	CHAR chBufCopy[BUFSIZE];
	while (true)
	{
		// STDIN으로 들어오는 데이터 버퍼에 저장
		bSuccess = ReadFile(hStdin, chBuf, BUFSIZE, &dwRead, NULL);
		if (!bSuccess){
			WriteFile(hStdout, "recieve error filename\n", strlen("recieve error filename\n"), &dwWritten, NULL);
			return 0;
		}
		StringCchCopyA(chBufCopy, dwRead+1, chBuf);
		//WriteFile(hStdout, "[Child][Receive] : ", strlen("[Child][Receive] : "), &dwWritten, NULL);
		//WriteFile(hStdout, chBufCopy, strlen(chBufCopy), &dwWritten, NULL);
		//WriteFile(hStdout, "\n", 1, &dwWritten, NULL);
		
		if (strcmp(chBufCopy, "quit") == 0){
			break;
		}
			
		int nLen = MultiByteToWideChar(CP_ACP, 0, chBufCopy, strlen(chBufCopy), NULL, NULL);
		WCHAR *fileName = (WCHAR*)malloc(nLen * sizeof(WCHAR));
		MultiByteToWideChar(CP_ACP, 0, chBufCopy, strlen(chBufCopy), fileName, nLen);
		fileName[dwRead] = '\0';

		// 파일 읽어서 부모에게 넘겨주기
		hInputFile = CreateFile(
			fileName,
			GENERIC_READ,
			0,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_READONLY,
			NULL);

		if (hInputFile == INVALID_HANDLE_VALUE) {
			bSuccess = WriteFile(hStdout, "Cannnot Read File", strlen("Cannnot Read File"), &dwWritten, NULL);
			bSuccess = WriteFile(hStdout, "\nCEND", strlen("\nCEND"), &dwWritten, NULL);
			continue;
			//return 0;
		}
		//WriteFile(hStdout, "[Child][Send] : ", strlen("[Child][Send] : "), &dwWritten, NULL);
		WriteFile(hStdout, "[FileContent] : \n", strlen("[FileContent] : \n"), &dwWritten, NULL);
		for (;;){
			// 파일 읽어서 chBuf에 저장하고 읽은 크기를 dwRead에 저장
			bSuccess = ReadFile(hInputFile, chBuf, BUFSIZE, &dwRead, NULL);
			if (!bSuccess || dwRead == 0) break;

			// STDOUT 핸들에 chBuf 내용 쓰기
			bSuccess = WriteFile(hStdout, chBuf, dwRead, &dwWritten, NULL);
			if (!bSuccess)
				break;
		}
		// STDOUT 핸들에 chBuf 내용 쓰기
		bSuccess = WriteFile(hStdout, "\nCEND", strlen("\nCEND"), &dwWritten, NULL);
		CloseHandle(hInputFile);
	}
	

	// STDOUT 핸들에 chBuf 내용 쓰기
	char end[] = "\n child process end \n";
	bSuccess = WriteFile(hStdout, end, strlen(end), &dwWritten, NULL);

	return 0;
}