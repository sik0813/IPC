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
		&cbMessage,                   // 있으면 크기 없으면 MAILSLOT_NO_MESSAGE, NULL 사용가능
		&cMessage,                    // 전체 메시지 개수, NULL 사용가능
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

	while (cMessage != 0)  // 마지막으로 읽은 메시지의 개수
	{
		lpszBuffer = (LPTSTR)GlobalAlloc(GPTR, cbMessage); // 메시지 크기 만큼 할당 후 초기화
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

		// 결과 출력
		_tprintf(_T("Message #%d of %d\nContents of the mailslot: %s\n"), cAllMessages - cMessage + 1, cAllMessages, lpszBuffer);

		GlobalFree((HGLOBAL)lpszBuffer); // 메모리 할당 해제

		fResult = GetMailslotInfo(hSlot, // mailslot handle 
			NULL,						   // any size
			&cbMessage,                   // 있으면 크기 없으면 MAILSLOT_NO_MESSAGE, NULL 사용가능
			&cMessage,                    // 전체 메시지 개수, NULL 사용가능
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
		0,                             // 최대 size, 0: any size
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