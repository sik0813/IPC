#include <Windows.h>
#include <windowsx.h>
#include <tchar.h>
#include "resource.h"
#include "..\Mydll\Mydll.h"

#define chHANDLE_DLGMSG(hWnd, message, fn)                 \
   case (message): return (SetDlgMsgResult(hWnd, uMsg,     \
      HANDLE_##message((hWnd), (wParam), (lParam), (fn))))

HANDLE g_hMainDlg;


// 다이얼로그 초기화
BOOL Dlg_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam) {
	/*
	1. 
	*/
	SetWindowText(GetDlgItem(hwnd, IDC_CONTENT), TEXT(""));
	CFStruct myST;
	myST.processId = GetCurrentProcessId();
	Client(myST);
	g_hMainDlg = hwnd;
	return(TRUE);
}


///////////////////////////////////////////////////////////////////////////////


void Dlg_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify) {

	switch (id) {
	case IDCANCEL:
		EndDialog(hwnd, id);
		break;
	case IDC_REFRESH:
		break;
	case IDC_SAVE:
		break;
	}
}


INT_PTR WINAPI Dlg_Proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
		chHANDLE_DLGMSG(hwnd, WM_INITDIALOG, Dlg_OnInitDialog);
		chHANDLE_DLGMSG(hwnd, WM_COMMAND, Dlg_OnCommand);
	}

	return(FALSE);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow){
	DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG), NULL, Dlg_Proc);
	return 0;
}