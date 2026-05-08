#include "DiskHealthMainWin.h"

#include "../../../DiskCopy/mod.TBQtLib/BPLabel.h"
#include "../../../DiskCopy/mod.TBQtLib/BPBox.h"
#include "../../../DiskCopy/mod.TBQtLib/BPComboBox.h"
#include "../../../DiskCopy/mod.TBQtLib/BPPushBtn.h"
#include "../../../DiskCopy/mod.TBQtLib/BPTableWidget.h"
#include "../../../DiskCopy/mod.TBQtLib/BPCheckBox.h"

CDiskHealthMainWin::CDiskHealthMainWin(DiskInfoMgr* pDiskMgr, QWidget* parent)
	: CBPWinFrm(parent)
	, m_pDiskMgr(pDiskMgr)
{
	Init();

	InitConnections();

	// todo 放线程
	InitData();
}

CDiskHealthMainWin::~CDiskHealthMainWin()
{
	
}

void CDiskHealthMainWin::InitConnections()
{
	if (!m_pDiskMgr)
		return;

	// 磁盘下拉选择
	connect(m_pDiskList, QOverload<int>::of(&CBPComboBox::currentIndexChanged),
		this, &CDiskHealthMainWin::onDiskSelected);
	// 刷新按钮
	connect(m_pRefreshBtn, &CBPPushBtn::clicked, this, &CDiskHealthMainWin::onRefreshClicked);
	// 温度卡片点击
	//connect(m_pTempFrame, &QFrame::mousePressEvent, this, [this](QMouseEvent*) { onTemperatureClicked(); });
	// 自动启动复选框状态改变
	connect(m_pAutoStartCheck, &CBPCheckBox::toggled, this, &CDiskHealthMainWin::onAutoStartToggled);
	// 完成按钮
	//connect(m_pDoneBtn, &CBPPushBtn::clicked, this, &CDiskHealthMainWin::onDoneClicked);

	// DiskInfoMgr 信号
	connect(m_pDiskMgr, &DiskInfoMgr::sigDiskInfosChanged, this, &CDiskHealthMainWin::onDiskInfosChanged);
	connect(m_pDiskMgr, &DiskInfoMgr::sigLanguageChanged, this, &CDiskHealthMainWin::onLanguageChanged);
	connect(m_pDiskMgr, &DiskInfoMgr::sigIsTempFchanged, this, &CDiskHealthMainWin::onTempUnitChanged);
	connect(m_pDiskMgr, &DiskInfoMgr::sigSwitchDiskIndex, this, [this]() {
		int idx = m_pDiskMgr->diskIndexArg();
		if (idx >= 0 && idx < m_pDiskList->count())
			m_pDiskList->setCurrentIndex(idx);
		});
	connect(m_pDiskMgr, &DiskInfoMgr::sigAboutRefresh, this, [this]() {
		//setWaitingOverlayVisible(true);
		});
}

void CDiskHealthMainWin::showEvent(QShowEvent* event)
{
	__super::showEvent(event);
	if (m_bFirstShow)
	{
		m_bFirstShow = false;
		// 首次显示时，如果后台还没有数据，触发一次刷新
		if (m_pDiskMgr && m_pDiskList->count() == 0)
		{
			m_pDiskMgr->refreshDisks();
		}
	}
}


void CDiskHealthMainWin::onDiskSelected(int index)
{
	if (index >= 0)
	{
		updateDiskInfo(index);
		if (m_pDiskMgr)
		{
			m_pDiskMgr->selectDisk(index);

			// 检查数据是否有效，若无效则强制刷新一次
			QVariantMap status = m_pDiskMgr->getDiskStatus(index);
			if (status.isEmpty() || status["statusInfo"].toString().isEmpty())
			{
				m_pDiskMgr->refreshDisks();
			}
		}

		//// 强制立即重绘表格
		//if (m_pAtrriList) m_pAtrriList->viewport()->update();
		//if (m_pDiskStatusTable) m_pDiskStatusTable->viewport()->update();
	}
}

void CDiskHealthMainWin::onRefreshClicked()
{
	if (m_pDiskMgr)
	{
		m_pDiskMgr->clickRefresh();
		m_pDiskMgr->refreshDisks();
	}
}

void CDiskHealthMainWin::onTemperatureClicked()
{
	if (m_pDiskMgr)
	{
		m_pDiskMgr->setIsTempF(!m_pDiskMgr->isTempF());
	}
}

void CDiskHealthMainWin::onAutoStartToggled(bool checked)
{
	// 这里只是记录，实际保存由 Done 按钮调用 doDone 时处理
	Q_UNUSED(checked);
}

void CDiskHealthMainWin::onDoneClicked()
{
	if (m_pDiskMgr)
	{
		bool result = m_pDiskMgr->doDone(m_pAutoStartCheck->isChecked());
		if (result)
			hide();
		else
			close();
	}
}

void CDiskHealthMainWin::onDiskInfosChanged()
{
	// 刷新磁盘列表，并保持当前选中的索引（如果有）
	int oldIndex = m_pDiskList->currentIndex();
	loadDiskList();
	if (oldIndex >= 0 && oldIndex < m_pDiskList->count())
		m_pDiskList->setCurrentIndex(oldIndex);
	else if (m_pDiskList->count() > 0)
		m_pDiskList->setCurrentIndex(0);
	//setWaitingOverlayVisible(false);
}

void CDiskHealthMainWin::onLanguageChanged()
{
	//// 重新翻译界面静态文本
	//m_pLeftLb->setText(tr("Which disk do you care about?"));
	//m_pStatusTitle->setText(tr("Status"));
	//m_pTempTitle->setText(tr("Temperature"));
	//m_pAutoStartCheck->setText(tr("Display alert when disk problems are found. (Ask for autostart boot)"));
	//m_pDoneBtn->setText(tr("Done"));
	//// 重新加载当前磁盘信息，因为 getDiskStatusInfo 会返回翻译后的文本
	//if (m_currentDiskIndex >= 0)
	//	updateDiskInfo(m_currentDiskIndex);
	//// 刷新表格标题
	//updateAttributeTable(m_currentDiskIndex);
}

void CDiskHealthMainWin::onTempUnitChanged()
{
	//// 温度单位变化，刷新当前磁盘的温度显示
	//if (m_currentDiskIndex >= 0)
	//	updateStatusAndTemp(m_currentDiskIndex);
}

void CDiskHealthMainWin::onSwitchToDisk(int diskIndex)
{
	if (diskIndex >= 0 && diskIndex < m_pDiskList->count())
		m_pDiskList->setCurrentIndex(diskIndex);
}

static QString stripHtml(const QString& html) {
	QTextDocument doc;
	doc.setHtml(html);
	return doc.toPlainText();
}

void CDiskHealthMainWin::loadDiskList()
{
	if (!m_pDiskMgr)
		return;
	QVariantList disks = m_pDiskMgr->getDiskInfos();
	m_pDiskList->clear();
	for (const QVariant& v : disks)
	{
		m_pDiskList->addItem(stripHtml(v.toString()));
	}
	if (m_pDiskList->count() > 0 && m_currentDiskIndex == -1)
		m_pDiskList->setCurrentIndex(0);
}

void CDiskHealthMainWin::updateDiskInfo(int diskIndex)
{
	if (!m_pDiskMgr || diskIndex < 0)
		return;
	if (diskIndex >= m_pDiskList->count())
		return;

	m_currentDiskIndex = diskIndex;
	updateStatusAndTemp(diskIndex);
	updateDiskStatusInfo(diskIndex);
	updateAttributeTable(diskIndex);
}

void CDiskHealthMainWin::updateStatusAndTemp(int diskIndex)
{
	QVariantMap status = m_pDiskMgr->getDiskStatus(diskIndex);
	// 状态
	QString statusText = status["statusInfo"].toString();
	QString life = status["life"].toString();
	int statusIdx = status["statusIndex"].toInt();
	QString statusColor;
	QString statusIconPath;
	switch (statusIdx)
	{
	case 1:
		statusColor = "#2DBD8B";
		statusIconPath = ":/res/pic_status_good.png";
		break;
	case 2:
	case 3:
		statusColor = "#FF5327";
		statusIconPath = ":/res/pic_status_bad.png";
		break;
	default:
		statusColor = "#6E7480";
		statusIconPath = ":/res/pic_status_none.png";
		break;
	}
	m_pStatusText->setText(statusText);
	m_pStatusText->setStyleSheet(QString("font-size:16px; font-weight:bold; color:%1;").arg(statusColor));
	m_pHealthPersentTxt->setText(life.isEmpty() ? "--" : life);
	//m_pStatusPix->setPixmap(QPixmap(statusIconPath).scaled(48, 70, Qt::KeepAspectRatio, Qt::SmoothTransformation));
	m_pStatusWid->setStyleSheet(QString("QFrame#StatusWid{border-image: url(%1); border-radius: 4px;}").arg(statusIconPath));
	// 温度
	QString tempValue = status["tempValue"].toString();
	QString tempDesc = status["tempDesc"].toString();
	int tempIdx = status["tempIndex"].toInt();
	QString tempColor;
	QString tempIconPath;
	switch (tempIdx)
	{
	case 1:
		tempColor = "#308FFF";
		tempIconPath = ":/res/pic_temprature_normal.png";
		break;
	case 2:
	case 3:
		tempColor = "#FF5327";
		tempIconPath = ":/res/pic_temprature_warning.png";
		break;
	default:
		tempColor = "#6E7480";
		tempIconPath = ":/res/pic_temprature_none.png";
		break;
	}
	m_pTempTxt->setText(tempValue);
	m_pTempTxt->setStyleSheet(QString("font-size:16px; font-weight:bold; color:%1;").arg(tempColor));
	m_pTempStatusTxt->setText(tempDesc);
	m_pTempWid->setStyleSheet(QString("QFrame#TempWid{border-image: url(%1); border-radius: 4px;}").arg(tempIconPath));
}

void CDiskHealthMainWin::updateDiskStatusInfo(int diskIndex)
{
	if(!m_pDiskMgr) return;
	QVariantList infoList = m_pDiskMgr->getDiskStatusInfo(diskIndex);
	if (!m_pDiskStatusTable) return;

	// 总行数 = 键值对数量 * 2
	m_pDiskStatusTable->setRowCount(infoList.size() * 2);

	for (int i = 0; i < infoList.size(); ++i) {
		QVariantMap item = infoList[i].toMap();
		QString key = item["arg"].toString();
		QString value = item["value"].toString();

		// 键行
		QTableWidgetItem* keyItem = new QTableWidgetItem(key);
		keyItem->setForeground(QColor("#6F7884"));
		keyItem->setFont(QFont("SegoeUI", 14));        // 稍小字体
		keyItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		// 可选：键行背景色
		// keyItem->setBackground(QColor("#F0F0F0"));

		// 值行
		QTableWidgetItem* valueItem = new QTableWidgetItem(value);
		valueItem->setForeground(QColor("#1F2C40"));
		valueItem->setFont(QFont("SegoeUI", 14, QFont::Bold));
		valueItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);

		int row = i * 2;
		m_pDiskStatusTable->setItem(row, 0, keyItem);
		m_pDiskStatusTable->setItem(row + 1, 0, valueItem);
	}

	// 调整列宽：单列填满整个表格宽度
	m_pDiskStatusTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
	// 调整行高以适应内容
	m_pDiskStatusTable->resizeRowsToContents();
}

void CDiskHealthMainWin::updateAttributeTable(int diskIndex)
{
	if (!m_pDiskMgr) return;
	m_pDiskMgr->loadInfoList(diskIndex);

	// 获取原始模型（只是为了方便取数据）
	AttrModel* pAttrModel = dynamic_cast<AttrModel*>(m_pDiskMgr->getModel());
	if (!pAttrModel) return;

	int rowCount = pAttrModel->rowCount();
	if (rowCount == 0) return;

	// 创建标准表格模型
	QStandardItemModel* pViewModel = new QStandardItemModel(rowCount, 0, this);
	bool rawMode = m_pDiskMgr->showRaw(diskIndex);

	// 根据 rawMode 设置列数和表头
	if (rawMode) {
		pViewModel->setColumnCount(4);
		pViewModel->setHorizontalHeaderLabels(QStringList() << "" << tr("ID") << tr("Attribute") << tr("Raw Value"));
	}
	else {
		pViewModel->setColumnCount(5);
		pViewModel->setHorizontalHeaderLabels(QStringList() << "" << tr("ID") << tr("Attribute") << tr("Current") << tr("Threshold"));
	}

	// 状态颜色映射
	auto getStatusIcon = [](int statusVal) -> QIcon {
	QColor color;
	switch (statusVal) {
	case 1: color = QColor(45, 189, 139); break;   // 绿色 #2DBD8B
	case 2: color = QColor(245, 155, 0); break;    // 橙色 #F59B00
	case 3: color = QColor(238, 9, 9); break;      // 红色 #EE0909
	default: color = QColor(192, 192, 192); break; // 灰色
	}
	QPixmap pixmap(16, 16);
	pixmap.fill(Qt::transparent);
	QPainter painter(&pixmap);
	painter.setRenderHint(QPainter::Antialiasing);
	painter.setBrush(color);
	painter.setPen(Qt::NoPen);
	painter.drawEllipse(2, 2, 12, 12); // 绘制圆形
	return QIcon(pixmap);
	};

	// 逐行填充数据
	for (int row = 0; row < rowCount; ++row) {
		QModelIndex idx = pAttrModel->index(row, 0);
		QVariant status = pAttrModel->data(idx, AttrModel::USER_STATUS);
		QVariant id = pAttrModel->data(idx, AttrModel::USER_ID);
		QVariant attr = pAttrModel->data(idx, AttrModel::USER_ATTR);
		QVariant current = pAttrModel->data(idx, AttrModel::USER_COUNT);
		QVariant threshold = pAttrModel->data(idx, AttrModel::USER_HOLD);
		QVariant raw = pAttrModel->data(idx, AttrModel::USER_RAW);

		int statusVal = status.toInt();
		QStandardItem* statusItem = new QStandardItem();
		statusItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		statusItem->setIcon(getStatusIcon(statusVal));
		statusItem->setText("");            // 不显示文本
		statusItem->setTextAlignment(Qt::AlignCenter);
		pViewModel->setItem(row, 0, statusItem);
		pViewModel->setItem(row, 1, new QStandardItem(id.toString()));
		pViewModel->setItem(row, 2, new QStandardItem(attr.toString()));

		if (rawMode) {
			pViewModel->setItem(row, 3, new QStandardItem(raw.toString()));
		}
		else {
			pViewModel->setItem(row, 3, new QStandardItem(current.toString()));
			pViewModel->setItem(row, 4, new QStandardItem(threshold.toString()));
		}
	}

	// 设置模型到 QTableView
	m_pAtrriList->setModel(pViewModel);

	// 调整列宽和样式
	m_pAtrriList->resizeColumnToContents(0);
	m_pAtrriList->resizeColumnToContents(1);
	m_pAtrriList->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
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

	} while (0);
}

void CDiskHealthMainWin::InitStatusWid(QBoxLayout* m_pStatusWid)
{
	do {
		if (m_pStatusWid == nullptr) break;
		
		QVBoxLayout* pLeft = new(std::nothrow) QVBoxLayout;
		pLeft->setSpacing(0);

		auto* pTitleLb = new (std::nothrow) CBPLabel;
		pTitleLb->setText(tr("Status"));
		m_pStatusText = new (std::nothrow) CBPLabel;
		m_pHealthPersentTxt = new (std::nothrow) CBPLabel;

		pLeft->addWidget(pTitleLb);
		pLeft->addWidget(m_pHealthPersentTxt);
		pLeft->addWidget(m_pStatusText);


		//m_pStatusPix = new (std::nothrow) CBPLabel;
		//m_pStatusPix->setFixedSize(48, 70);
		m_pStatusWid->addLayout(pLeft);
		//m_pStatusWid->addWidget(m_pStatusPix);
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

		//m_pTempPix = new (std::nothrow) CBPLabel;
		//m_pTempPix->setFixedSize(48, 70);
		pTempWid->addLayout(pLeft);
		//pTempWid->addWidget(m_pTempPix);

	} while (0);
}


void CDiskHealthMainWin::Init()
{
	do {
		InitCommonWnd(true, 4, 0, 0);
		SetMainBoxBorder(28, 18, 28, 20);
		InitTitle();
		setFixedSize(848, 598);
		setObjectName("CDiskHealthMainWin");
		setStyleSheet("#CDiskHealthMainWin{border:2px solid #F4F2F7; background: #FFFFFF; border-radius: 4px;}");

		m_pLeftLb = new(std::nothrow) CBPLabel;
		m_pLeftLb->setWordWrap(true);
		m_pLeftLb->setText(tr("Which disk do you care about?"));
		//QWidget* pWidget, bool bExpand, quint32 uPadding, bool isHLayout, Qt::Alignment alignment)
		AppendContentEx(m_pLeftLb, false, 0, false, Qt::AlignTop);
		
		m_pVBoxMain->addSpacing(10);
		// 上
		m_pTopWid = new(std::nothrow) CBPBox(false);
		m_pTopWid->SetSpacing(0);
		m_pTopWid->setAttribute(Qt::WA_TranslucentBackground);
	
		m_pDiskList = new(std::nothrow) CBPComboBox;
		m_pDiskList->setObjectName("DiskList");
		m_pDiskList->setStyleSheet("#DiskList{border:2px solid #F4F2F7; background: #EBEEF2; border-radius: 4px;}");


		m_pTopWid->PackStart(m_pDiskList, false, false, 0);
		m_pTopWid->AddSpacing(8);
		m_pRefreshBtn = new(std::nothrow) CBPPushBtn;
		m_pRefreshBtn->setFixedSize(40, 24);
		m_pTopWid->PackStart(m_pRefreshBtn, false, false, 0);
		// 刷新
		connect(m_pRefreshBtn, &CBPPushBtn::clicked, this, [this]() {
			// todo 刷新处理
			InitData();
			});

		AppendContentEx(m_pTopWid, false, 0, false, Qt::AlignTop);

		m_pVBoxMain->addSpacing(18);
		
		// 中间
		auto pContentWid = new(std::nothrow) CBPBox(false);
		pContentWid->SetSpacing(0);
		pContentWid->setAttribute(Qt::WA_TranslucentBackground);

		// 左
		auto* pLeftWid = new (std::nothrow) CBPBox(true);
		pLeftWid->setFixedWidth(286);
		pLeftWid->SetSpacing(0);
		pLeftWid->setAttribute(Qt::WA_TranslucentBackground);

		// 左上
		auto* pLeftTopWid = new (std::nothrow) CBPBox(false);
		pLeftTopWid->SetSpacing(0);
		pLeftTopWid->setAttribute(Qt::WA_TranslucentBackground);

		// 状态控件
		m_pStatusWid = new (std::nothrow) QFrame;
		m_pStatusWid->setFixedSize(140, 70);
		QHBoxLayout* pStatusLayout = new (std::nothrow) QHBoxLayout(m_pStatusWid);
		m_pStatusWid->setObjectName("StatusWid");
		pLeftTopWid->PackStart(m_pStatusWid, false, false, 0);

		pLeftTopWid->AddSpacing(5);

		// 温度控件
		m_pTempWid = new (std::nothrow) QFrame;
		m_pTempWid->setFixedSize(140, 70);
		QHBoxLayout* pTempLayout = new (std::nothrow) QHBoxLayout(m_pTempWid);
		pTempLayout->setSpacing(0);
		pTempLayout->setContentsMargins(0, 0, 0, 0);
		m_pTempWid->setObjectName("TempWid");
		pLeftTopWid->PackStart(m_pTempWid, false, false, 0);

		//m_pStatusWid->setStyleSheet("QFrame#StatusWid{background: #EBF5FF; border-radius: 4px;}");
		//m_pTempWid->setStyleSheet("QFrame#StatusWid{background: #EBF5FF; border-radius: 4px;}");

		InitStatusWid(pStatusLayout);
		InitTempWid(pTempLayout);


		// 左下
		m_pDiskStatusInfo = new (std::nothrow) QFrame;
		m_pDiskStatusInfo->setObjectName("DiskStatusInfo");
		//m_pDiskStatusInfo->setAttribute(Qt::WA_TranslucentBackground);
		//m_pDiskStatusInfo->setMinimumHeight(442);
		m_pDiskStatusInfo->setObjectName("DiskStatusInfo");
		m_pDiskStatusInfo->setStyleSheet("QFrame#DiskStatusInfo{border:1px solid #F4F2F7; border-radius: 4px; background: #F2F4F7;}");

		QVBoxLayout* pDiskStatusLayout = new (std::nothrow) QVBoxLayout(m_pDiskStatusInfo);

		m_pDiskStatusTable = new (std::nothrow) CBPTableWidget;
		m_pDiskStatusTable->setObjectName("DiskStatusTable");
		pDiskStatusLayout->addWidget(m_pDiskStatusTable);
		m_pDiskStatusTable->setColumnCount(1);
		m_pDiskStatusTable->horizontalHeader()->setVisible(false);
		m_pDiskStatusTable->verticalHeader()->setVisible(false);
		m_pDiskStatusTable->setEditTriggers(QTableWidget::NoEditTriggers);
		m_pDiskStatusTable->setShowGrid(false);
		m_pDiskStatusTable->setAlternatingRowColors(false);
		m_pDiskStatusTable->setStyleSheet("QTableWidget#DiskStatusTable{border:none; background: transparent;}");

		pLeftWid->PackStart(pLeftTopWid);
		pLeftWid->AddSpacing(18);
		pLeftWid->PackStart(m_pDiskStatusInfo);

		// 右
		m_pAtrriList = new (std::nothrow) QTableView;
		m_pAtrriList->setFixedHeight(407);
		m_pAtrriList->setAlternatingRowColors(true);
		m_pAtrriList->setShowGrid(false);
		m_pAtrriList->setSelectionBehavior(QAbstractItemView::SelectRows);
		m_pAtrriList->verticalHeader()->setVisible(false);
		m_pAtrriList->horizontalHeader()->setStretchLastSection(true);
		m_pAtrriList->setEditTriggers(QAbstractItemView::NoEditTriggers);
		m_pAtrriList->setStyleSheet(
			"QTableView { border:1px solid #F4F2F7; background: transparent; border-radius: 4px;}"
			"QTableView::item {"
			"   background-color: #F2F4F7;"              // 奇数行背景色
			"}"
			"QTableView::item:alternate { background-color: #E6E9ED; }"
			"QHeaderView::section {"
			"   background-color: #E4EAF2;"              // 表头背景色
			"   border: none;"
			"   padding: 4px;"
			"}"
		);

		pContentWid->PackStart(pLeftWid, true, false, 0);
		pContentWid->AddSpacing(8);
		pContentWid->PackStart(m_pAtrriList, true, false, 0);

		AppendContentEx(pContentWid, true, false, 0);
		
		m_pVBoxMain->addSpacing(18);

		auto* pBottom = new (std::nothrow) CBPBox(false);
		pBottom->SetSpacing(0);
		pBottom->setAttribute(Qt::WA_TranslucentBackground);
		
		m_pAutoStartCheck = new CBPCheckBox;
		m_pAutoStartCheck->setText(tr("Display alert when disk problems are found. (Ask for autostart boot)"));
		pBottom->PackStart(m_pAutoStartCheck, true, false, 0);
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
		if (nullptr == m_pStatusText) break;
		//if (nullptr == m_pStatusPix) break;

		if (nullptr == m_pTempTxt) break;
		if (nullptr == m_pTempStatusTxt) break;
		
		m_pHealthPersentTxt->setText(tr("90%"));
		m_pStatusText->setText(tr("Good"));
		//m_pStatusPix->setPixmap(QPixmap(":/DiskHealth/img/healthy.png").scaled(24, 24, Qt::KeepAspectRatio, Qt::SmoothTransformation));
		

		m_pTempTxt->setText(tr("45c"));
		m_pTempStatusTxt->setText(tr("Normal"));
	


	} while (0);
}