#include "DiskHealthMainWin.h"

#include "../../../DiskCopy/mod.TBQtLib/BPLabel.h"
#include "../../../DiskCopy/mod.TBQtLib/BPBox.h"
#include "../../../DiskCopy/mod.TBQtLib/BPComboBox.h"
#include "../../../DiskCopy/mod.TBQtLib/BPPushBtn.h"
#include "../../../DiskCopy/mod.TBQtLib/BPTableWidget.h"
#include "../../../DiskCopy/mod.TBQtLib/BPCheckBox.h"
#include "../../../DiskCopy/mod.TBQtLib/BPImageButton.h"

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

	if (m_pTempWid) {
		// 安装过滤器，并将过滤器对象的父对象设为 m_pTempWid，保证生命周期
		m_pTempWid->installEventFilter(new TempWidgetClickFilter(this, m_pTempWid));
		m_pTempWid->setCursor(Qt::PointingHandCursor);
	}
	// 自动启动复选框状态改变
	connect(m_pAutoStartCheck, &CBPCheckBox::toggled, this, &CDiskHealthMainWin::onAutoStartToggled);
	// 完成按钮
	connect(m_pDoneBtn, &CBPPushBtn::clicked, this, &CDiskHealthMainWin::onDoneClicked);

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

		//// 设置自动启动复选框的状态
		//updateAutoStartState();
	}
}

void CDiskHealthMainWin::showLoading()
{
	if (!m_pLoadingOverlay) {
		// 创建透明遮罩，作为主窗口的子控件
		m_pLoadingOverlay = new QFrame(this);
		m_pLoadingOverlay->setAttribute(Qt::WA_TranslucentBackground);
		m_pLoadingOverlay->setStyleSheet("background:transparent; border:none;");
		m_pLoadingOverlay->setGeometry(rect());

		// 垂直+水平居中布局
		QVBoxLayout* layout = new QVBoxLayout(m_pLoadingOverlay);
		layout->setAlignment(Qt::AlignCenter);
		layout->setContentsMargins(0, 0, 0, 0);

		auto *waitLb = new CBPLabel(m_pLoadingOverlay);
		waitLb->setAlignment(Qt::AlignCenter);
		// 加载图片资源（请确保资源路径正确，根据实际项目可能为 ":/res/loading00.png"）
		QPixmap pixmap(":/res/loading00.png");
		if (!pixmap.isNull()) {
			waitLb->setPixmap(pixmap);
			waitLb->setScaledContents(false); // 保持原尺寸
		}
		
		layout->addWidget(waitLb);

		// 使遮罩拦截鼠标事件，避免刷新过程中用户误操作
		m_pLoadingOverlay->setAttribute(Qt::WA_TransparentForMouseEvents, false);
	}

	// 确保遮罩尺寸与当前窗口一致
	m_pLoadingOverlay->setGeometry(rect());
	m_pLoadingOverlay->raise();
	m_pLoadingOverlay->show();

	// 改变光标为等待状态，提升用户体验
	setCursor(Qt::WaitCursor);
}

void CDiskHealthMainWin::hideLoading()
{
	if (m_pLoadingOverlay) {
		m_pLoadingOverlay->hide();
	}
	// 恢复光标
	unsetCursor();
}

void CDiskHealthMainWin::onDiskSelected(int index)
{
	if (index < 0) return;

	updateDiskInfo(index);
	if (m_pDiskMgr) {
		m_pDiskMgr->selectDisk(index);

		QVariantMap status = m_pDiskMgr->getDiskStatus(index);
		if (status.isEmpty() || status["statusInfo"].toString().isEmpty()) {
			// 避免在刷新线程运行时再次触发刷新
			if (!m_pDiskMgr->isBusy())
				showLoading();         
				m_pDiskMgr->refreshDisks();
		}
	}
}

void CDiskHealthMainWin::onRefreshClicked()
{
	showLoading();
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
	loadDiskList();         
	hideLoading();                     
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
	// 温度单位变化，刷新当前磁盘的温度显示
	if (m_currentDiskIndex >= 0)
		updateStatusAndTemp(m_currentDiskIndex);
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
	if (!m_pDiskMgr) return;

	QVariantList disks = m_pDiskMgr->getDiskInfos();
	int newCount = disks.size();
	int oldCount = m_pDiskList->count();

	// 调整条目数量
	if (newCount > oldCount) {
		for (int i = oldCount; i < newCount; ++i)
			m_pDiskList->addItem("");
	}
	else if (newCount < oldCount) {
		for (int i = oldCount - 1; i >= newCount; --i)
			m_pDiskList->removeItem(i);
	}

	// 更新文本（仅文本变化，不改变 currentIndex）
	for (int i = 0; i < newCount; ++i) {
		QString text = stripHtml(disks[i].toString());
		if (m_pDiskList->itemText(i) != text)
			m_pDiskList->setItemText(i, text);
	}

	// 如果原来没有选中项且有可用磁盘，默认选中第一个
	if (m_pDiskList->count() > 0 && m_pDiskList->currentIndex() == -1)
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
	if (!m_pDiskStatusInfoLayout) return;

	while (QLayoutItem* item = m_pDiskStatusInfoLayout->takeAt(0)) {
		if (QWidget* widget = item->widget()) {
			delete widget;         
		}
		
		delete item;               
	}

	for (int i = 0; i < infoList.size(); ++i) {
		QVariantMap item = infoList[i].toMap();
		QString key = item["arg"].toString();
		QString value = item["value"].toString();
		CBPLabel *pKey = new CBPLabel(key);
		pKey->setStyleSheet("font-family: \"Segoe UI\"; font-size: 14px; color: #6F7884;");
		m_pDiskStatusInfoLayout->addWidget(pKey);

		CBPLabel* pValue = new CBPLabel(value);
		pValue->setStyleSheet("font-family: \"Segoe UI\"; font-size: 14px; font-weight:600; color: #1F2C40;");
		m_pDiskStatusInfoLayout->addWidget(pValue);
	}
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
		m_pAtrriList->setRowHeight(row, 30);
	}

	// 设置模型到 QTableView
	m_pAtrriList->setModel(pViewModel);

	// 调整列宽和样式
	m_pAtrriList->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
	m_pAtrriList->resizeColumnToContents(0);
	m_pAtrriList->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
	m_pAtrriList->resizeColumnToContents(1);
	m_pAtrriList->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
	m_pAtrriList->resizeColumnToContents(2);
	m_pAtrriList->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
	/*if (!rawMode)
	{
		m_pAtrriList->resizeColumnToContents(4);
	}*/
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

void CDiskHealthMainWin::InitStatusWid(QBoxLayout* pStatusLay)
{
	do {
		if (pStatusLay == nullptr) break;
		
		QVBoxLayout* pLeft = new(std::nothrow) QVBoxLayout;
		pLeft->setContentsMargins(5, 5, 5, 5);
		pLeft->setSpacing(5);

		auto* pTitleLb = new (std::nothrow) CBPLabel;
		pTitleLb->setText(tr("Status"));
		pTitleLb->setStyleSheet("font-family: \"Segoe UI\"; font-size: 14px; color: #6F7884;");
		m_pStatusText = new (std::nothrow) CBPLabel;

		m_pHealthPersentTxt = new (std::nothrow) CBPLabel;
		m_pHealthPersentTxt->setStyleSheet("font-family: \"Segoe UI\"; font-size: 14px; font-weight:600; color: #1F2C40;");

		pLeft->addWidget(pTitleLb);
		pLeft->addWidget(m_pStatusText);
		pLeft->addWidget(m_pHealthPersentTxt);

		pStatusLay->addLayout(pLeft);
	} while (0);

}

void CDiskHealthMainWin::InitTempWid(QBoxLayout* pTempLay)
{
	do {
		if (pTempLay == nullptr) break;
	
		auto* pLeft = new(std::nothrow) QVBoxLayout;
		pLeft->setContentsMargins(5, 5, 5, 5);
		pLeft->setSpacing(5);

		auto* pTitleLb = new (std::nothrow) CBPLabel;
		pTitleLb->setText(tr("Temperature"));
		pTitleLb->setStyleSheet("font-family: \"Segoe UI\"; font-size: 14px; color: #6F7884;");
		m_pTempTxt = new (std::nothrow) CBPLabel;
		m_pTempStatusTxt = new (std::nothrow) CBPLabel;
		m_pTempStatusTxt->setStyleSheet("font-family: \"Segoe UI\"; font-size: 14px; font-weight:600; color: #1F2C40;");

		pLeft->addWidget(pTitleLb);
		pLeft->addWidget(m_pTempTxt);
		pLeft->addWidget(m_pTempStatusTxt);

		pTempLay->addLayout(pLeft);

	} while (0);
}


void CDiskHealthMainWin::Init()
{
	do {
		InitCommonWnd(true, 4, 0, 0);
		SetMainBoxBorder(18, 8, 18, 10);
		InitTitle();
		setFixedSize(848, 606);
		setObjectName("CDiskHealthMainWin");
		setStyleSheet("#CDiskHealthMainWin{border:2px solid #F4F2F7; background: #FFFFFF; border-radius: 4px;}");

		m_pLeftLb = new(std::nothrow) CBPLabel;
		m_pLeftLb->setWordWrap(true);
		m_pLeftLb->setText(tr("Which disk do you care about?"));
		m_pLeftLb->setStyleSheet("font-family: \"Segoe UI\"; font-size: 14px; font-weight:600; color: #1F2C40;");
		AppendContentEx(m_pLeftLb, false, 0, false, Qt::AlignTop);
		
		m_pVBoxMain->addSpacing(5);
		m_pTopWid = new(std::nothrow) CBPBox(false);
		m_pTopWid->SetSpacing(0);
		m_pTopWid->setAttribute(Qt::WA_TranslucentBackground);
	
		m_pDiskList = new(std::nothrow) CBPComboBox;
		m_pDiskList->setObjectName("DiskList");
		m_pDiskList->setStyleSheet("#DiskList{border:2px solid #F4F2F7; background: #EBEEF2; border-radius: 4px; font-family: \"Segoe UI\"; font-size: 14px;}");


		m_pTopWid->PackStart(m_pDiskList, true, true, 0);
		m_pTopWid->AddSpacing(8);

		m_pRefreshBtn = new(std::nothrow) QPushButton;

		QString style = R"(
			QPushButton {
				background-image: url(:/res/icon_button_refresh_blue18.png);
				background-repeat: no-repeat;
				background-position: center;
				background-color: #E0EAFC;    
				border: 2px;    
				border-radius: 6px;             
			}

			QPushButton:hover {
				background-color: #C5D5F3;    
				border: 2px;       
			}
			QPushButton:pressed {
				background-color: #A6BDE6;    
				border: 2px;         
			}
		)";

		m_pRefreshBtn->setStyleSheet(style);

		m_pRefreshBtn->setFixedSize(62, 38);
		m_pTopWid->PackStart(m_pRefreshBtn, false, false);

		AppendContentEx(m_pTopWid, false, 0, false, Qt::AlignTop);

		m_pVBoxMain->addSpacing(5);
		
		auto pContentWid = new(std::nothrow) CBPBox(false);
		pContentWid->SetSpacing(0);
		pContentWid->setAttribute(Qt::WA_TranslucentBackground);

		auto* pLeftWid = new (std::nothrow) CBPBox(true);
		pLeftWid->setFixedWidth(286);
		pLeftWid->SetSpacing(0);
		pLeftWid->setAttribute(Qt::WA_TranslucentBackground);

		auto* pLeftTopWid = new (std::nothrow) CBPBox(false);
		pLeftTopWid->SetSpacing(0);
		pLeftTopWid->setAttribute(Qt::WA_TranslucentBackground);

		// 状态
		m_pStatusWid = new (std::nothrow) QFrame;
		m_pStatusWid->setFixedSize(140, 70);
		QHBoxLayout* pStatusLayout = new (std::nothrow) QHBoxLayout(m_pStatusWid);
		pStatusLayout->setSpacing(0);
		pStatusLayout->setContentsMargins(0, 0, 0, 0);
		m_pStatusWid->setObjectName("StatusWid");
		pLeftTopWid->PackStart(m_pStatusWid, false, false, 0);

		pLeftTopWid->AddSpacing(5);

		// 温度
		m_pTempWid = new (std::nothrow) QFrame;
		m_pTempWid->setFixedSize(140, 70);
		QHBoxLayout* pTempLayout = new (std::nothrow) QHBoxLayout(m_pTempWid);
		pTempLayout->setSpacing(0);
		pTempLayout->setContentsMargins(0, 0, 0, 0);
		m_pTempWid->setObjectName("TempWid");
		pLeftTopWid->PackStart(m_pTempWid, false, false, 0);

		InitStatusWid(pStatusLayout);
		InitTempWid(pTempLayout);

		// 磁盘状态信息
		m_pDiskStatusInfo = new (std::nothrow) QFrame;
		//m_pDiskStatusInfo->setMaximumHeight(440);
		m_pDiskStatusInfo->setObjectName("DiskStatusInfo");
		m_pDiskStatusInfo->setStyleSheet("QFrame#DiskStatusInfo{border:1px solid #F4F2F7; border-radius: 4px; background: #F2F4F7;}");

		m_pDiskStatusInfoLayout = new (std::nothrow) QVBoxLayout(m_pDiskStatusInfo);

		pLeftWid->PackStart(pLeftTopWid);
		pLeftWid->AddSpacing(5);
		pLeftWid->PackStart(m_pDiskStatusInfo);

		// 属性列表
		m_pAtrriList = new (std::nothrow) QTableView;
		m_pAtrriList->setAlternatingRowColors(true);
		m_pAtrriList->setShowGrid(false);
		m_pAtrriList->setSelectionBehavior(QAbstractItemView::SelectRows);
		m_pAtrriList->verticalHeader()->setVisible(false);
		m_pAtrriList->horizontalHeader()->setStretchLastSection(true);
		m_pAtrriList->setEditTriggers(QAbstractItemView::NoEditTriggers);
		m_pAtrriList->setStyleSheet(
			"QTableView { border:1px solid #F4F2F7; background: transparent; border-radius: 4px;}"
			"QTableView::item {"
			"   background-color: #F2F4F7;"
			"   color:#1F2C40;"
			"	font-family:\"Segoe UI\";"
			"	font-size:14px;"
			"   font-weight:600;"
			"}"
			"QTableView::item:alternate { background-color: #E6E9ED; }"
			"QHeaderView::section {"
			"   background-color: #E4EAF2;"              
			"   border: none;"
			"   padding: 4px;"
			"   color:#6F7884;"
			"	font-family:\"Segoe UI\";"
			"	font-size: 14px;"
			"}"
		);

		m_pAtrriList->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft| Qt::AlignVCenter);
		m_pAtrriList->horizontalHeader()->setFixedHeight(30);   
		
		m_pAtrriList->setSelectionMode(QAbstractItemView::NoSelection);

		pContentWid->PackStart(pLeftWid, true, false, 0);
		pContentWid->AddSpacing(8);
		pContentWid->PackStart(m_pAtrriList, true, false, 0);

		AppendContentEx(pContentWid, false, false, 0);
		
		m_pVBoxMain->addSpacing(5);

		auto* pBottom = new (std::nothrow) QFrame;
		QHBoxLayout* pBottomLayout = new (std::nothrow) QHBoxLayout(pBottom);
		pBottomLayout->setSpacing(0);
		pBottomLayout->setContentsMargins(0, 0, 0, 0);
		pBottom->setAttribute(Qt::WA_TranslucentBackground);
		
		m_pAutoStartCheck = new CBPCheckBox;
		m_pAutoStartCheck->setText(" ");
		pBottomLayout->addWidget(m_pAutoStartCheck);
		
		auto* pLb = new CBPLabel;
		// 富文本标签
		pLb->setTextFormat(Qt::RichText); // 启用富文本
		pLb->setText("Display alert when disk problems are found "
			"(<font color='orange'>Ask for automatic boot</font>)");

		//// 点击标签也能触发复选框状态
		//connect(pLb, &CBPLabel::linkActivated, [=]() {
		//	m_pAutoStartCheck->setChecked(!m_pAutoStartCheck->isChecked());
		//	});

		pBottomLayout->addWidget(pLb);
		pBottomLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));
		
		m_pDoneBtn = new(std::nothrow) CBPPushBtn;
		m_pDoneBtn->setFixedSize(130, 37);
		m_pDoneBtn->setText(tr("Done"));
		pBottomLayout->addWidget(m_pDoneBtn);
		
		AppendContentEx(pBottom, false, false, 0);


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

void CDiskHealthMainWin::updateAutoStartState()
{
	if (!m_pDiskMgr || !m_pAutoStartCheck) return;
	bool bAutoStart = m_pDiskMgr->isAutoStart();
	m_pAutoStartCheck->setChecked(bAutoStart);
}