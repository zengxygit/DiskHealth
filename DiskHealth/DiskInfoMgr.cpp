#include "DiskInfoMgr.h"

//#include <QQmlApplicationEngine>
#include <QGuiApplication>
#include <QSettings>
#include <QDir>
#include <QTimer>
#include <QProcess>
#include <QApplication>
#include <QDesktopWidget>

#include <QJsonDocument>
#include <QJsonObject>

#include <WtsApi32.h>
#pragma comment(lib, "Wtsapi32.lib")

#include <vector>
#include "../../ShareLib/AliyunLog-2017/include/AliyunWrap.h"

#include "ui/DiskHealthMainWin.h"

using namespace std;

#define OEM_NAME_XAGON   "Xagon"
#define OEM_NAME_MAINLAND	"MainLand"  //大陆版
#define DH_DATA_HEADER "DHINFOS:"

static QString g_product = "DiskHealth";

extern QMap<QString, QMap<QString, QString>> loadDiskAttrs();

#define WM_FULLSCREEN 10087

#define ALILOG_KEY_WINDOW		L"Window"
#define ALILOG_KEY_SOURCE		L"Source"
#define ALILOG_KEY_ACTIVITY     L"Activity"
#define ALILOG_KEY_ATTRIBUTE  L"Attribute"
#define ALILOG_KEY_VID  L"VID"

bool inEPM = false;

void fnSendUserInfo(QString w, QString a, QString attr = QString(), QString src = QString(), QString vid = QString())
{
	if (inEPM)
		return;

	vector<pair<wstring, wstring>> vectInfo;
	vectInfo.push_back(make_pair<wstring, wstring>(ALILOG_KEY_WINDOW, (const wchar_t*)w.utf16()));
	if (!src.isEmpty())
	{
		vectInfo.push_back(make_pair<wstring, wstring>(ALILOG_KEY_SOURCE, (const wchar_t*)src.utf16()));
	}
	vectInfo.push_back(make_pair<wstring, wstring>(ALILOG_KEY_ACTIVITY, (const wchar_t*)a.utf16()));
	if (!attr.isEmpty())
	{
		QJsonDocument jsonDocument = QJsonDocument::fromJson(attr.toUtf8().data());
		if (jsonDocument.isNull())
		{//not json format
			QJsonObject jsonInfo;
			jsonInfo.insert("attribute", attr);
			QJsonDocument jsonDocument(jsonInfo);
			QByteArray baAttributeInfo = jsonDocument.toJson(QJsonDocument::Compact);
			attr = QString(baAttributeInfo);
		}

		vectInfo.push_back(make_pair<wstring, wstring>(ALILOG_KEY_ATTRIBUTE, (const wchar_t*)attr.utf16()));
	}
	if (!vid.isEmpty())
	{
		vectInfo.push_back(make_pair<wstring, wstring>(ALILOG_KEY_VID, (const wchar_t*)vid.utf16()));
	}

	switch (vectInfo.size())
	{
	case 2:
		SEND_USER_INFO(vectInfo[0].first.c_str(), vectInfo[0].second.c_str(), vectInfo[1].first.c_str(), vectInfo[1].second.c_str());
		break;
	case 3:
		SEND_USER_INFO(vectInfo[0].first.c_str(), vectInfo[0].second.c_str(), vectInfo[1].first.c_str(), vectInfo[1].second.c_str(), vectInfo[2].first.c_str(), vectInfo[2].second.c_str());
		break;
	case 4:
		SEND_USER_INFO(vectInfo[0].first.c_str(), vectInfo[0].second.c_str(), vectInfo[1].first.c_str(), vectInfo[1].second.c_str(), vectInfo[2].first.c_str(), vectInfo[2].second.c_str(), vectInfo[3].first.c_str(), vectInfo[3].second.c_str());
		break;
	case 5:
		SEND_USER_INFO(vectInfo[0].first.c_str(), vectInfo[0].second.c_str(), vectInfo[1].first.c_str(), vectInfo[1].second.c_str(), vectInfo[2].first.c_str(), vectInfo[2].second.c_str(), vectInfo[3].first.c_str(), vectInfo[3].second.c_str(), vectInfo[4].first.c_str(), vectInfo[4].second.c_str());
		break;
	default:
		break;
	}
}

BOOL IsWinPESystem()
{
	QVariant value;

	char szSystemDevice[MAX_PATH] = { 0 };
	if (!GetSystemDirectoryA(szSystemDevice, MAX_PATH))
	{
		return FALSE;
	}

	//PE下系统盘符必然是X
	if (szSystemDevice[0] != 'X' && szSystemDevice[0] != 'x')
	{
		return FALSE;
	}

	char szVol[10] = { 0 };
	sprintf(szVol, "\\\\.\\%c:", szSystemDevice[0]);

	HANDLE hVol = CreateFileA(szVol, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hVol == INVALID_HANDLE_VALUE || hVol == NULL)
	{
		return FALSE;
	}

	DWORD nBufferSize = sizeof(VOLUME_DISK_EXTENTS) + sizeof(DISK_EXTENT) * 10;

	unsigned char *pBuffer = new UINT8[nBufferSize];

	DWORD nBytesRet = 0;

	//根据卷对应的磁盘号来判断，如果在PE下，磁盘号不存在，为0xFFFFFFFF（-1）
	BOOL bRet = DeviceIoControl(hVol, IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS,
		NULL, 0, pBuffer, nBufferSize, &nBytesRet, NULL);

	if (!bRet)
	{
		return FALSE;
	}

	BOOL bPESystem = FALSE;

	PVOLUME_DISK_EXTENTS volExtents = (PVOLUME_DISK_EXTENTS)pBuffer;
	if (volExtents->Extents[0].DiskNumber == -1)
	{
		bPESystem = TRUE;
	}
	else
	{
		bPESystem = FALSE;
	}

	delete[] pBuffer;

	CloseHandle(hVol);

	return bPESystem;
}

DiskInfoMgr::DiskInfoMgr():m_model(this), m_refreshThread(this)
{
	QString dll = qApp->applicationDirPath() + "/AliyunWrap.dll";
	UserInfo::PrepareDllFromFullPath((TCHAR *)dll.toStdWString().c_str());

	connect(&m_refreshThread, &QThread::finished, this, [this]()
	{
		if (!m_bVisible && !m_EPMTimer.isActive())
		{
			m_EPMTimer.setSingleShot(true);
			m_EPMTimer.start(60 * 1000);
		}
		m_model.reset();
		emit DiskInfoMgr::sigDiskInfosChanged();
	});

	{
		QSettings reg(QString("HKEY_LOCAL_MACHINE/SOFTWARE/EaseUS/%1").arg("DiskHealth").replace("/", "\\"), QSettings::NativeFormat);
		m_bIsTempF = reg.value("TempF").toInt();
	}

	QSettings reg(QString("HKEY_LOCAL_MACHINE/SOFTWARE/EaseUS/%1").arg(g_product).replace("/", "\\"), QSettings::NativeFormat);
	int showIcon = reg.value("ShowIcon", 0).toInt();

	QStringList args = qApp->arguments();

	parseArgs();

	QString temp = QString("{\"start_from\":\"%1\"}");

	if (m_bIsEPMDiscoery)
	{
		if (IsWinPESystem())
			temp = temp.arg("epm-pe");
		else
			temp = temp.arg("epm");
	}
	else
	{
		if (IsWinPESystem())
			temp = temp.arg("epm-pe-desktop");
		else
			temp = temp.arg("none");
	}

	fnSendUserInfo("DiskHealth", "Info_Start", temp);

	if ((showIcon && !m_bEPMStart) || m_bAutoStart)
	{
		m_tray = new CTrayIcon(this);
		fnSendUserInfo("DiskHealth", "Result_Startup_BGS", QString("{\"result\":true}"));
		connect(m_tray, &CTrayIcon::sigHoverEnter, this, &DiskInfoMgr::doHoverEnter);
		connect(m_tray, &CTrayIcon::sigHoverLeave, this, &DiskInfoMgr::hoverLeave);
		connect(m_tray, &CTrayIcon::sigShowFrame, this, &DiskInfoMgr::showFrame);
		connect(m_tray, &CTrayIcon::sigExit, this, &DiskInfoMgr::sigExit);
	}

	connect(&m_EPMTimer, &QTimer::timeout, this, &DiskInfoMgr::refreshDisks);

	if (m_bEPMStart && args.size() >= 3)
	{
		connect(&m_socket, &QLocalSocket::disconnected, this, &DiskInfoMgr::doExit);
		connect(&m_socket, &QLocalSocket::readyRead, this, &DiskInfoMgr::onReadEPMMsg);
		m_socket.connectToServer(args[2]);
		m_socket.waitForConnected(5000);

		refreshDisks();
	}

	getCPUTime();
	qApp->installNativeEventFilter(this);
}

DiskInfoMgr::~DiskInfoMgr()
{
	
}

void DiskInfoMgr::loadInfo()
{
	m_listDiskInfos.clear();
	//控制对不同类型USB/RAID磁盘的支持
	m_Ata.FlagUsbSat = 1;			//启用对 “USB - SATA 桥接设备” 的支持（常见于 USB 移动硬盘）
	m_Ata.FlagUsbIodata = 1;
	m_Ata.FlagUsbSunplus = 1;
	m_Ata.FlagUsbLogitec = 1;
	m_Ata.FlagUsbProlific = 1;
	m_Ata.FlagUsbJmicron = 1;
	m_Ata.FlagUsbCypress = 1;
	m_Ata.FlagUsbASM1352R = 1;
	m_Ata.FlagUsbMemory = 0;		//禁用对 “USB 闪存盘” 的支持（仅关注硬盘 / SSD）。
	m_Ata.FlagUsbNVMeJMicron = 1;	//启用对 “JMicron 芯片 USB-NVMe 设备” 的支持；
	m_Ata.FlagUsbNVMeASMedia = 1;
	m_Ata.FlagUsbNVMeRealtek = 1;
	m_Ata.FlagMegaRAID = 1;			//启用对 “MegaRAID 阵列卡” 下物理磁盘的支持；

	//初始化 CAtaSmart 并枚举磁盘
	//枚举系统中所有物理磁盘；不忽略 USB 设备（支持 USB 移动硬盘）；默认枚举；同时枚举 SATA/SSD/HDD/NVMe；启用 SMART 数据读取；启用日志（便于调试）
	m_Ata.Init(TRUE, FALSE, NULL, FALSE, TRUE, FALSE);
	DWORD errorCount = 0;
	//m_Ata.vars里元素：一块磁盘的健康信息
	int temp = m_Ata.vars.GetCount();
	for (int i = 0; i < m_Ata.vars.GetCount(); i++)
	{
		// 统计“通电时间原始值>0”的磁盘（可能用于判断磁盘是否已启用）
		if (m_Ata.vars[i].PowerOnRawValue > 0)
		{
			errorCount++;
		}

		// 按“是否为SSD”设置温度告警阈值（SSD耐热性差，阈值更低）
		if (m_Ata.vars[i].IsSsd)
		{
			m_Ata.vars[i].AlarmTemperature = 60;
		}
		else
		{
			m_Ata.vars[i].AlarmTemperature = 50;
		}
		// 启用健康状态告警（1=启用，0=禁用）
		m_Ata.vars[i].AlarmHealthStatus = 1;

		m_Ata.vars[i].Threshold05 = 1; // 05h属性（重映射扇区）：阈值1（超过则告警）
		m_Ata.vars[i].ThresholdC5 = 1; // C5h属性（待映射扇区）：阈值1
		m_Ata.vars[i].ThresholdC6 = 1; // C6h属性（无法校正错误）：阈值1
		m_Ata.vars[i].ThresholdFF = 10; // FFh属性（厂商自定义，如NAND寿命）：阈值10
	}

	for (int i = 0; i < m_Ata.vars.GetCount(); i++)
	{
		m_Ata.UpdateSmartInfo(i);
	}

	resetDiskList();

	static int send = 0;

	for (int i = 0; i < m_Ata.vars.GetCount(); i++)
	{
		CAtaSmart::ATA_SMART_INFO* pSmartInfo = &m_Ata.vars[i];
		QString strSmartInfo = "{";

		vector<char> buffer(1024*1024*2);

		for (int j = 0; j < pSmartInfo->AttributeCount; j++)
		{
			QString des = GetAttributeDesc(pSmartInfo, pSmartInfo->Attribute[j].Id);

			if (j == 0)
				sprintf(buffer.data(), "\"%s/%d\":\"%d/%d\"", des.toStdString().c_str(), pSmartInfo->Attribute[j].Id, pSmartInfo->Attribute[j].CurrentValue, pSmartInfo->Threshold[j].ThresholdValue);
			else
				sprintf(buffer.data(), ",\"%s/%d\":\"%d/%d\"", des.toStdString().c_str(), pSmartInfo->Attribute[j].Id, pSmartInfo->Attribute[j].CurrentValue, pSmartInfo->Threshold[j].ThresholdValue);

			strSmartInfo += buffer.data();
		}


		strSmartInfo += "}";
		
		QString status = "Unknown";
		QStringList list;
		list << "Unknown" << "Good"<< "Caution" << "Bad";

		if (pSmartInfo->DiskStatus >= 0 && pSmartInfo->DiskStatus < list.size())
			status = list[pSmartInfo->DiskStatus];
		
		QString model = QString::fromWCharArray(pSmartInfo->Model.GetString());

		QString lifeValue = pSmartInfo->Life == -1 ? "*" : (QString::number(pSmartInfo->Life) + "%");

		sprintf(buffer.data(), "{\"disk\":\"%d\",\"disk_model\":\"%s\",\"status\":\"%s/%s\",\"temprature\":\"%d\",\"smart_indicators\":%s}",
			pSmartInfo->PhysicalDriveId, model.toStdString().c_str(),status.toStdString().c_str(), lifeValue.toStdString().c_str(),pSmartInfo->Temperature, strSmartInfo.toStdString().c_str());

		QString info = buffer.data();

		if (!send)
		{
			fnSendUserInfo("DiskHealth", "Result_LoadDiskInfo", info);
		}
	}

	emit sigDiskInfosChanged();

	if (m_bRefreshForPop)
	{
		m_bRefreshForPop = false;

		auto v = getCautionDiskStatusInfo();
		if (v.size() && !send)
			fnSendUserInfo("DiskHealth", "Info_ShowNotification", QString("{\"Disk\":\"disk %1\"}").arg(v[0].toMap()["diskIndex"].toInt()));

		emit sigCheckForPop();
	}

	if (!send)
	{
		send = 1;
	}

	if (m_bEPMStart)
	{
		sendMsgToEPM();
	}
}

Q_INVOKABLE QVariantList DiskInfoMgr::getDiskInfos()
{
	return m_listDiskInfos;
}

Q_INVOKABLE QVariantMap DiskInfoMgr::getDiskStatus(int index)
{
	if (index >= 0 && index < m_Ata.vars.GetCount())
	{
		auto &info = m_Ata.vars[index];
		ULONG nIndex = (ULONG)info.DiskStatus;
		if (nIndex > 4)
			nIndex = 0;

		QVariantMap vm;
		vm["statusIndex"] = (int)nIndex;
		vm["statusInfo"] = m_listStatusDes[nIndex];

		vm["diskIndex"] = QString::number(m_Ata.vars[index].PhysicalDriveId);
		vm["diskDes"] =  QString::fromWCharArray(m_Ata.vars[index].Model.GetString());

		nIndex = 1;
		if (info.Temperature > 70)
			nIndex = 2;
		if (info.Temperature < -100 || info.Temperature >= 200)
			nIndex = 0;

		vm["tempIndex"] = (int)nIndex;
		vm["tempDesc"] = m_listTempDes[nIndex];

		//isTempF
		double ft = (double)info.Temperature * 1.8 + 32;
		QString tempF = QString::number((int)ft);

		if (isTempF())
		{
			vm["tempValue"] = nIndex == 0 ? QObject::tr("- -") : tempF + QString::fromWCharArray(L"°F");
		}
		else
		{
			vm["tempValue"] = nIndex == 0 ? QObject::tr("- -") : QString::number(info.Temperature) + QString::fromWCharArray(L"°C");
		}
		
		QString strLife;
		if (info.Life != -1)
		{
			strLife = QString::number(info.Life) + "%";
		}
		vm["life"] = strLife;

		return vm;
	}

	return QVariantMap();
}

static void addResult(QVariantList &result, QString key, QString value)
{
	QVariantMap arg;
	arg["arg"] = key;
	arg["value"] = value;
	result.append(arg);
}

Q_INVOKABLE QVariantList DiskInfoMgr::getDiskStatusInfo(int index)
{
	resetDiskList();

	if (index >= 0 && index < m_Ata.vars.GetCount())
	{
		QVariantList result;
		CAtaSmart::ATA_SMART_INFO* pSmartInfo = &m_Ata.vars[index];
		const INT powerOnHours = pSmartInfo->MeasuredPowerOnHours > 0 ? pSmartInfo->MeasuredPowerOnHours : pSmartInfo->DetectedPowerOnHours;

		addResult(result, QObject::tr("Power On Hours"), QString::number(powerOnHours)); //+" " + QObject::tr("hours"));
		addResult(result, QObject::tr("Power On Count"), QString::number(pSmartInfo->PowerOnCount));// +" " + QObject::tr("count"));

		if (pSmartInfo->NominalMediaRotationRate > 1)
			addResult(result, QObject::tr("Rotation Rate"), QString::number(pSmartInfo->NominalMediaRotationRate) + " " + QObject::tr("RPM"));
		else
			addResult(result, QObject::tr("Rotation Rate"), "--");

		if (pSmartInfo->HostWrites > 0)
			addResult(result, QObject::tr("Total Host Writes"), QString("%1 GB").arg(pSmartInfo->HostWrites));
		else if (pSmartInfo->NvCacheSize > 0)
			addResult(result, QObject::tr("Total Host Writes"), QString("%1 MB").arg(pSmartInfo->NvCacheSize / 1024 / 1024));
		else
			addResult(result, QObject::tr("Total Host Writes"), "--");

		if (pSmartInfo->HostReads >= 0)
			addResult(result, QObject::tr("Total Host Read"), QString("%1 GB").arg(pSmartInfo->HostReads));
		else if (pSmartInfo->BufferSize > 0)
			addResult(result, QObject::tr("Total Host Read"), QString("%1 KB").arg(pSmartInfo->NvCacheSize / 1024));
		else
			addResult(result, QObject::tr("Total Host Read"), "--");

		//addResult(result, QObject::tr("Firmware"), QString::fromWCharArray(pSmartInfo->FirmwareRev.GetString()));
		addResult(result, QObject::tr("Serial Number"), QString::fromWCharArray(pSmartInfo->SerialNumber.GetString()));
		//addResult(result, QObject::tr("Interface"), QString::fromWCharArray(pSmartInfo->Interface.GetString()));

		if (pSmartInfo->CurrentTransferMode.IsEmpty() && pSmartInfo->MaxTransferMode)
			addResult(result, QObject::tr("Transfer Mode"), "--");
		else
			addResult(result, QObject::tr("Transfer Mode"), 
				QString::fromWCharArray(pSmartInfo->CurrentTransferMode.GetString()) + QString(" | ") + QString::fromWCharArray(pSmartInfo->MaxTransferMode.GetString()));

		//if (!pSmartInfo->MinorVersion.IsEmpty())
		//{
		//	addResult(result, QObject::tr("Standard"), 
		//		QString::fromWCharArray(pSmartInfo->MajorVersion.GetString()) + QString(" | ") + QString::fromWCharArray(pSmartInfo->MinorVersion.GetString()));
		//}
		//else
		//{
		//	addResult(result, QObject::tr("Standard"),
		//		QString::fromWCharArray(pSmartInfo->MajorVersion.GetString()));
		//}
		//addResult(result, QObject::tr("Features"),
		//getFeature(index));
	
		return result;
	}

	return QVariantList();
}

Q_INVOKABLE QVariantList DiskInfoMgr::getCautionDiskStatusInfo()
{
	int size = m_listDiskInfos.size();
	m_iFirstCautionDisk = -1;
	QVariantList l;

	for (int i = 0; i < size; ++i)
	{
		QVariantMap m = getDiskStatus(i);

		if (m["statusIndex"].toInt() > 1)
		{
			l.append(m);
			if (m_iFirstCautionDisk == -1)
				m_iFirstCautionDisk = i;
		}
	}

	return l;
}

Q_INVOKABLE void DiskInfoMgr::loadInfoList(int index)
{
	m_listRightInfo.clear();

	if (index >= 0 && index < m_Ata.vars.GetCount())
	{
		CAtaSmart::ATA_SMART_INFO* pSmartInfo = &m_Ata.vars[index];
		CAtaSmart::ATA_SMART_INFO* pInfo = pSmartInfo;

		for (int j = 0; j < pSmartInfo->AttributeCount; j++)
		{
			if (pInfo->Attribute[j].Id == 0x00 || pInfo->Attribute[j].Id == 0xFF)
				continue;

			ListInfo info;
			info.attrId = pInfo->Attribute[j].Id;
			info.curValue = pInfo->Attribute[j].CurrentValue;

			{
				char buffer[MAX_PATH];
				sprintf(buffer, "%02X%02X%02X%02X%02X%02X%02X",
					pInfo->Attribute[j].Reserved,
					pInfo->Attribute[j].RawValue[5],
					pInfo->Attribute[j].RawValue[4],
					pInfo->Attribute[j].RawValue[3],
					pInfo->Attribute[j].RawValue[2],
					pInfo->Attribute[j].RawValue[1],
					pInfo->Attribute[j].RawValue[0]);

				info.raw = buffer;
			}

			if (pInfo->IsThresholdCorrect)
				info.threshold = QString::number(pInfo->Threshold[j].ThresholdValue);
			else
				info.threshold = "--------";

			if (pInfo->IsSmartCorrect && pInfo->IsThresholdCorrect && !pInfo->IsThresholdBug)
			{
				do
				{
					info.attrName = GetAttributeDesc(pInfo, pInfo->Attribute[j].Id);

					// sector count info
					if (ProcessSectorAttr(info, pInfo, j))
						break;
					// Temperature
					if (ProcessTempAttr(info, pInfo, j))
						break;
					// Life for WDC/SanDisk
					if (ProcessWDCScanDisk(info, pInfo, j))
						break;
					// Life
					if (ProcessLifeAttr(info, pInfo, j))
						break;
					// Read Error Rate for SandForce Bug
					if (ProcessScanErrRate(info, pInfo, j))
						break;
					// less than 0x0d
					if (ProcessLess0DAttr(info, pInfo, j))
						break;
					// other
					info.iconId = 1;
				} while (false);
			}
			else // unknown
			{
				info.iconId = 0;
				info.attrName = QObject::tr("Unknown");
			}

			m_listRightInfo.append(info);
		}
	}

	m_model.reset();

	return Q_INVOKABLE void();
}

Q_INVOKABLE QString DiskInfoMgr::getCurrentLanguage()
{
	return m_strCurLang;
}

Q_INVOKABLE void DiskInfoMgr::setCurrrentLanguage(QString lang)
{
	do
	{
		QSettings reg(QString("HKEY_LOCAL_MACHINE/SOFTWARE/EaseUS/%1").arg(g_product).replace("/", "\\"), QSettings::NativeFormat);
		reg.setValue("Language", lang);

		if (!m_translator.isEmpty()) {
			qApp->removeTranslator(&m_translator);
		}
		qApp->setLayoutDirection("Arabic" == m_strCurLang ? Qt::RightToLeft : Qt::LeftToRight);

		QString file;
		while (true)
		{
			file = qApp->applicationDirPath() + QString("/translations/%1_").arg(g_product) + lang + ".qm";
			if (QFileInfo::exists(file)) break;

			if ("English" == lang) break;
			lang = "English";
		}

		m_strCurLang = lang;
		bool bLoad = m_translator.load(file);
		bLoad = qApp->installTranslator(&m_translator);
		//m_engine->retranslate();
		m_attrNames = loadDiskAttrs();

		m_listStatusDes.clear();
		m_listTempDes.clear();
		m_listStatusDes << QObject::tr("Unknown") << QObject::tr("Good") << QObject::tr("Caution") << QObject::tr("Bad");
		m_listTempDes << QObject::tr("Unknown") << QObject::tr("Normal") << QObject::tr("Warning");

		emit sigLanguageChanged();

	} while (false);
}

static int doOperation(QString cmd)
{
	QProcess proc;
	proc.start(cmd, QIODevice::ReadWrite);
	int ret = 0;
	proc.waitForStarted();
	proc.waitForFinished();
	proc.close();
	ret = proc.exitCode();
	return ret;
}

#define _TASKNAME_ g_product

Q_INVOKABLE bool DiskInfoMgr::isAutoStart()
{
	QString cmd = QString("\"%1/querytask.bat\" %2").arg(qApp->applicationDirPath()).arg(_TASKNAME_);
	int ret = doOperation(cmd);
	return ret == 0;
}

Q_INVOKABLE void DiskInfoMgr::delAutoStart()
{
	QString cmd = QString("\"%1/deltask.bat\" %2").arg(qApp->applicationDirPath()).arg(_TASKNAME_);
	doOperation(cmd);
	QSettings reg(QString("HKEY_LOCAL_MACHINE/SOFTWARE/EaseUS/%1").arg(g_product).replace("/", "\\"), QSettings::NativeFormat);
	reg.setValue("ShowIcon", 0);
}

Q_INVOKABLE void DiskInfoMgr::addAutoStart()
{
	QString route = qApp->applicationDirPath() + "/DiskHealthAuto.exe";
	route = route.replace("/", "\\");

	QString cmd = QString("\"%1/addtask.bat\" %2 \"\'%3\'\"").arg(qApp->applicationDirPath()).arg(_TASKNAME_).arg(route);
	doOperation(cmd);
}

Q_INVOKABLE void DiskInfoMgr::refreshDisks()
{
	emit sigAboutRefresh();
	m_refreshThread.start();
}

Q_INVOKABLE void DiskInfoMgr::setWindowIcon(QWindow * w)
{
	if (w)
	{
		m_pW = w;
		APPBARDATA abd = { 0 };
		abd.cbSize = sizeof(abd);
		abd.uCallbackMessage = WM_FULLSCREEN;
		abd.hWnd = (HWND)m_pW->winId();
		SHAppBarMessage(ABM_NEW, &abd);

		WTSRegisterSessionNotification(abd.hWnd, NOTIFY_FOR_ALL_SESSIONS);

		w->setIcon(QIcon(QString(":/res/disksmart.ico")));
		connect(&m_pCPUTimer, &QTimer::timeout, this, &DiskInfoMgr::getCPUTime);
		m_pCPUTimer.start(1000);
	}
}

Q_INVOKABLE bool DiskInfoMgr::showWindow()
{
	return !m_bAutoStart && !m_bEPMStart;
}

Q_INVOKABLE bool DiskInfoMgr::doDone(bool check)
{
	bool result = false;

	fnSendUserInfo("DiskHealth", "Click_Done",QString("{\"enable_bgs\":%1}").arg(check?"true":"false"));

	if (check)
	{
		if (m_tray == nullptr)
		{
			m_tray = new CTrayIcon(this);
			fnSendUserInfo("DiskHealth", "Result_Startup_BGS", QString("{\"result\":true}"));
			connect(m_tray, &CTrayIcon::sigHoverEnter, this, &DiskInfoMgr::sigHoverEnter);
			connect(m_tray, &CTrayIcon::sigHoverLeave, this, &DiskInfoMgr::sigHoverLeave);
			connect(m_tray, &CTrayIcon::sigShowFrame, this, &DiskInfoMgr::showFrame);
			connect(m_tray, &CTrayIcon::sigExit, this, &DiskInfoMgr::sigExit);
			QSettings reg(QString("HKEY_LOCAL_MACHINE/SOFTWARE/EaseUS/%1").arg(g_product).replace("/", "\\"), QSettings::NativeFormat);
			reg.setValue("ShowIcon", 1);
			addAutoStart();
		}

		result = true;
	}
	else
	{
		if (m_tray != nullptr)
		{
			delete m_tray;
			m_tray = nullptr;
			fnSendUserInfo("DiskHealth", "Result_Stop_BGS", QString("{\"result\":true}"));
			QSettings reg(QString("HKEY_LOCAL_MACHINE/SOFTWARE/EaseUS/%1").arg(g_product).replace("/", "\\"), QSettings::NativeFormat);
			reg.setValue("ShowIcon", 0);
			delAutoStart();
		}
	}
	
	return result;
}

Q_INVOKABLE bool DiskInfoMgr::isTrayExist()
{
	return m_tray != nullptr;
}

Q_INVOKABLE void DiskInfoMgr::doExit()
{
	qDebug() << "disk info mgr quit";
	if (m_tray)
	{
		delete m_tray;
		m_tray = nullptr;
		fnSendUserInfo("DiskHealth", "Result_Stop_BGS", QString("{\"result\":true}"));
	}
	m_refreshThread.wait();
	qDebug() << "disk info mgr quit2";
	qApp->quit();
}

Q_INVOKABLE QPoint DiskInfoMgr::mousePos()
{
	return QCursor::pos();
}

Q_INVOKABLE bool DiskInfoMgr::isBusy()
{
	return m_refreshThread.isRunning();
}

Q_INVOKABLE void DiskInfoMgr::quit()
{
	delAutoStart();
	doExit();
}

Q_INVOKABLE bool DiskInfoMgr::popCondition()
{
	return !m_bIsFullScreen && m_iCPU < 50;
}

Q_INVOKABLE QPoint DiskInfoMgr::screenSize()
{
	RECT r;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &r, 0);
	return QPoint(r.right, r.bottom);
}

Q_INVOKABLE int DiskInfoMgr::diskIndexArg()
{
	int ret = -1;

	if (m_iDiskIndex != -1)
	{
		int idx = m_iDiskIndex;
		for (int i = 0; i < m_Ata.vars.GetCount(); ++i)
		{
			if (m_Ata.vars[i].PhysicalDriveId == idx)
			{
				ret = i;
				break;
			}
		}

		m_iDiskIndex = -1;
	}

	return ret;
}

Q_INVOKABLE void DiskInfoMgr::refreshForPop()
{
	if (!m_bEPMStart)
	{
		m_bRefreshForPop = true;
		refreshDisks();
	}
}

Q_INVOKABLE bool DiskInfoMgr::showRaw(int index)
{
	bool result = true;

	if (index >= 0 && index < m_Ata.vars.GetCount())
	{
		auto &info = m_Ata.vars[index];

			for (int i = 0; i < info.AttributeCount; i++)
			{
				if (info.Attribute[i].CurrentValue != 0)
				{
					result = false;
					break;
				}
			}
	}

	return result;
}

Q_INVOKABLE void DiskInfoMgr::showFrame()
{
	fnSendUserInfo("DiskHealth", "Click_SysIcon_Menu");
	emit sigShowFrame();
}

Q_INVOKABLE void DiskInfoMgr::hoverLeave()
{
	m_bNoEnter = true;
	emit sigHoverLeave();
}

Q_INVOKABLE void DiskInfoMgr::openUrl()
{
	fnSendUserInfo("DiskHealth", "Click_Open_Browser_By_Notification");
}

Q_INVOKABLE void DiskInfoMgr::showFromCaution()
{
	fnSendUserInfo("DiskHealth", "Click_Open_DiskHealth_By_Notification");
}

Q_INVOKABLE void DiskInfoMgr::clickRefresh()
{
	fnSendUserInfo("DiskHealth", "Click_Refresh");
}

Q_INVOKABLE void DiskInfoMgr::selectDisk(int index)
{
	if (index >= 0 && index < m_Ata.vars.GetCount())
	{
		auto &info = m_Ata.vars[index];
		fnSendUserInfo("DiskHealth", "SelectDisk",QString("{\"disk\":\"%1\"}").arg(info.PhysicalDriveId));
	}
}

Q_INVOKABLE void DiskInfoMgr::setVisible(bool v)
{
	if (v)
	{
		m_EPMTimer.stop();
	}
	else
	{
		if (!m_EPMTimer.isActive())
		{
			m_EPMTimer.setSingleShot(true);
			m_EPMTimer.start(60 * 1000);
		}
	}

	m_bVisible = v;
}

Q_INVOKABLE bool DiskInfoMgr::isTempF()
{
	return m_bIsTempF;
}

Q_INVOKABLE void DiskInfoMgr::setIsTempF(bool v)
{
	QSettings reg(QString("HKEY_LOCAL_MACHINE/SOFTWARE/EaseUS/%1").arg("DiskHealth").replace("/", "\\"), QSettings::NativeFormat);
	m_bIsTempF = v;
	reg.setValue("TempF", (int)m_bIsTempF);
	emit sigIsTempFchanged();
}

Q_INVOKABLE bool DiskInfoMgr::fileExists(QString file)
{
	bool ret = QFile::exists(file.replace("qrc:/", ":/"));
	return ret;
}


static LONG GetOEMNameFromReg(std::string &strOEMName)
{
	LONG lOpenReg = ERROR_INVALID_FUNCTION;
	do
	{
		DWORD BufferSize = MAX_PATH;
		char szOem[MAX_PATH] = { 0 };

		//获取注册表中的OEM信息
		HKEY hKey;
		if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,
			TEXT("Software\\EASEUS\\EPM\\Version"),
			0,
			KEY_READ,
			&hKey) == ERROR_SUCCESS)
		{
			lOpenReg = RegQueryValueExA(hKey,
				"OEM",
				NULL,
				NULL,
				(LPBYTE)szOem,
				&BufferSize);
		}
		strOEMName = szOem;

		RegCloseKey(hKey);
		hKey = NULL;
	} while (0);

	return lOpenReg;
}

static bool IsAssignOEM(const char* pszOem, bool bIsStartWith = false)
{
	if (NULL == pszOem)
	{
		return false;
	}

	std::string strOEM(pszOem);
	std::string strOEMName = "";
	GetOEMNameFromReg(strOEMName);
	if (bIsStartWith)
	{
		return (0 == strOEMName.compare(0, strlen(pszOem), pszOem));
	}
	else
	{
		return (strOEM == strOEMName);
	}
}

Q_INVOKABLE bool DiskInfoMgr::isMainland()
{
	return IsAssignOEM(OEM_NAME_MAINLAND,true) || IsAssignOEM(OEM_NAME_XAGON);
}

Q_INVOKABLE QString DiskInfoMgr::theme()
{
	return m_strTheme.length() ? m_strTheme : "light";
}

Q_INVOKABLE QString DiskInfoMgr::getPopLink(QString lang)
{
	QMap<QString, QString> langToLinks;

	langToLinks.insert("en-US","https://www.easeus.com/partition-master/hard-drive-failure.html");
	langToLinks.insert("ja-JP", "https://jp.easeus.com/partition-manager/hard-drive-failure.html");
	langToLinks.insert("ko-KR", "https://www.easeus.co.kr/partition-manager-software/hard-drive-failure.html");
	langToLinks.insert("zh-TW", "https://tw.easeus.com/partition-manager-tips/hard-drive-failure.html");
	langToLinks.insert("de-DE", "https://www.easeus.de/partitionieren-tipps/festplattenausfall.html");
	langToLinks.insert("pt-BR", "https://br.easeus.com/partition-manager-tips/falha-no-disco-rigido.html");
	langToLinks.insert("it-IT", "https://it.easeus.com/partition-manager-tips/fallimento-di-hard-disk.html");
	langToLinks.insert("es-ES", "https://es.easeus.com/partition-manager-tips/fallo-del-disco-duro.html");
	langToLinks.insert("fr-FR", "https://www.easeus.fr/partition-manager-tips/panne-de-disque-dur.html");

	QString result = langToLinks["en-US"];

	auto it = langToLinks.find(lang);
	if (it != langToLinks.end())
		result = it.value();

	return result;
}

static bool isWindows10Greater()
{
	QStringList list = QSysInfo::kernelVersion().split(".");
	if (list.isEmpty()) return false;
	return (list.at(0).toInt() >= 10) || (list.last().toInt() >= 10240);
}

Q_INVOKABLE bool DiskInfoMgr::showWindowShadow()
{
	if (isWindows10Greater()) {
		bool *msg = nullptr;
		SPI_SETLOWPOWERTIMEOUT;
		BOOL ret = SystemParametersInfo(0x1024, 0, (PVOID)&msg, 0);
		return msg != nullptr;
	}
	return false;
}

bool DiskInfoMgr::checkHasSmartError(int index)
{
	bool result = false;

	if (index >= 0 && index < m_Ata.vars.GetCount())
	{
		CAtaSmart::ATA_SMART_INFO* pSmartInfo = &m_Ata.vars[index];
		CAtaSmart::ATA_SMART_INFO* pInfo = pSmartInfo;

		for (int j = 0; j < pSmartInfo->AttributeCount; j++)
		{
			if (pInfo->Attribute[j].Id == 0x00 || pInfo->Attribute[j].Id == 0xFF)
				continue;

			ListInfo info;
			info.attrId = pInfo->Attribute[j].Id;
			info.curValue = pInfo->Attribute[j].CurrentValue;

			{
				char buffer[MAX_PATH];
				sprintf(buffer, "%02X%02X%02X%02X%02X%02X%02X",
					pInfo->Attribute[j].Reserved,
					pInfo->Attribute[j].RawValue[5],
					pInfo->Attribute[j].RawValue[4],
					pInfo->Attribute[j].RawValue[3],
					pInfo->Attribute[j].RawValue[2],
					pInfo->Attribute[j].RawValue[1],
					pInfo->Attribute[j].RawValue[0]);

				info.raw = buffer;
			}

			if (pInfo->IsThresholdCorrect)
				info.threshold = QString::number(pInfo->Threshold[j].ThresholdValue);
			else
				info.threshold = "--------";

			if (pInfo->IsSmartCorrect && pInfo->IsThresholdCorrect && !pInfo->IsThresholdBug)
			{
				do
				{
					info.attrName = GetAttributeDesc(pInfo, pInfo->Attribute[j].Id);

					// sector count info
					if (ProcessSectorAttr(info, pInfo, j))
						break;
					// Temperature
					if (ProcessTempAttr(info, pInfo, j))
						break;
					// Life for WDC/SanDisk
					if (ProcessWDCScanDisk(info, pInfo, j))
						break;
					// Life
					if (ProcessLifeAttr(info, pInfo, j))
						break;
					// Read Error Rate for SandForce Bug
					if (ProcessScanErrRate(info, pInfo, j))
						break;
					// less than 0x0d
					if (ProcessLess0DAttr(info, pInfo, j))
						break;
					// other
					info.iconId = 1;
				} while (false);
			}
			else // unknown
			{
				info.iconId = 0;
				info.attrName = QObject::tr("Unknown");
			}

			if (info.iconId != 1)
			{
				result = true;
				break;
			}

		}
	}

	return result;
}

QString DiskInfoMgr::getFeature(int index)
{
	if (index >= 0 && index < m_Ata.vars.GetCount())
	{
		CAtaSmart::ATA_SMART_INFO* pSmartInfo = &m_Ata.vars[index];
		int nCount = 0;
		int nRefCount = 4;
		QString result;
		if (pSmartInfo->IsSmartSupported)
		{
			result += QString("S.M.A.R.T., ");
			nCount++;
		}

		if (pSmartInfo->IsApmSupported)
		{
			result += QString("APM, ");
			nCount++;
		}

		if (pSmartInfo->IsAamSupported)
		{
			result += QString("AAM, ");
			nCount++;
			if (nCount >= nRefCount)
			{
				result += QString("\n");
				nCount = 0;
			}
		}

		if (pSmartInfo->IsNcqSupported)
		{
			result += QString("NCQ, ");
			nCount++;
			if (nCount >= nRefCount)
			{
				result += QString("\n");
				nCount = 0;
			}
		}

		if (pSmartInfo->IsTrimSupported)
		{
			result += QString("TRIM, ");
			nCount++;
			if (nCount >= nRefCount)
			{
				result += QString("\n");
				nCount = 0;
			}
		}

		if (pSmartInfo->IsDeviceSleepSupported)
		{
			result += QString("DevSleep, ");
			nCount++;
			if (nCount >= nRefCount)
			{
				result += QString("\n");
				nCount = 0;
			}
		}

		if (pSmartInfo->IsVolatileWriteCachePresent)
		{
			result += QString("VolatileWriteCache, ");
		}

		if (!result.isEmpty())
		{
			if (nCount == 0)
				result = result.left(result.length() - 3);
			else
				result = result.left(result.length() - 2);
		}

		return result;
	}

	return QString();
}

QString DiskInfoMgr::GetAttributeDesc(CAtaSmart::ATA_SMART_INFO* pInfo, ULONG dwId)
{
	QString result = QObject::tr("Unknown");

	auto it = m_attrNames.find(QString::fromWCharArray(pInfo->SmartKeyName.GetString()));
	if (it != m_attrNames.end())
	{
		auto attrs = it.value();
		auto it2 = attrs.find(QString("%1").arg(dwId, 2, 16,QLatin1Char('0')).toUpper());
		if (it2 != attrs.end())
			result = it2.value();
		else
			result = QObject::tr("Vendor Specific");
	}

	return result;
}

bool DiskInfoMgr::ProcessSectorAttr(ListInfo & info, CAtaSmart::ATA_SMART_INFO * pInfo, int j)
{
	bool bRet = false;
	if (!pInfo->IsSsd &&
		(pInfo->Attribute[j].Id == 0x05 // Reallocated Sectors Count
			|| pInfo->Attribute[j].Id == 0xC5 // Current Pending Sector Count
			|| pInfo->Attribute[j].Id == 0xC6 // Off-Line Scan Uncorrectable Sector Count
			))
	{
		if (pInfo->Threshold[j].ThresholdValue != 0
			&& pInfo->Attribute[j].CurrentValue < pInfo->Threshold[j].ThresholdValue)
		{
			info.iconId = 3;
		}
		else
		{
			WORD raw = MAKEWORD(pInfo->Attribute[j].RawValue[0], pInfo->Attribute[j].RawValue[1]);
			WORD threshold;
			switch (pInfo->Attribute[j].Id)
			{
			case 0x05:
				threshold = pInfo->Threshold05;
				break;
			case 0xC5:
				threshold = pInfo->ThresholdC5;
				break;
			case 0xC6:
				threshold = pInfo->ThresholdC6;
				break;
			}
			if (threshold > 0 && raw >= threshold)
			{
				info.iconId = 2;
			}
			else
			{
				info.iconId = 1;
			}
		}
		bRet = true;
	}
	return bRet;
}

bool DiskInfoMgr::ProcessTempAttr(ListInfo & info, CAtaSmart::ATA_SMART_INFO * pInfo, int j)
{
	bool bRet = false;
	if (pInfo->Attribute[j].Id == 0xC2 ||
		pInfo->Attribute[j].Id == 0xB8)
	{
		info.iconId = 1;
		bRet = true;
	}
	return bRet;
}

bool DiskInfoMgr::ProcessWDCScanDisk(ListInfo & info, CAtaSmart::ATA_SMART_INFO * pInfo, int j)
{
	bool bRet = false;
	if (pInfo->Attribute[j].Id == 0xE6 &&
		(pInfo->DiskVendorId == CAtaSmart::SSD_VENDOR_WDC ||
			pInfo->DiskVendorId == CAtaSmart::SSD_VENDOR_SANDISK))
	{
		int life;

		if (pInfo->FlagLifeSanDisk0_1)
		{
			life = 100 - (pInfo->Attribute[j].RawValue[1] * 256 + pInfo->Attribute[j].RawValue[0]) / 100;
		}
		else
		{
			life = 100 - pInfo->Attribute[j].RawValue[1];
		}

		if (life <= 0) { life = 0; }
		if (life == 0)
		{
			info.iconId = 3;
		}
		else if (life <= pInfo->ThresholdFF)
		{
			info.iconId = 2;
		}
		else
		{
			info.iconId = 1;
		}
		bRet = true;
	}
	return bRet;
}

#define CONDITION_LIFT_ATTR(j)		\
		(pInfo->Attribute[j].Id == 0xA9 && (pInfo->DiskVendorId == CAtaSmart::SSD_VENDOR_REALTEK || (pInfo->DiskVendorId == CAtaSmart::SSD_VENDOR_KINGSTON && pInfo->HostReadsWritesUnit == CAtaSmart::HOST_READS_WRITES_32MB)))	\
|| (pInfo->Attribute[j].Id == 0xAD && (pInfo->DiskVendorId == CAtaSmart::SSD_VENDOR_TOSHIBA || pInfo->DiskVendorId == CAtaSmart::SSD_VENDOR_KIOXIA))	\
|| (pInfo->Attribute[j].Id == 0xBB && pInfo->DiskVendorId == CAtaSmart::SSD_VENDOR_MTRON)	\
|| (pInfo->Attribute[j].Id == 0xCA && (pInfo->DiskVendorId == CAtaSmart::SSD_VENDOR_MICRON || pInfo->DiskVendorId == CAtaSmart::SSD_VENDOR_MICRON_MU02 || pInfo->DiskVendorId == CAtaSmart::SSD_VENDOR_INTEL_DC))	\
|| (pInfo->Attribute[j].Id == 0xD1 && pInfo->DiskVendorId == CAtaSmart::SSD_VENDOR_INDILINX)	\
|| (pInfo->Attribute[j].Id == 0xE7 && (pInfo->DiskVendorId == CAtaSmart::SSD_VENDOR_SANDFORCE || pInfo->DiskVendorId == CAtaSmart::SSD_VENDOR_CORSAIR || pInfo->DiskVendorId == CAtaSmart::SSD_VENDOR_KINGSTON || pInfo->DiskVendorId == CAtaSmart::SSD_VENDOR_SKHYNIX || pInfo->DiskVendorId == CAtaSmart::SSD_VENDOR_REALTEK || pInfo->DiskVendorId == CAtaSmart::SSD_VENDOR_SANDISK || pInfo->DiskVendorId == CAtaSmart::SSD_VENDOR_SSSTC || pInfo->DiskVendorId == CAtaSmart::SSD_VENDOR_APACER || pInfo->DiskVendorId == CAtaSmart::SSD_VENDOR_JMICRON || pInfo->DiskVendorId == CAtaSmart::SSD_VENDOR_SEAGATE || pInfo->DiskVendorId == CAtaSmart::SSD_VENDOR_MAXIOTEK))	\
|| (pInfo->Attribute[j].Id == 0xE8 && pInfo->DiskVendorId == CAtaSmart::SSD_VENDOR_PLEXTOR)	\
|| (pInfo->Attribute[j].Id == 0xE9 && (pInfo->DiskVendorId == CAtaSmart::SSD_VENDOR_INTEL || pInfo->DiskVendorId == CAtaSmart::SSD_VENDOR_OCZ || pInfo->DiskVendorId == CAtaSmart::SSD_VENDOR_OCZ_VECTOR || pInfo->DiskVendorId == CAtaSmart::SSD_VENDOR_SKHYNIX))	\
|| (pInfo->Attribute[j].Id == 0xE9 && pInfo->FlagLifeSanDiskLenovo)

bool DiskInfoMgr::ProcessLifeAttr(ListInfo & info, CAtaSmart::ATA_SMART_INFO * pInfo, int j)
{
	bool bRet = false;
	if (
		CONDITION_LIFT_ATTR(j))
	{
		if (pInfo->FlagLifeRawValue)
		{
			info.iconId = 1;
		}
		else if (pInfo->Attribute[j].CurrentValue == 0
			|| pInfo->Attribute[j].CurrentValue < pInfo->Threshold[j].ThresholdValue)
		{
			info.iconId = 3;
		}
		else if (pInfo->Attribute[j].CurrentValue <= pInfo->ThresholdFF)
		{
			info.iconId = 2;
		}
		else
		{
			info.iconId = 1;
		}
		bRet = true;
	}
	return bRet;
}

bool DiskInfoMgr::ProcessScanErrRate(ListInfo & info, CAtaSmart::ATA_SMART_INFO * pInfo, int j)
{
	bool bRet = false;
	if (pInfo->Attribute[j].Id == 0x01
		&& pInfo->DiskVendorId == CAtaSmart::SSD_VENDOR_SANDFORCE)
	{
		if (pInfo->Attribute[j].CurrentValue == 0
			&& pInfo->Attribute[j].RawValue[0] == 0
			&& pInfo->Attribute[j].RawValue[1] == 0)
		{
			info.iconId = 1;
		}
		else if (pInfo->Threshold[j].ThresholdValue != 0
			&& pInfo->Attribute[j].CurrentValue < pInfo->Threshold[j].ThresholdValue)
		{
			info.iconId = 3;
		}
		else
		{
			info.iconId = 1;
		}
		bRet = true;
	}
	return bRet;
}

#define CONDITION_0D_ATTR(j)	\
(pInfo->IsSsd && !pInfo->IsRawValues8)	\
|| ((0x01 <= pInfo->Attribute[j].Id && pInfo->Attribute[j].Id <= 0x0D)	\
	|| (0xBB <= pInfo->Attribute[j].Id && pInfo->Attribute[j].Id <= 0xC1)	\
	|| (0xC3 <= pInfo->Attribute[j].Id && pInfo->Attribute[j].Id <= 0xD1)	\
	|| (0xD3 <= pInfo->Attribute[j].Id && pInfo->Attribute[j].Id <= 0xD4)	\
	|| (0xDC <= pInfo->Attribute[j].Id && pInfo->Attribute[j].Id <= 0xE4)	\
	|| (0xE6 <= pInfo->Attribute[j].Id && pInfo->Attribute[j].Id <= 0xE7)	\
	|| pInfo->Attribute[j].Id == 0xF0	\
	|| pInfo->Attribute[j].Id == 0xFA	\
	|| pInfo->Attribute[j].Id == 0xFE	\
	)

bool DiskInfoMgr::ProcessLess0DAttr(ListInfo & info, CAtaSmart::ATA_SMART_INFO * pInfo, int j)
{
	bool bRet = false;
	if (CONDITION_0D_ATTR(j))
	{
		if (pInfo->Threshold[j].ThresholdValue != 0
			&& pInfo->Attribute[j].CurrentValue < pInfo->Threshold[j].ThresholdValue)
		{
			info.iconId = 3;
		}
		else
		{
			info.iconId = 1;
		}
		bRet = true;
	}
	return bRet;
}

static __int64 CompareFileTime2(FILETIME time1, FILETIME time2)
{
	__int64 a = static_cast<__int64>(time1.dwHighDateTime) << 32 | time1.dwLowDateTime;
	__int64 b = static_cast<__int64>(time2.dwHighDateTime) << 32 | time2.dwLowDateTime;
	return b - a;
}

void DiskInfoMgr::getCPUTime()
{
	FILETIME idleTime;
	FILETIME kernelTime;
	FILETIME userTime;

	GetSystemTimes(&idleTime, &kernelTime, &userTime);

	__int64 idle = CompareFileTime2(m_prevCPU.idle, idleTime);
	__int64 kernel = CompareFileTime2(m_prevCPU.kernel, kernelTime);
	__int64 user = CompareFileTime2(m_prevCPU.user, userTime);

	if (kernel + user == 0)
	{
		m_iCPU = 0;
	}
	else
	{
		m_iCPU = static_cast<int>(abs((double)(kernel + user - idle) * 100 / (kernel + user)));
	}

	m_prevCPU.idle = idleTime;
	m_prevCPU.kernel = kernelTime;
	m_prevCPU.user = userTime;
}

void DiskInfoMgr::initMultiLanguage()
{
	QSettings reg(QString("HKEY_LOCAL_MACHINE/SOFTWARE/EaseUS/%1").arg(g_product).replace("/", "\\"), QSettings::NativeFormat);
	m_strCurLang = reg.value("Language", "null").toString();

	if (m_strCurLang == "null")
	{
		QSettings reg(QString("HKEY_LOCAL_MACHINE/SOFTWARE/EaseUS/%1").arg("EPM").replace("/", "\\"), QSettings::NativeFormat);
		m_strCurLang = reg.value("Language", "English").toString();
	}

	emit sigLanguageChanged();
}

void DiskInfoMgr::onReadEPMMsg()
{
	QByteArray bt = m_socket.readAll();

	if (bt.startsWith(DH_DATA_HEADER))
	{
		unsigned char *data = (unsigned char *)(bt.data() + strlen(DH_DATA_HEADER));
		m_iDiskIndex = *data;

		if (!m_refreshThread.isRunning())
			emit sigSwitchDiskIndex();
	}
}

struct InfoToEPM
{
	int statusIndex;
	int life;
	int temp;
	int tempIndex;
	int diskIndex;
	int hasSmartError;
};

void DiskInfoMgr::sendMsgToEPM()
{
	char buffer[1024];
	sprintf(buffer, DH_DATA_HEADER);

	int it = strlen(DH_DATA_HEADER);

	if (sizeof(buffer) >= m_Ata.vars.GetCount() * sizeof(InfoToEPM) + strlen(DH_DATA_HEADER))
	{
		InfoToEPM ite;

		for (int i = 0; i < m_Ata.vars.GetCount(); ++i)
		{
			auto &info = m_Ata.vars[i];
			ULONG nIndex = (ULONG)info.DiskStatus;
			if (nIndex > 4)
				nIndex = 0;

			if (m_mapStatus.find(info.PhysicalDriveId) != m_mapStatus.end())
			{
				if (m_mapStatus[info.PhysicalDriveId] != 0 && nIndex == 0)
					continue;		
				else 
					m_mapStatus[info.PhysicalDriveId] = nIndex;
			}
			else
				m_mapStatus[info.PhysicalDriveId] = nIndex;

			ite.statusIndex = (int)nIndex;
			nIndex = 1;
			if (info.Temperature > 70)
				nIndex = 2;
			if (info.Temperature < -100 || info.Temperature >= 200)
				nIndex = 0;
			ite.diskIndex = info.PhysicalDriveId;
			ite.tempIndex  = (int)nIndex;
			ite.temp = info.Temperature;
			ite.life = info.Life;

			ite.hasSmartError = checkHasSmartError(ite.diskIndex);

			memcpy(buffer + it, &ite, sizeof(InfoToEPM));
			it += sizeof(InfoToEPM);
		}
	}

	m_socket.write(buffer,it);
	m_socket.flush();
	m_socket.waitForBytesWritten();
}

void DiskInfoMgr::resetDiskList()
{
	m_listDiskInfos.clear();
	for (int i = 0; i < m_Ata.vars.GetCount(); i++)
	{
		QString des = QString::fromWCharArray(m_Ata.vars[i].Model.GetString());
		QString strCapacity;
		char formatBuffer[1024];

		if (m_Ata.vars[i].TotalDiskSize >= 1000)
		{
			sprintf(formatBuffer, "%.1f GB", m_Ata.vars[i].TotalDiskSize / 1000.0);
			strCapacity = formatBuffer;
		}
		else if (m_Ata.vars[i].TotalDiskSize > 0)
		{
			sprintf(formatBuffer, "%d MB", m_Ata.vars[i].TotalDiskSize);
			strCapacity = formatBuffer;
		}

		des += " ";
		des += strCapacity;

		QString ret;

		if (theme() == "dark")
			ret = "<span style=\"color:#898c99";
		else
			ret = "<span style=\"color:#6e7480";
		
		ret += ";font-size:14";
		ret += "\">" + des + "</span>";

		QString diskDes = QObject::tr("Disk %1 (%2)").arg(m_Ata.vars[i].PhysicalDriveId).arg(ret);
		m_listDiskInfos.append(QVariant(diskDes));
	}
}

bool DiskInfoMgr::nativeEventFilter(const QByteArray & eventType, void * message, long * result)
{
	if (eventType == "windows_generic_MSG" || eventType == "windows_dispatcher_MSG")
	{
		MSG * pMsg = reinterpret_cast<MSG *>(message);
		
		if (pMsg->message == WM_FULLSCREEN)
		{
			if ((UINT)pMsg->wParam == ABN_FULLSCREENAPP)
			{
				m_bIsFullScreen = pMsg->lParam;
				return  true;
			}
		}

		if (pMsg->message == WM_WTSSESSION_CHANGE)
		{
			if (pMsg->wParam == WTS_SESSION_UNLOCK || pMsg->wParam == WTS_CONSOLE_CONNECT || pMsg->wParam == WTS_SESSION_LOGON || pMsg->wParam == WTS_REMOTE_CONNECT)
			{
				refreshForPop();
				return true;
			}
		}

		if (pMsg->message == WM_COPYDATA)
		{
			COPYDATASTRUCT *data = (COPYDATASTRUCT*)(pMsg->lParam);

			if (data->lpData)
			{
				m_iDiskIndex = *(int *)data->lpData;
				if (!m_refreshThread.isRunning())
					emit sigSwitchDiskIndex();
			}
			
			emit sigShowFrame();
		}

	}

	return false;
}

void DiskInfoMgr::parseArgs()
{
	QStringList args = qApp->arguments();

	for (int i = 0; i < args.size(); ++i)
	{
		if (args[i].contains("diskHealthIndex="))
		{
			m_bIsEPMDiscoery = true;
			auto v = args[i].split('=');
			if (v.size() >= 2)
			{
				m_iDiskIndex = v[1].toInt();
			}
		}

		if (args[i].contains("isPe="))
		{
			m_bIsEPMDiscoery = true;
			auto v = args[i].split('=');
			if (v.size() >= 2)
			{
				m_bIsEPMPE = v[1] == "true";
			}
		}

		if (args[i].contains("themeName="))
		{
			m_bIsEPMDiscoery = true;
			auto v = args[i].split('=');
			if (v.size() >= 2)
			{
				m_strTheme = v[1];
			}
		}
	}


	if (m_strTheme.length() == 0)
	{
		QSettings set("config.ini", QSettings::IniFormat);
		m_strTheme = set.value("main/realStyleName","light").toString();
	}

	m_bAutoStart = args.contains(_AUTOSTARTUP_);
	m_bEPMStart = args.contains(_EPM_CONSOLE);
	inEPM = m_bEPMStart;

}

void DiskInfoMgr::doHoverEnter(int x, int y)
{
	if (m_bNoEnter)
		fnSendUserInfo("DiskHealth", "Info_SysIcon_Hover");
	m_bNoEnter = false;
	emit sigHoverEnter(x, y);
}

RefreshThread::RefreshThread(DiskInfoMgr * parent):QThread(parent),m_mgr(parent)
{

}

void RefreshThread::run()
{
	if (m_mgr)
		m_mgr->loadInfo();
}
