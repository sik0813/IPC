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
	//�ڽ��� STDIN/OUT �ڵ� ������ ����
	hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	hStdin = GetStdHandle(STD_INPUT_HANDLE);
	if ((hStdout == INVALID_HANDLE_VALUE) || (hStdin == INVALID_HANDLE_VALUE))
		ExitProcess(1);
	CHAR chBufCopy[BUFSIZE];
	while (true)
	{
		// STDIN���� ������ ������ ���ۿ� ����
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

		// ���� �о �θ𿡰� �Ѱ��ֱ�
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
			// ���� �о chBuf�� �����ϰ� ���� ũ�⸦ dwRead�� ����
			bSuccess = ReadFile(hInputFile, chBuf, BUFSIZE, &dwRead, NULL);
			if (!bSuccess || dwRead == 0) break;

			// STDOUT �ڵ鿡 chBuf ���� ����
			bSuccess = WriteFile(hStdout, chBuf, dwRead, &dwWritten, NULL);
			if (!bSuccess)
				break;
		}
		// STDOUT �ڵ鿡 chBuf ���� ����
		bSuccess = WriteFile(hStdout, "\nCEND", strlen("\nCEND"), &dwWritten, NULL);
		CloseHandle(hInputFile);
	}
	

	// STDOUT �ڵ鿡 chBuf ���� ����
	char end[] = "\n child process end \n";
	bSuccess = WriteFile(hStdout, end, strlen(end), &dwWritten, NULL);

	return 0;
}