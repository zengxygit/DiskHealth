#pragma once

#include "../../../DiskCopy/mod.TBQtLib/BPWinFrm.h"
#include "../DiskInfoMgr.h"

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

	// 公开接口供 DiskInfoMgr 回调
	void onDiskInfosChanged();
	void onLanguageChanged();
	void onTempUnitChanged();
	void onSwitchToDisk(int diskIndex);   // 外部请求切换到指定磁盘（物理索引）

protected:
	void showEvent(QShowEvent* event) override;

private slots:
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
	

	DiskInfoMgr*		m_pDiskMgr = nullptr;

	CBPLabel*			m_pLeftLb = nullptr;
	CBPLabel*			m_pRightLb = nullptr;
	CBPComboBox*		m_pDiskList = nullptr;
	QPushButton*		m_pRefreshBtn = nullptr;
	CBPBox*				m_pTopWid = nullptr;
	QFrame*				m_pDiskStatusInfo = nullptr;	
	QTableView*			m_pAtrriList = nullptr;

	CBPLabel*			m_pStatusText = nullptr;
	CBPLabel*			m_pHealthPersentTxt = nullptr;
	CBPLabel*			m_pTempTxt = nullptr;
	CBPLabel*			m_pTempStatusTxt = nullptr;
	CBPCheckBox*		m_pAutoStartCheck = nullptr;

	int					m_currentDiskIndex = -1;
	bool				m_bFirstShow = true;

	QVBoxLayout*		m_pDiskStatusInfoLayout = nullptr;
	QFrame*				m_pTempWid = nullptr;
	QFrame*				m_pStatusWid = nullptr;

};
