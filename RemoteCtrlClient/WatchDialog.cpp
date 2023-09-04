// WatchDialog.cpp: 实现文件
//

#include "pch.h"
#include "RemoteCtrlClient.h"
#include "WatchDialog.h"
#include "afxdialogex.h"


// CWatchDialog 对话框

IMPLEMENT_DYNAMIC(CWatchDialog, CDialogEx)

CWatchDialog::CWatchDialog(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_WATCH, pParent)
{

}

CWatchDialog::~CWatchDialog()
{
}

void CWatchDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_WATCH, m_picture);
}


BEGIN_MESSAGE_MAP(CWatchDialog, CDialogEx)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON1, &CWatchDialog::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CWatchDialog::OnBnClickedButton2)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()


// CWatchDialog 消息处理程序


void CWatchDialog::OnTimer(UINT_PTR nIDEvent) {
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CController::getInstance().SendMessage(WM_SHOW_PIC, 0, 0);
	CDialogEx::OnTimer(nIDEvent);
}


CPoint CWatchDialog::UserPoint2RemoteScreenPoint(CPoint& point, bool isScreen /*= false*/) {
	CPoint cur = point;
	CRect clientRect;
	if (isScreen)ScreenToClient(&point);
	m_picture.GetWindowRect(clientRect);
	int width0 = clientRect.Width();
	int height0 = clientRect.Height() - 50;
	int width = m_nObjWidth;
	int heigh = m_nObjHeight;
	int x = point.x * width / width0;
	int y = (point.y - 50) * heigh / height0;
	return CPoint(x, y);
}

BOOL CWatchDialog::OnInitDialog() {
	CDialogEx::OnInitDialog();
	// TODO:  在此添加额外的初始化
	m_nObjHeight = m_nObjWidth = -1;
	SetTimer(0, 50, NULL);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CWatchDialog::OnBnClickedButton1() {
	// TODO: 在此添加控件通知处理程序代码
	CController::getInstance().SendMessage(WM_LOCK_MACHINE, 0, 0);
}


void CWatchDialog::OnBnClickedButton2() {
	// TODO: 在此添加控件通知处理程序代码
	CController::getInstance().SendMessage(WM_UNLOCK_MACHINE, 0, 0);
}


void CWatchDialog::OnLButtonDblClk(UINT nFlags, CPoint point) {
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (m_nObjHeight != -1 && m_nObjWidth != -1) {
		CPoint remote = UserPoint2RemoteScreenPoint(point);
		MOUSEEV mevent;
		mevent.ptXY = remote;
		mevent.nButton = 0;//左键
		mevent.nAction = 1;//双击
		CController::getInstance().SendMessage(WM_MOUSE_EVENT, (WPARAM)&mevent, NULL);
	}
	CDialogEx::OnLButtonDblClk(nFlags, point);
}


void CWatchDialog::OnLButtonDown(UINT nFlags, CPoint point) {
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (m_nObjHeight != -1 && m_nObjWidth != -1) {
		CPoint remote = UserPoint2RemoteScreenPoint(point);
		MOUSEEV mevent;
		mevent.ptXY = remote;
		mevent.nButton = 0;//左键
		mevent.nAction = 2;//按下
		CController::getInstance().SendMessage(WM_MOUSE_EVENT, (WPARAM)&mevent, NULL);
	}
	CDialogEx::OnLButtonDown(nFlags, point);
}


void CWatchDialog::OnLButtonUp(UINT nFlags, CPoint point) {
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (m_nObjHeight != -1 && m_nObjWidth != -1) {
		CPoint remote = UserPoint2RemoteScreenPoint(point);
		MOUSEEV mevent;
		mevent.ptXY = remote;
		mevent.nButton = 0;//左键
		mevent.nAction = 3;//抬起
		CController::getInstance().SendMessage(WM_MOUSE_EVENT, (WPARAM)&mevent, NULL);
	}
	CDialogEx::OnLButtonUp(nFlags, point);
}


void CWatchDialog::OnRButtonDblClk(UINT nFlags, CPoint point) {
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (m_nObjHeight != -1 && m_nObjWidth != -1) {
		CPoint remote = UserPoint2RemoteScreenPoint(point);
		MOUSEEV mevent;
		mevent.ptXY = remote;
		mevent.nButton = 1;//左键
		mevent.nAction = 1;//抬起
		CController::getInstance().SendMessage(WM_MOUSE_EVENT, (WPARAM)&mevent, NULL);
	}
	CDialogEx::OnRButtonDblClk(nFlags, point);
}


void CWatchDialog::OnRButtonDown(UINT nFlags, CPoint point) {
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (m_nObjHeight != -1 && m_nObjWidth != -1) {
		CPoint remote = UserPoint2RemoteScreenPoint(point);
		MOUSEEV mevent;
		mevent.ptXY = remote;
		mevent.nButton = 1;//左键
		mevent.nAction = 2;//抬起
		CController::getInstance().SendMessage(WM_MOUSE_EVENT, (WPARAM)&mevent, NULL);
	}
	CDialogEx::OnRButtonDown(nFlags, point);
}


void CWatchDialog::OnRButtonUp(UINT nFlags, CPoint point) {
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (m_nObjHeight != -1 && m_nObjWidth != -1) {
		CPoint remote = UserPoint2RemoteScreenPoint(point);
		MOUSEEV mevent;
		mevent.ptXY = remote;
		mevent.nButton = 1;//左键
		mevent.nAction = 3;//抬起
		CController::getInstance().SendMessage(WM_MOUSE_EVENT, (WPARAM)&mevent, NULL);
	}
	CDialogEx::OnRButtonUp(nFlags, point);
}
