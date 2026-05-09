#include "BPWinFrmTitle.h"
#include "BPImage.h"
#include "BPImageButton.h"
#include "BPLabel.h"
#include "Util.h"
#include "LangString.h"

#ifdef Q_OS_WIN
#include <windows.h>
#endif

#define Border_Right 16

const int c_iSpacing = 30;
const int c_iMainTitleLeftSpacing = 50;
const int c_iMainControlBarDefaultWidth = 222;

CBPWinFrmTitle::CBPWinFrmTitle(bool bMainWgt, QWidget *pParent /*= 0*/ )
:QWidget(pParent)
,m_pBindWnd(pParent)
,m_clrBg(Util::GetWindowTitleColor())
,m_bMainWgt(bMainWgt)
,m_iMainControlBarWidth(c_iMainControlBarDefaultWidth)
{
    setAttribute(Qt::WA_TranslucentBackground, false);
    Init();
}

void CBPWinFrmTitle::Init(void)
{
    do {
		m_pBtnSetting = CreateImgButton(QVector<QString>() << res("../res/dc6.0/memu_normal.png") << res("../res/dc6.0/memu_hover.png")
			<< res("../res/dc6.0/memu_press.png") << res("../res/dc6.0/memu_disable.png"), Btn_Setting, "Title_Setting");
		if (NULL == m_pBtnSetting) break;

        m_pBtnMin = CreateImgButton(QVector<QString>()<<res("../res/dc6.0/minimize_normal.png")<<res("../res/dc6.0/minimize_hover.png")
            <<res("../res/dc6.0/minimize_press.png")<<res("../res/dc6.0/minimize_disable.png"), Btn_Min, "Title_Min");
        if (NULL == m_pBtnMin) break;

        m_pBtnClose = CreateImgButton(QVector<QString>()<<res("../res/dc6.0/close_normal.png")<<res("../res/dc6.0/close_hover.png")
            <<res("../res/dc6.0/close_press.png")<<res("../res/dc6.0/close_disable.png"), Btn_Close, "Title_Close");
        if (NULL == m_pBtnClose) break;

        m_pHboxBtn = new QHBoxLayout;
        if (NULL == m_pHboxBtn) break;

		m_pHboxCustomBtns = new QHBoxLayout;
		if (NULL == m_pHboxCustomBtns) break;
        m_pHboxCustomBtns->setContentsMargins(0, 0, 0, 0);
		m_pHboxCustomBtns->setSpacing(16);
		m_pHboxCustomBtns->setAlignment(m_bMainWgt ? Qt::AlignVCenter : Qt::AlignBottom);

        m_pHboxBtn->setContentsMargins(0, 0, 0, (m_bMainWgt ? 0 : 3));
        m_pHboxBtn->setSpacing(16);
		m_pHboxBtn->addLayout(m_pHboxCustomBtns);
        m_pHboxBtn->addWidget(m_pBtnSetting, 0, (m_bMainWgt ? Qt::AlignVCenter : Qt::AlignBottom));
        m_pHboxBtn->addWidget(m_pBtnMin, 0, (m_bMainWgt ? Qt::AlignVCenter : Qt::AlignBottom));
        m_pHboxBtn->addWidget(m_pBtnClose, 0, (m_bMainWgt ? Qt::AlignVCenter : Qt::AlignBottom));

		m_pTitleLabel = new CBPLabel;
		if (NULL == m_pTitleLabel) break;
        m_pTitleLabel->setStyleSheet("QLabel{color:#1B1C1D;font-family: \"Segoe UI Semibold\";font-size: 14px;}");

        QHBoxLayout *pHboxMain = new QHBoxLayout;
        if (NULL == pHboxMain) break;
        pHboxMain->setSpacing(5);
        pHboxMain->setContentsMargins(Border_Right, 0, Border_Right, 0);
		pHboxMain->addWidget(m_pTitleLabel);
        pHboxMain->addSpacerItem(new QSpacerItem(0,0,QSizePolicy::Expanding,QSizePolicy::Minimum));
        pHboxMain->addLayout(m_pHboxBtn);
        setLayout(pHboxMain);
        setFixedHeight(m_bMainWgt ? Default_MainWgt_Hieght : Default_Hieght);

        InitMainControlBarWidth();
    }while (0);
}

void CBPWinFrmTitle::InitMainControlBarWidth(void)
{
    m_iMainControlBarWidth = 222;
}

void CBPWinFrmTitle::paintEvent( QPaintEvent *pEvent )
{
    QPainter paint(this);
    paint.setRenderHint(QPainter::Antialiasing);

	// 如果是透明背景，直接返回
	if (m_bIsTransparent || 0 == m_clrBg.alpha()) {
		return;
	}

    QRect drawRect = rect();
    paint.setPen(Qt::NoPen);
    paint.setBrush(QBrush(m_clrBg));

    int iRadius = m_bMainWgt ? c_iMainRadius : c_iRadius;
    int ix = drawRect.x() + (m_bMainWgt ? c_iMainTitleLeftSpacing : 0);
    int iy = drawRect.y();
    int iWidth = drawRect.width() - (m_bMainWgt ? c_iMainTitleLeftSpacing : 0);
    int iheight = drawRect.height();

    QPainterPath path;
    path.moveTo(ix, iy + iRadius);
    path.arcTo(ix, iy, iRadius * 2, iRadius * 2, 180, -90); //左上角圆弧
    path.lineTo(ix + iWidth - iRadius, iy); //顶边
    path.arcTo(ix + iWidth - iRadius * 2, iy, iRadius * 2, iRadius * 2, 90, -90); //右上角圆弧
    path.lineTo(ix + iWidth, iy + iheight); //右边
    path.lineTo(ix , iy + iheight); //底边
    path.lineTo(ix, iy + iRadius);//左边
    paint.drawPath(path);
}

int CBPWinFrmTitle::GetMainControlBarWidth(void)
{
    return m_iMainControlBarWidth;
}

void CBPWinFrmTitle::SetTitleText(const QString& strTitle)
{
	if (NULL != m_pTitleLabel) {
		m_pTitleLabel->setText(strTitle);
	}
}

void CBPWinFrmTitle::InsertCoustomButtons(const QList<CBPImageButton*>& listBtns, int index)
{
    do {
        if (listBtns.isEmpty() || nullptr == m_pHboxCustomBtns) break;

        for (const auto& pBtn : listBtns)
        {
            m_pHboxCustomBtns->insertWidget(index, pBtn);
        }
    } while (false);
}

void CBPWinFrmTitle::setTransparent(bool transparent)
{
	if (m_bIsTransparent != transparent) {
        m_bIsTransparent = transparent;
		setAttribute(Qt::WA_TranslucentBackground, transparent);
		update();
	}
}

void CBPWinFrmTitle::SetBgColor( QColor &Bgclr )
{
    m_clrBg = Bgclr;
}

void CBPWinFrmTitle::ShowTitleBtn( int iBtnid, bool bVisible /*= true*/ )
{
    if (iBtnid & Btn_Min){
        m_pBtnMin->setVisible(bVisible);
    }
    if (iBtnid & Btn_Close){
        m_pBtnClose->setVisible(bVisible);
    }
	if (iBtnid & Btn_Setting) {
		m_pBtnSetting->setVisible(bVisible);
	}
}

void CBPWinFrmTitle::EnableTitleBtn( int iBtnid, bool bEnable /*= true*/ )
{
    if (iBtnid & Btn_Min){
        m_pBtnMin->setVisible(TRUE);
        m_pBtnMin->setEnabled(bEnable);
    }
    if (iBtnid & Btn_Close){
        m_pBtnClose->setVisible(TRUE);
        m_pBtnClose->setEnabled(bEnable);
    }
	if (iBtnid & Btn_Setting) {
		m_pBtnSetting->setVisible(TRUE);
		m_pBtnSetting->setEnabled(bEnable);
	}
}


CBPImageButton * CBPWinFrmTitle::CreateImgButton( QVector<QString> &vecRes, int btnid, QString strAccessibleName)
{
    CBPImageButton *pBtn = new CBPImageButton(vecRes, btnid);
    if (NULL == pBtn) return NULL;

    pBtn->setFocusPolicy(Qt::NoFocus);
    pBtn->setVisible(false);//hide the button by default
    pBtn->setAccessibleName(strAccessibleName);
    connect(pBtn, &CBPImageButton::btn_released, this, [this](int id) {
		emit sigBtnClicked(id);
    });
    
    return pBtn;
}

void CBPWinFrmTitle::mousePressEvent( QMouseEvent *pEvent )
{
    do 
    {
        if (NULL == m_pBindWnd) break;
        if (NULL == pEvent) break;
        if (pEvent->button() != Qt::LeftButton) break;

#ifdef Q_OS_WIN
        // 使用 Win32 原生窗口拖拽，避免在 mouseMoveEvent 中高频调用
        // QWidget::move() 导致 WA_TranslucentBackground 分层窗口的
        // Qt backing store 渲染管线异常，拖拽后界面无法刷新
        ::ReleaseCapture();
        ::SendMessage((HWND)m_pBindWnd->winId(), WM_NCLBUTTONDOWN, HTCAPTION, 0);
#else
        m_bTitlePressed = true;
        m_ptRelative = pEvent->globalPos() - m_pBindWnd->pos();
#endif
        pEvent->accept();
        return;
       
    }while (0);

    QWidget::mousePressEvent(pEvent);
}

void CBPWinFrmTitle::mouseMoveEvent( QMouseEvent *pEvent )
{
    do 
    {
        if (NULL == m_pBindWnd) break;
        if (NULL == pEvent) break;

        if (m_bTitlePressed){
            m_pBindWnd->move(pEvent->globalPos() - m_ptRelative);
            pEvent->accept();
            return;
        }
    }while (0);

    QWidget::mouseMoveEvent(pEvent);
}

void CBPWinFrmTitle::mouseReleaseEvent( QMouseEvent *pEvent )
{
    if (m_bTitlePressed)
    {
        m_bTitlePressed = false;
        pEvent->accept();
        return;
    }
    m_bTitlePressed = false;
    QWidget::mouseReleaseEvent(pEvent);
}

CBPImageButton* CBPWinFrmTitle::titleBtn(int iBtnInd)
{
    CBPImageButton* pBtn = NULL;
    do {
        if (iBtnInd & Btn_Min){
            pBtn = m_pBtnMin;
        }
        if (iBtnInd & Btn_Close){
            pBtn = m_pBtnClose;
        }
		if (iBtnInd & Btn_Setting) {
			pBtn = m_pBtnSetting;
		}
    } while (0);

    return pBtn;
}
