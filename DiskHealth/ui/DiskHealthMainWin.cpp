#include "DiskHealthMainWin.h"

#include "../../../DiskCopy/mod.TBQtLib/BPLabel.h"
#include "../../../DiskCopy/mod.TBQtLib/BPBox.h"
#include "../../../DiskCopy/mod.TBQtLib/BPComboBox.h"
#include "../../../DiskCopy/mod.TBQtLib/BPPushBtn.h"
#include "../../../DiskCopy/mod.TBQtLib/BPTableWidget.h"
#include "../../../DiskCopy/mod.TBQtLib/BPCheckBox.h"

CDiskHealthMainWin::CDiskHealthMainWin(QWidget* parent)
	: CBPWinFrm(parent)
{
	Init();
	// todo 放线程
	InitData();
}

CDiskHealthMainWin::~CDiskHealthMainWin()
{
	
}



void CDiskHealthMainWin::OnClose()
{
	__super::OnClose();
}

void CDiskHealthMainWin::InitTitle()
{
	SetTitleText(tr("EaseUS Disk Health"));
	//setTitleHeight(53);
	SetTitleBg(QColor("#F4F2F7"));
	ShowTitleBtn(Btn_Setting);
	ShowTitleBtn(Btn_Close);

}

void CDiskHealthMainWin::InitDiskList()
{
	do {
		if (m_pDiskList == nullptr) break;
	
		// test
		QStringList temp = { "disk1", "disk2" };
		m_pDiskList->addItems(temp);

	} while (0);
}

void CDiskHealthMainWin::InitStatusWid(QBoxLayout* pStatusWid)
{
	do {
		if (pStatusWid == nullptr) break;
		
		QVBoxLayout* pLeft = new(std::nothrow) QVBoxLayout;
		pLeft->setSpacing(0);

		auto* pTitleLb = new (std::nothrow) CBPLabel;
		pTitleLb->setText(tr("Status"));
		m_pStatusTxt = new (std::nothrow) CBPLabel;
		m_pHealthPersentTxt = new (std::nothrow) CBPLabel;

		pLeft->addWidget(pTitleLb);
		pLeft->addWidget(m_pHealthPersentTxt);
		pLeft->addWidget(m_pStatusTxt);


		m_pStatusPix = new (std::nothrow) CBPLabel;

		pStatusWid->addLayout(pLeft);
		pStatusWid->addWidget(m_pStatusPix);
	} while (0);

}

void CDiskHealthMainWin::InitTempWid(QBoxLayout* pTempWid)
{
	do {
		if (pTempWid == nullptr) break;
	
		auto* pLeft = new(std::nothrow) QVBoxLayout;
		pLeft->setSpacing(0);

		auto* pTitleLb = new (std::nothrow) CBPLabel;
		pTitleLb->setText(tr("Temperature"));
		m_pTempTxt = new (std::nothrow) CBPLabel;
		m_pTempStatusTxt = new (std::nothrow) CBPLabel;

		pLeft->addWidget(pTitleLb);
		pLeft->addWidget(m_pTempTxt);
		pLeft->addWidget(m_pTempStatusTxt);

		m_pTempPix = new (std::nothrow) CBPLabel;

		pTempWid->addLayout(pLeft);
		pTempWid->addWidget(m_pTempPix);

	} while (0);
}


void CDiskHealthMainWin::Init()
{
	do {
		InitCommonWnd(true, 6, 0, 0);
		SetMainBoxBorder(18, 22, 18, 18);
		InitTitle();
		setFixedSize(1141, 808);
		setObjectName("CDiskHealthMainWin");
		setStyleSheet("#CDiskHealthMainWin{border:2px solid #F4F2F7; background: #FFFFFF; border-radius: 6px;}");

		m_pLeftLb = new(std::nothrow) CBPLabel;
		m_pLeftLb->setWordWrap(true);
		m_pLeftLb->setText(tr("Which disk do you care about?"));
		//QWidget* pWidget, bool bExpand, quint32 uPadding, bool isHLayout, Qt::Alignment alignment)
		AppendContentEx(m_pLeftLb, false, 0, false, Qt::AlignTop);
		
		m_pVBoxMain->addSpacing(18);
		// 上
		m_pTopWid = new(std::nothrow) CBPBox(false);
		m_pTopWid->SetSpacing(0);
		m_pTopWid->setAttribute(Qt::WA_TranslucentBackground);
	
		m_pDiskList = new(std::nothrow) CBPComboBox;
		m_pDiskList->setObjectName("DiskList");
		m_pDiskList->setStyleSheet("#DiskList{border:2px solid #F4F2F7; background: #EBEEF2; border-radius: 4px;}");

		m_pDiskList->setFixedSize(1027, 30);
		//m_pDiskList->addItem(tr("Disk 0"));
		m_pTopWid->PackStart(m_pDiskList, false, false, 0);
		m_pTopWid->AddSpacing(18);
		m_pRefreshBtn = new(std::nothrow) CBPPushBtn;
		m_pRefreshBtn->setFixedSize(46, 28);
		m_pTopWid->PackStart(m_pRefreshBtn, false, false, 0);

		connect(m_pRefreshBtn, &CBPPushBtn::clicked, this, [this]() {
			// todo 刷新数据
			InitData();
			});

		AppendContentEx(m_pTopWid, false, 0, false, Qt::AlignTop);

		m_pVBoxMain->addSpacing(18);
		// 下
		auto pContentWid = new(std::nothrow) CBPBox(false);
		pContentWid->SetSpacing(0);
		pContentWid->setAttribute(Qt::WA_TranslucentBackground);

		// 左
		auto* pLeftWid = new (std::nothrow) CBPBox(true);
		pLeftWid->SetSpacing(0);
		pLeftWid->setAttribute(Qt::WA_TranslucentBackground);

		// 左上
		auto* pLeftTopWid = new (std::nothrow) CBPBox(false);
		pLeftTopWid->SetSpacing(0);
		pLeftTopWid->setAttribute(Qt::WA_TranslucentBackground);

		// 状态
		auto *pStatusWid = new (std::nothrow) QFrame;
		QHBoxLayout* pStatusLayout = new (std::nothrow) QHBoxLayout(pStatusWid);
		pStatusWid->setObjectName("StatusWid");
		pLeftTopWid->PackStart(pStatusWid, false, false, 0);

		pLeftTopWid->AddSpacing(18);

		// 温度
		auto *pTempWid = new (std::nothrow) QFrame;
		QHBoxLayout* pTempLayout = new (std::nothrow) QHBoxLayout(pTempWid);
		pTempWid->setObjectName("StatusWid");
		pLeftTopWid->PackStart(pTempWid, false, false, 0);

		pStatusWid->setStyleSheet("QFrame#StatusWid{background: #EBF5FF; border-radius: 4px;}");
		pTempWid->setStyleSheet("QFrame#StatusWid{background: #EBF5FF; border-radius: 4px;}");

		InitStatusWid(pStatusLayout);
		InitTempWid(pTempLayout);


		// 左下
		m_pBriefInfo = new (std::nothrow) QFrame;
		m_pBriefInfo->setMinimumHeight(442);
		m_pBriefInfo->setObjectName("BriefInfo");
		m_pBriefInfo->setStyleSheet("QFrame#BriefInfo{background: #EBEEF2; border-radius: 4px;}");

		pLeftWid->PackStart(pLeftTopWid);
		pLeftWid->AddSpacing(18);
		pLeftWid->PackStart(m_pBriefInfo);

		// 右
		m_pAtrriList = new (std::nothrow) CBPTableWidget;

		pContentWid->PackStart(pLeftWid, true, false, 0);
		pContentWid->AddSpacing(18);
		pContentWid->PackStart(m_pAtrriList, true, false, 0);

		AppendContentEx(pContentWid, true, false, 0);
		
		m_pVBoxMain->addSpacing(18);

		auto* pBottom = new (std::nothrow) CBPBox(false);
		pBottom->SetSpacing(0);
		pBottom->setAttribute(Qt::WA_TranslucentBackground);
		
		auto* pCheckBtn = new CBPCheckBox;
		pCheckBtn->setText(tr("Display alert when disk problems are found. (Ask for autostart boot)"));
		pBottom->PackStart(pCheckBtn, true, false, 0);
		pBottom->AddSpacerItem(true);
		
		auto* pDoBtn = new(std::nothrow) CBPPushBtn;
		pDoBtn->setFixedSize(130, 37);
		pDoBtn->setText(tr("Done"));
		pBottom->PackStart(pDoBtn);
		
		AppendContentEx(pBottom, true, false, 0);


	} while (0);
}

void CDiskHealthMainWin::InitData()
{
	// test
	InitDiskList();
	InitInfo();
}

void CDiskHealthMainWin::InitInfo()
{
	do {
		if (nullptr == m_pHealthPersentTxt) break;
		if (nullptr == m_pStatusTxt) break;
		if (nullptr == m_pStatusPix) break;

		if (nullptr == m_pTempTxt) break;
		if (nullptr == m_pTempStatusTxt) break;
		if (NULL == m_pTempPix) break;
		
		m_pHealthPersentTxt->setText(tr("90%"));
		m_pStatusTxt->setText(tr("Good"));
		m_pStatusPix->setPixmap(QPixmap(":/DiskHealth/img/healthy.png").scaled(24, 24, Qt::KeepAspectRatio, Qt::SmoothTransformation));
		

		m_pTempTxt->setText(tr("45c"));
		m_pTempStatusTxt->setText(tr("Normal"));
		m_pTempPix->setPixmap(QPixmap(":/DiskHealth/img/normal.png").scaled(24, 24, Qt::KeepAspectRatio, Qt::SmoothTransformation));


	} while (0);
}