#pragma once
#include <QObject>
#include <QVariant>
#include <QTranslator>
#include <QThread>
//#include <QQmlApplicationEngine>
#include <QWindow>
#include <QTimer>

#include "stdafx.h"
#include "AtaSmart.h"
#include "AttrModel.h"
#include "CTrayIcon.h"

#include <QLocalSocket>

#define _AUTOSTARTUP_ "auto"
#define _EPM_CONSOLE "EPM"

struct ListInfo
{
	int iconId;
	DWORD attrId;
	QString attrName;
	DWORD curValue;
	QString threshold;
	QString raw;
};

struct CPUTime
{
	FILETIME idle;
	FILETIME kernel;
	FILETIME user;
};

class DiskInfoMgr;

class RefreshThread : public QThread
{
public:
	RefreshThread(DiskInfoMgr *parent);
protected:
	virtual void run() override;
	DiskInfoMgr *m_mgr = nullptr;
};

class CDiskHealthMainWin;
class DiskInfoMgr :public QObject, public QAbstractNativeEventFilter
{
	Q_OBJECT

	Q_PROPERTY(QVariantList  diskInfos READ getDiskInfos NOTIFY sigDiskInfosChanged)
	Q_PROPERTY(QString  currentLanguage READ getCurrentLanguage WRITE setCurrrentLanguage NOTIFY sigLanguageChanged)
	Q_PROPERTY(bool isTempF READ isTempF WRITE setIsTempF NOTIFY sigIsTempFchanged)
	friend class AttrModel;
public:
	DiskInfoMgr();
	~DiskInfoMgr();
	Q_INVOKABLE void loadInfo();

	Q_INVOKABLE QVariantList getDiskInfos();
	Q_INVOKABLE QVariantMap getDiskStatus(int index);
	Q_INVOKABLE QVariantList getDiskStatusInfo(int index);
	Q_INVOKABLE QVariantList getCautionDiskStatusInfo();
	Q_INVOKABLE void loadInfoList(int index);
	Q_INVOKABLE QAbstractItemModel *getModel() { return &m_model; }
	Q_INVOKABLE QString getCurrentLanguage();
	Q_INVOKABLE void setCurrrentLanguage(QString lang);

	Q_INVOKABLE bool isAutoStart();
	Q_INVOKABLE void delAutoStart();
	Q_INVOKABLE void addAutoStart();
	Q_INVOKABLE void refreshDisks();

	Q_INVOKABLE void setWindowIcon(QWindow *w);
	Q_INVOKABLE bool showWindow();
	Q_INVOKABLE bool doDone(bool check);
	Q_INVOKABLE bool isTrayExist();
	Q_INVOKABLE void doExit();
	Q_INVOKABLE QPoint mousePos();
	Q_INVOKABLE bool isBusy();
	Q_INVOKABLE void quit();
	Q_INVOKABLE bool popCondition();
	Q_INVOKABLE QPoint screenSize();
	Q_INVOKABLE int diskIndexArg();
	Q_INVOKABLE void refreshForPop();

	Q_INVOKABLE bool showRaw(int index);
	Q_INVOKABLE void showFrame();
	Q_INVOKABLE void hoverLeave();

	Q_INVOKABLE void openUrl();
	Q_INVOKABLE void showFromCaution();
	Q_INVOKABLE void clickRefresh();
	Q_INVOKABLE void selectDisk(int index);

	Q_INVOKABLE void setVisible(bool v);

	Q_INVOKABLE int firstCautionIndex() { return m_iFirstCautionDisk; }

	Q_INVOKABLE bool isTempF();
	Q_INVOKABLE void setIsTempF(bool v);

	Q_INVOKABLE bool fileExists(QString file);
	Q_INVOKABLE bool isMainland();
	Q_INVOKABLE QString theme();
	
	Q_INVOKABLE QString getPopLink(QString lang);

	Q_INVOKABLE bool showWindowShadow();

	bool checkHasSmartError(int index);

	void setMaminWin(CDiskHealthMainWin* w) { m_pMainWin = w; }
	//void setEngine(QQmlApplicationEngine *engine) { m_engine = engine; }
	QString getFeature(int index);
	QString GetAttributeDesc(CAtaSmart::ATA_SMART_INFO* pInfo, ULONG dwId);
	bool ProcessSectorAttr(ListInfo &info, CAtaSmart::ATA_SMART_INFO* pInfo, int j);

	bool ProcessTempAttr(ListInfo &info, CAtaSmart::ATA_SMART_INFO* pInfo, int j);
	bool ProcessWDCScanDisk(ListInfo &info, CAtaSmart::ATA_SMART_INFO* pInfo, int j);
	bool ProcessLifeAttr(ListInfo &info, CAtaSmart::ATA_SMART_INFO* pInfo, int j);
	bool ProcessScanErrRate(ListInfo &info, CAtaSmart::ATA_SMART_INFO* pInfo, int j);
	bool ProcessLess0DAttr(ListInfo &info, CAtaSmart::ATA_SMART_INFO* pInfo, int j);

	void getCPUTime();
	void initMultiLanguage();

	void onReadEPMMsg();
	void sendMsgToEPM();
	void resetDiskList();

protected:
	virtual bool nativeEventFilter(const QByteArray & eventType, void * message, long * result)override;
	void parseArgs();
	void doHoverEnter(int x, int y);

signals:
	void sigDiskInfosChanged();
	void sigLanguageChanged();
	void sigCheckForPop();
	void sigSwitchDiskIndex();

	void sigExit();
	void sigShowFrame();
	void sigHoverEnter(int x, int y);
	void sigHoverLeave();
	void sigAboutRefresh();
	void sigIsTempFchanged();
protected:
	CPUTime m_prevCPU = { 0 };
	CAtaSmart	m_Ata;
	QVariantList m_listDiskInfos;
	QStringList m_listStatusDes;
	QStringList m_listTempDes;
	QList<ListInfo> m_listRightInfo;
	QMap<QString, QMap<QString, QString>> m_attrNames;
	AttrModel m_model;
	QString m_strCurLang;
	QTranslator m_translator;
	CDiskHealthMainWin* m_pMainWin = nullptr;
	//QQmlApplicationEngine *m_engine = nullptr;
	RefreshThread m_refreshThread;
	CTrayIcon *m_tray = nullptr;
	QWindow *m_pW = nullptr;
	QTimer m_pCPUTimer;
	QTimer m_EPMTimer;
	QLocalSocket m_socket;
	QMap<int, int> m_mapStatus;
	QString m_strTheme;
	int m_iCPU = 0;
	int m_iFirstCautionDisk = -1;
	int m_iDiskIndex = -1;
	bool m_bAutoStart = false;
	bool m_bEPMStart = false;
	bool m_bIsFullScreen = false;
	bool m_bRefreshForPop = false;
	bool m_bIsEPMDiscoery = false;
	bool m_bIsEPMPE = false;
	bool m_bVisible = false;
	bool m_bIsTempF = false;
	bool m_bNoEnter = true;

	
};
