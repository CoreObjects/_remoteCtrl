
// RemoteCtrlClientDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "RemoteCtrlClient.h"
#include "RemoteCtrlClientDlg.h"
#include "afxdialogex.h"
#include "Controller.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CRemoteCtrlClientDlg 对话框



CRemoteCtrlClientDlg::CRemoteCtrlClientDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_REMOTECTRLCLIENT_DIALOG, pParent)
	, m_strPort(_T("")) {
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CRemoteCtrlClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IPADDRESS1, m_IPAddressCtrl);
	DDX_Text(pDX, IDC_EDIT1, m_strPort);
	DDX_Control(pDX, IDC_TAB1, m_Tab);
}

void CRemoteCtrlClientDlg::UpdateIP() {
	DWORD dwIP;
	m_IPAddressCtrl.GetAddress(dwIP);
	BYTE nField0, nField1, nField2, nField3;
	nField0 = FIRST_IPADDRESS(dwIP);
	nField1 = SECOND_IPADDRESS(dwIP);
	nField2 = THIRD_IPADDRESS(dwIP);
	nField3 = FOURTH_IPADDRESS(dwIP);
	m_strIP.Format(_T("%d.%d.%d.%d"), nField0, nField1, nField2, nField3);
}

void CRemoteCtrlClientDlg::SetIPCtrl(const CString& strIP) {
	int nField0, nField1, nField2, nField3;
	_stscanf_s(strIP, _T("%d.%d.%d.%d"), &nField0, &nField1, &nField2, &nField3);

	DWORD dwIP = MAKEIPADDRESS(nField0, nField1, nField2, nField3);
	m_IPAddressCtrl.SetAddress(dwIP);
}

BEGIN_MESSAGE_MAP(CRemoteCtrlClientDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(IPN_FIELDCHANGED, IDC_IPADDRESS1, &CRemoteCtrlClientDlg::OnIpnFieldchangedIpaddress1)
	ON_EN_CHANGE(IDC_EDIT1, &CRemoteCtrlClientDlg::OnEnChangeEdit1)
	ON_BN_CLICKED(IDC_BUTTON_TEST, &CRemoteCtrlClientDlg::OnBnClickedButtonTest)
	ON_BN_CLICKED(IDC_BUTTON2, &CRemoteCtrlClientDlg::OnBnClickedButtonWatchDlg)
	ON_BN_CLICKED(IDC_BUTTON3, &CRemoteCtrlClientDlg::OnBnClickedButtonGetFileDriver)
END_MESSAGE_MAP()


// CRemoteCtrlClientDlg 消息处理程序

BOOL CRemoteCtrlClientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	m_strPort = "27015";
	CString strIP = _T("127.0.0.1");
//	CString strIP = _T("192.168.225.138");
	MSG msg;
	msg.message = WM_SET_IP_PORT;
	msg.wParam = (WPARAM)strIP.GetBuffer();
	msg.lParam = (LPARAM)m_strPort.GetBuffer();
	CController::getInstance().SendMessage(msg);
	SetIPCtrl(strIP);
	UpdateData(FALSE);
	m_Tab.InsertItem(0, "文件");
	m_Tab.InsertItem(1, "进程");
	m_Tab.InsertItem(2, "软件");
	m_Tab.InsertItem(3, "开机启动项");

	CController::getInstance().InitAllWnd(&m_Tab);
	
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CRemoteCtrlClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CRemoteCtrlClientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CRemoteCtrlClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CRemoteCtrlClientDlg::OnIpnFieldchangedIpaddress1(NMHDR* pNMHDR, LRESULT* pResult) {
	LPNMIPADDRESS pIPAddr = reinterpret_cast<LPNMIPADDRESS>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
	UpdateIP();
	CController::getInstance()\
		.SendMessage(WM_SET_IP_PORT, (WPARAM)m_strIP.GetBuffer(), (LPARAM)m_strPort.GetBuffer());
	//g_Ctrl.SendMessage(msg);
}


void CRemoteCtrlClientDlg::OnEnChangeEdit1() {
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。
	UpdateData();
	MSG msg;
	msg.message = WM_SET_IP_PORT;
	msg.wParam = (WPARAM)m_strIP.GetBuffer();
	msg.lParam = (LPARAM)m_strPort.GetBuffer();
	CController::getInstance().SendMessage(msg);
	// TODO:  在此添加控件通知处理程序代码
}


void CRemoteCtrlClientDlg::OnBnClickedButtonTest() {
	// TODO: 在此添加控件通知处理程序代码
	MSG msg{ 0 };
	msg.message = WM_TEST_CONNECT;
	if (CController::getInstance().SendMessage(msg)) {
		AfxMessageBox("连接成功");
	}
	else {
		AfxMessageBox("连接失败");
	}
}


void CRemoteCtrlClientDlg::OnBnClickedButtonWatchDlg() {
	// TODO: 在此添加控件通知处理程序代码
	CController::getInstance().InvokeWatchDlg();
}


void CRemoteCtrlClientDlg::OnBnClickedButtonGetFileDriver() {
	// TODO: 在此添加控件通知处理程序代码
	MSG msg{ 0 };
	msg.message = WM_GET_DRIVER;
	CController::getInstance().SendMessage(msg);
}
