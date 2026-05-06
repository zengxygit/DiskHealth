#include <windows.h>
#include "CommCtrl.h"

HWND FindTrayWnd()
{
	HWND hWnd = NULL;
	HWND hWndPaper = NULL;

	if ((hWnd = FindWindow(L"Shell_TrayWnd", NULL)) != NULL)
	{
		if ((hWnd = FindWindowEx(hWnd, 0, L"TrayNotifyWnd", NULL)) != NULL)
		{
			hWndPaper = FindWindowEx(hWnd, 0, L"SysPager", NULL);
			if (!hWndPaper)
				hWnd = FindWindowEx(hWnd, 0, L"ToolbarWindow32", NULL);
			else
				hWnd = FindWindowEx(hWndPaper, 0, L"ToolbarWindow32", NULL);
		}
	}

	return hWnd;
}

HWND FindNotifyIconOverflowWindow()
{
	HWND hWnd = NULL;

	hWnd = FindWindow(L"NotifyIconOverflowWindow", NULL);
	if (hWnd != NULL)
	{
		hWnd = FindWindowEx(hWnd, NULL, L"ToolbarWindow32", NULL);
	}

	return hWnd;
}

bool EnumNotifyWindow(RECT &rect, HWND hWnd, HWND trayWnd)
{
	rect = { 0,0,0,0 };
	bool bSuc = false;
	unsigned long lngPID = 0;
	long ret = 0, lngButtons = 0;
	long lngHwndAdr = 0, lngHwnd = 0;//,lngTextAdr,lngButtonID;
	HANDLE hProcess = NULL;
	LPVOID lngAddress = NULL, lngRect = NULL;
	BOOL bIsWow64 = FALSE;
	IsWow64Process(GetCurrentProcess(), &bIsWow64);
	int offset = 16;

	if (hWnd != NULL)
	{
		ret = GetWindowThreadProcessId(hWnd, &lngPID);
		if (ret != 0 && lngPID != 0)
		{
			hProcess = OpenProcess(PROCESS_ALL_ACCESS | PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE, 0, lngPID);//
			if (hProcess != NULL)
			{
				lngAddress = VirtualAllocEx(hProcess, 0, 0x4096, MEM_COMMIT, PAGE_READWRITE);
				lngRect = VirtualAllocEx(hProcess, 0, sizeof(RECT), MEM_COMMIT, PAGE_READWRITE);
				lngButtons = SendMessage(hWnd, TB_BUTTONCOUNT, 0, 0); //发送消息获取托盘button数量

				if (lngAddress != NULL && lngRect != NULL)
				{
					for (int i = 0; i < lngButtons; i++)
					{
						RECT rc = { 0 };
						int j = i;
						ret = SendMessage(hWnd, TB_GETBUTTON, j, long(lngAddress));//发送消息获取托盘项数据起始地址
						ret = ReadProcessMemory(hProcess, LPVOID(long(lngAddress) + offset), &lngHwndAdr, 4, 0);
						if (ret != 0 && lngHwndAdr != -1)
						{
							ret = ReadProcessMemory(hProcess, LPVOID(lngHwndAdr), &lngHwnd, 4, 0);//获取句柄
							if (ret != 0 && (HWND)lngHwnd == trayWnd)
							{
								ret = SendMessage(hWnd, TB_GETITEMRECT, (WPARAM)j, (LPARAM)lngRect); //发送消息获取托盘项区域数据
								ret = ReadProcessMemory(hProcess, lngRect, &rc, sizeof(rc), 0);  //读取托盘区域数据
								if (ret != 0)
								{
									POINT zero = { 0,0 };
									POINT pt = { rc.right,rc.bottom };

									ClientToScreen(hWnd, &zero);
									ClientToScreen(hWnd, &pt);

									rect = { zero.x,zero.y, pt.x, pt.y };
								}
								bSuc = true;//在普通托盘区找到，在溢出区不再查找
								break;
							}
						}
					}
				}
				if (lngAddress != NULL)
				{
					VirtualFreeEx(hProcess, lngAddress, 0x4096, MEM_DECOMMIT);
					VirtualFreeEx(hProcess, lngAddress, 0, MEM_RELEASE);
				}
				if (lngRect != NULL)
				{
					VirtualFreeEx(hProcess, lngRect, sizeof(RECT), MEM_DECOMMIT);
					VirtualFreeEx(hProcess, lngRect, 0, MEM_RELEASE);
				}
				CloseHandle(hProcess);
			}
		}
	}
	return bSuc;
}
