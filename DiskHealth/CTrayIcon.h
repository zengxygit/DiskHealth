#pragma once

#include <QObject>
#include <QAbstractNativeEventFilter>
#include <windows.h>
#include <QIcon>
#include <QTimer>

class QMenu;
class QAction;
class QLabel;

class CTrayIcon :public QObject,public QAbstractNativeEventFilter
{
	Q_OBJECT
signals:
	void sigExit();
	void sigShowFrame();
	void sigHoverEnter(int x, int y);
	void sigHoverLeave();

public:
	CTrayIcon(QObject *parent = nullptr);
	~CTrayIcon();
	void OnActivate();
	void StarListen();
	void CheckMouse();
	bool inPos(QPoint pt);
	void onCreateIcon();
protected:
	virtual bool nativeEventFilter(const QByteArray & eventType, void * message, long * result)override;
private:
	QIcon m_icoDefault;
	QTimer *m_pLeaveTimer = nullptr;
	QRect m_IconRect;
	QWidget *m_pSysIcon = nullptr;
	QMenu *m_menu = nullptr;
	QList<QAction *> m_actions;
	QTimer m_createIconTimer;
	QPoint m_lastPos;
	clock_t createTime;
	bool showMenu = false;
	bool firstHover = true;
	clock_t lastHoverTime = 0;
};

