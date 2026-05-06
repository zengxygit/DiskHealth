#include "CTrayIcon.h"

#include <Commctrl.h>
#include <QAction>
#include <QMenu>
#include <QTimer>
#include <QEvent>
#include <QDebug>
#include <QLabel>
#include <QFile>
#include <QDateTime>
#include <QApplication>

#define NEW new(std::nothrow)

#define DEFAULT_ICON ":/res/ico_logo_pallets_disksmart16.png"
#define WM_TRAYNOTIFY 10086

Q_GUI_EXPORT HICON qt_pixmapToWinHICON(const QPixmap &p);

extern HWND FindTrayWnd();
extern HWND FindNotifyIconOverflowWindow();
extern bool EnumNotifyWindow(RECT &rect, HWND hWnd, HWND trayWnd);

static NOTIFYICONDATA m_nid = { 0 };

CTrayIcon::CTrayIcon(QObject *parent):QObject(parent)
{
	do
	{
		m_icoDefault.addFile(DEFAULT_ICON);

		m_pLeaveTimer = NEW QTimer(this);
		connect(m_pLeaveTimer, &QTimer::timeout, this, &CTrayIcon::CheckMouse);

		m_menu = new QMenu;
		QAction * act1 = m_menu->addAction("");
		QAction * act2 = m_menu->addAction("");

		m_actions.append(act1);
		m_actions.append(act2);

		act1->setText(QObject::tr("Show"));
		act2->setText(QObject::tr("Exit"));

		act1->setIcon(QIcon(":/res/ico_show16.png"));
		act2->setIcon(QIcon(":/res/ico_exit16.png"));
		
		connect(act1, &QAction::triggered, this, &CTrayIcon::sigShowFrame);
		connect(act2, &QAction::triggered, this, &CTrayIcon::sigExit);

		m_pSysIcon = new QWidget;

		onCreateIcon();

		connect(&m_createIconTimer, &QTimer::timeout, this, &CTrayIcon::onCreateIcon);

		qApp->installNativeEventFilter(this);
	} while (false);

}

CTrayIcon::~CTrayIcon()
{
	Shell_NotifyIcon(NIM_DELETE, &m_nid);
	delete m_pSysIcon;
}

void CTrayIcon::OnActivate()
{
	emit sigShowFrame();
}

void CTrayIcon::StarListen()
{
	//RECT rect;
	//HWND hWnd = FindTrayWnd();
	//if (hWnd != NULL)
	//{
	//	if (!EnumNotifyWindow(rect, hWnd, (HWND)m_pSysIcon->winId()))//如果没在普通托盘区
	//	{
	//		hWnd = FindNotifyIconOverflowWindow();//在溢出区（win7）
	//		if (hWnd != NULL)
	//		{
	//			EnumNotifyWindow(rect, hWnd, (HWND)m_pSysIcon->winId());
	//		}
	//	}
	//}

	//m_IconRect = QRect(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
	m_pLeaveTimer->stop();
	m_pLeaveTimer->start(100);
}

void CTrayIcon::CheckMouse()
{
	QPoint pos = QCursor::pos();
	if (pos != m_lastPos)
	{
		m_pLeaveTimer->stop();
		firstHover = true;
		emit sigHoverLeave();
	}
}

bool CTrayIcon::inPos(QPoint pt)
{
	bool inPos = false;

	RECT rect;
	HWND hWnd = FindTrayWnd();
	if (hWnd != NULL)
	{
		if (!EnumNotifyWindow(rect, hWnd, (HWND)m_pSysIcon->winId()))//如果没在普通托盘区
		{
			hWnd = FindNotifyIconOverflowWindow();//在溢出区（win7）
			if (hWnd != NULL)
			{
				EnumNotifyWindow(rect, hWnd, (HWND)m_pSysIcon->winId());
			}
		}
	}

	if (pt.x() >= rect.left &&pt.x() <= rect.right &&pt.y() >= rect.top && pt.y() <= rect.bottom)
		inPos = true;

	return inPos;
}

void CTrayIcon::onCreateIcon()
{
	m_nid.cbSize = sizeof m_nid;
	m_nid.hIcon = qt_pixmapToWinHICON(m_icoDefault.pixmap(16, 16));
	m_nid.hWnd = HWND(m_pSysIcon->winId());
	m_nid.uCallbackMessage = WM_TRAYNOTIFY;
	m_nid.uID = 1;
	m_nid.uFlags = NIF_ICON | NIF_MESSAGE;
	bool a = Shell_NotifyIcon(NIM_ADD, &m_nid);
	if (!a)
	{
		m_createIconTimer.setSingleShot(true);
		m_createIconTimer.start(1000);
	}

	createTime = clock();
}

bool CTrayIcon::nativeEventFilter(const QByteArray & eventType, void * message, long * result)
{
	if (eventType == "windows_generic_MSG" || eventType == "windows_dispatcher_MSG")
	{
		MSG * pMsg = reinterpret_cast<MSG *>(message);

		if (!showMenu && (clock() - createTime < 200))
			return false;

		showMenu = true;

		if (pMsg->message == WM_TRAYNOTIFY)
		{
			if (pMsg->lParam == WM_MOUSEMOVE)
			{
				if (firstHover)
				{
					firstHover = false;
					lastHoverTime = clock();
					return true;
				}
				else
				{
					if (clock() - lastHoverTime >= 100)
					{
						firstHover = true;
						return true;
					}
					else
					{
						QPoint pt = QCursor::pos();
						m_lastPos = pt;
						StarListen();
						emit sigHoverEnter(pt.x(), pt.y());
						return true;
					}

				}
			}
			else if (pMsg->lParam == WM_LBUTTONDBLCLK)
			{
				OnActivate();
				return true;
			}
			else if (pMsg->lParam == WM_RBUTTONDOWN)
			{
				QPoint pt = QCursor::pos();
				pt.setY(pt.y() - 60);
				m_menu->move(pt);
				m_menu->show();
				return true;
			}
		}
	}

	return false;
}
