#include <windows.h>
#include <QString>
#include <QDebug>
#include <QApplication>

HANDLE s_hMutex = NULL;
bool isConsole;

void releaseMutex()
{
#ifdef WIN32
	if (NULL != s_hMutex)
	{
		CloseHandle(s_hMutex);
		s_hMutex = NULL;
	}
#endif
}

BOOL CALLBACK enumWinFun(HWND hWnd, LPARAM lParam)
{
	DWORD pid;
	static QString progName = "DiskHealth.exe";
	::GetWindowThreadProcessId(hWnd, &pid);
	HANDLE handle = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pid);
	QString processPath;
	if (handle) {
		DWORD buffSize = MAX_PATH;
		wchar_t buf[MAX_PATH];
		QueryFullProcessImageNameW(handle, 0, buf, &buffSize);
		processPath = QString::fromWCharArray(buf);
		CloseHandle(handle);
	}

	do
	{
		if(processPath.right(progName.length()) == progName)
		{
			::SetForegroundWindow(hWnd);
			::BringWindowToTop(hWnd);

			QStringList list = qApp->arguments();

			int diskIndex = -1;

			for (int i = 0; i < list.size(); ++i)
			{
				if (list[i].contains("diskHealthIndex="))
				{
					auto v = list[i].split('=');
					if (v.size() >= 2)
					{
						diskIndex = v[1].toInt();
					}
				}
			}

			COPYDATASTRUCT cds;
			cds.dwData = 0x123;

			if (diskIndex != -1)
			{
				cds.cbData = sizeof(diskIndex);
				cds.lpData = &diskIndex;
			}
			else
			{
				cds.cbData = sizeof(COPYDATASTRUCT);
				cds.lpData = NULL;
			}
			SendMessage(hWnd, WM_COPYDATA, 0, (LPARAM)&cds);

			return FALSE;
		}
	} while (0);

	return TRUE;
};

bool CheckHasAnotherAppInstance()
{
	bool bRet = false;
#ifdef _WIN32
	const static wchar_t* MUTEX_NAME = isConsole ? L"EaseUS_DiskHealthConsole" : L"EaseUS_DiskHealth";
	s_hMutex = CreateMutexW(NULL, TRUE, MUTEX_NAME);
	DWORD dwError = ::GetLastError();
	if (ERROR_ALREADY_EXISTS == dwError)
	{
		bRet = true;
		if (!isConsole)
		{
			LONG p = 0;
			EnumWindows(enumWinFun, (LPARAM)&p);
			releaseMutex();
		}
	}
#endif
	return bRet;
}