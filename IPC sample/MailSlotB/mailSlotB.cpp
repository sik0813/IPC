// Wirte MailSlot
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <strsafe.h>

LPCTSTR SlotName = _T("\\\\.\\mailslot\\sample_mailslot");

BOOL WriteSlot(HANDLE hSlot, LPTSTR lpszMessage)
{
	BOOL fResult;
	DWORD cbWritten;

	fResult = WriteFile(hSlot,
		lpszMessage,
		(lstrlen(lpszMessage) + 1)*sizeof(TCHAR),
		&cbWritten,
		NULL);

	if (!fResult)
	{
		printf("WriteFile failed with %d.\n", GetLastError());
		return FALSE;
	}

	printf("Slot written to successfully.\n");

	return TRUE;
}

int main()
{
	HANDLE hFile;

	hFile = CreateFile(SlotName,
		GENERIC_WRITE,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		printf("CreateFile failed with %d.\n", GetLastError());
		return FALSE;
	}

	WriteSlot(hFile, _T("first message"));
	WriteSlot(hFile, _T("second message"));

	Sleep(3000);

	WriteSlot(hFile, _T("third message"));

	CloseHandle(hFile);

	return TRUE;
}