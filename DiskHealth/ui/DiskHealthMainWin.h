#pragma once

#include "../../../DiskCopy/mod.TBQtLib/BPWinFrm.h"
#include "../DiskInfoMgr.h"

#include <QMenu>
#include <QActionGroup>

class CBPLabel;
class CBPComboBox;
class CBPBox;
class CBPPushBtn;
class CBPTableWidget;
class CBPCheckBox;
class CDiskHealthMainWin : public CBPWinFrm
{
	Q_OBJECT

public:
	friend class DiskInfoMgr;
	CDiskHealthMainWin(DiskInfoMgr* pDiskMgr, QWidget* parent = nullptr);
	virtual ~CDiskHealthMainWin();

	// 重写基类方法
	virtual void OnClose() override;

public slots:
	void onDiskInfosChanged();
	void onLanguageChanged();
	void onTempUnitChanged();
	void onSwitchToDisk(int diskIndex);   // 外部请求切换到指定磁盘（物理索引）
	void onSettingButtonClicked(); 

protected:
	void showEvent(QShowEvent* event) override;

	void retranslateUI();          // 刷新所有界面文字
	void updateLanguageMenuCheck();// 更新菜单项的选中状态

public slots:
	void onDiskSelected(int index);
	void onRefreshClicked();
	void onTemperatureClicked();
	void onAutoStartToggled(bool checked);
	void onDoneClicked();

private:
	void InitTitle();
	void Init();
	void InitDiskList();
	void InitStatusWid(QBoxLayout* pStatusWid);
	void InitTempWid(QBoxLayout* pTempWid);

	void InitData();
	void InitInfo();

	void InitConnections();

	void loadDiskList();                 // 加载磁盘下拉列表
	void updateDiskInfo(int diskIndex);  // 更新选中磁盘的状态、温度、左侧参数、右侧表格
	void updateStatusAndTemp(int diskIndex);
	void updateDiskStatusInfo(int diskIndex); // 左侧参数列表（Power On Hours etc.）
	void updateAttributeTable(int diskIndex); // 右侧 SMART 属性表格
	
	void showLoading();
	void hideLoading();

	void updateAutoStartState();

	void initLangList();


	DiskInfoMgr*		m_pDiskMgr = nullptr;

	CBPLabel*			m_pLeftLb = nullptr;
	CBPLabel*			m_pRightLb = nullptr;
	CBPComboBox*		m_pDiskList = nullptr;
	QPushButton*		m_pRefreshBtn = nullptr;
	CBPBox*				m_pTopWid = nullptr;
	QFrame*				m_pDiskStatusInfo = nullptr;	
	QTableView*			m_pAtrriList = nullptr;

	CBPLabel*			m_pStatusTitleLb = nullptr;
	CBPLabel*			m_pStatusText = nullptr;
	CBPLabel*			m_pHealthPersentTxt = nullptr;

	CBPLabel*			m_pTempTitleLb = nullptr;
	CBPLabel*			m_pTempTxt = nullptr;
	CBPLabel*			m_pTempStatusTxt = nullptr;
	CBPLabel*			m_pAutoStartLabel = nullptr;
	CBPCheckBox*		m_pAutoStartCheck = nullptr;

	int					m_currentDiskIndex = -1;
	bool				m_bFirstShow = true;

	QVBoxLayout*		m_pDiskStatusInfoLayout = nullptr;
	QFrame*				m_pTempWid = nullptr;
	QFrame*				m_pStatusWid = nullptr;

	QFrame*				m_pLoadingOverlay = nullptr;	// 刷新时的加载遮罩

	CBPPushBtn*			m_pDoneBtn = nullptr;

	QMenu*				m_langMenu = nullptr;			// 语言菜单
	QActionGroup*		m_langActionGroup = nullptr;	// 菜单项组（实现单选）
};

// 温度卡片点击处理（独立事件过滤器）
class TempWidgetClickFilter : public QObject
{
public:
	TempWidgetClickFilter(CDiskHealthMainWin* win, QObject* parent = nullptr)
		: QObject(parent), m_win(win) {
	}

protected:
	bool eventFilter(QObject* watched, QEvent* event) override
	{
		if (event->type() == QEvent::MouseButtonPress) {
			m_win->onTemperatureClicked();
			return true;  // 事件已处理
		}
		return QObject::eventFilter(watched, event);
	}

private:
	CDiskHealthMainWin* m_win;
};