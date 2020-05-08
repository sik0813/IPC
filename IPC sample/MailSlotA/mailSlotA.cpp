// Read MailSlot
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <strsafe.h>

HANDLE hSlot;
LPTSTR SlotName = _T("\\\\.\\mailslot\\sample_mailslot");
DWORD printFlag = FALSE;
BOOL ReadSlot()
{
	DWORD cbMessage, cMessage, cbRead;
	BOOL fResult;
	LPTSTR lpszBuffer;
	DWORD cAllMessages;
	//HANDLE hEvent;
	OVERLAPPED ov;

	cbMessage = cMessage = cbRead = 0;

	//hEvent = CreateEvent(NULL, FALSE, FALSE, _T("ExampleSlot"));
	//if (NULL == hEvent)
	//	return FALSE;
	//ov.Offset = 0;
	//ov.OffsetHigh = 0;
	//ov.hEvent = hEvent;

	fResult = GetMailslotInfo(hSlot, // mailslot handle 
		NULL,						   // any size
		&cbMessage,                   // ������ ũ�� ������ MAILSLOT_NO_MESSAGE, NULL ��밡��
		&cMessage,                    // ��ü �޽��� ����, NULL ��밡��
		NULL);						 // no read time-out 

	if (!fResult)
	{
		printf("GetMailslotInfo failed with %d.\n", GetLastError());
		//CloseHandle(hEvent);
		return FALSE;
	}

	//if (cbMessage == MAILSLOT_NO_MESSAGE)
	if (cMessage == 0)
	{
		if (!printFlag) printf("Mailslot Empty\n");
		printFlag = TRUE;
		//CloseHandle(hEvent);
		return TRUE;
	}
	printFlag = FALSE;
	cAllMessages = cMessage;

	while (cMessage != 0)  // ���������� ���� �޽����� ����
	{
		lpszBuffer = (LPTSTR)GlobalAlloc(GPTR, cbMessage); // �޽��� ũ�� ��ŭ �Ҵ� �� �ʱ�ȭ
		if (NULL == lpszBuffer)
			return FALSE;
		lpszBuffer[0] = '\0';

		fResult = ReadFile(hSlot,
			lpszBuffer,
			cbMessage,
			&cbRead,
			NULL); //&ov);
		
		if (!fResult)
		{
			printf("ReadFile failed with %d.\n", GetLastError());
			GlobalFree((HGLOBAL)lpszBuffer);
			return FALSE;
		}

		// ��� ���
		_tprintf(_T("Message #%d of %d\nContents of the mailslot: %s\n"), cAllMessages - cMessage + 1, cAllMessages, lpszBuffer);

		GlobalFree((HGLOBAL)lpszBuffer); // �޸� �Ҵ� ����

		fResult = GetMailslotInfo(hSlot, // mailslot handle 
			NULL,						   // any size
			&cbMessage,                   // ������ ũ�� ������ MAILSLOT_NO_MESSAGE, NULL ��밡��
			&cMessage,                    // ��ü �޽��� ����, NULL ��밡��
			NULL);						 // no read time-out 

		if (!fResult)
		{
			printf("GetMailslotInfo failed (%d)\n", GetLastError());
			return FALSE;
		}
	}
	//CloseHandle(hEvent);
	return TRUE;
}

BOOL WINAPI MakeSlot(LPTSTR lpszSlotName)
{
	hSlot = CreateMailslot(lpszSlotName,
		0,                             // �ִ� size, 0: any size
		MAILSLOT_WAIT_FOREVER,         // MAILSLOT_WAIT_FOREVER: no timeout, 0: return immediately
		NULL); // default Security Attributes

	if (hSlot == INVALID_HANDLE_VALUE)
	{
		printf("CreateMailslot failed with %d\n", GetLastError());
		return FALSE;
	}
	return TRUE;
}

void main()
{
	MakeSlot(SlotName);

	while (TRUE)
	{
		ReadSlot();
		Sleep(3000);
	}
}