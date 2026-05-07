#pragma once

#include "../../../DiskCopy/mod.TBQtLib/BPWinFrm.h"

class CBPLabel;
class CBPComboBox;
class CBPBox;
class CBPPushBtn;
class CBPTableWidget;
class CDiskHealthMainWin : public CBPWinFrm
{
	Q_OBJECT

public:
	friend class DiskInfoMgr;
	CDiskHealthMainWin(QWidget* parent = nullptr);
	virtual ~CDiskHealthMainWin();

	// 重写基类方法
	virtual void OnClose() override;

private:
	void InitTitle();
	void Init();
	void InitDiskList();
	void InitStatusWid(QBoxLayout* pStatusWid);
	void InitTempWid(QBoxLayout* pTempWid);

	void InitData();
	void InitInfo();

	CBPLabel* m_pLeftLb = nullptr;
	CBPLabel* m_pRightLb = nullptr;
	CBPComboBox* m_pDiskList = nullptr;
	CBPPushBtn* m_pRefreshBtn = nullptr;
	CBPBox* m_pTopWid = nullptr;
	QFrame* m_pBriefInfo = nullptr;

	//CBPBox* m_pStatusWid = nullptr;
	//CBPBox* m_pTempWid = nullptr;

	CBPTableWidget* m_pAtrriList = nullptr;

	
	CBPLabel* m_pStatusTxt = nullptr;
	CBPLabel* m_pHealthPersentTxt = nullptr;
	CBPLabel* m_pStatusPix = nullptr;

	CBPLabel* m_pTempTxt = nullptr;
	CBPLabel* m_pTempStatusTxt = nullptr;
	CBPLabel* m_pTempPix = nullptr;
};
