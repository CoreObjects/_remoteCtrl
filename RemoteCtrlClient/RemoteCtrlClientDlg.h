
// RemoteCtrlClientDlg.h: 头文件
//

#pragma once

// CRemoteCtrlClientDlg 对话框
class CRemoteCtrlClientDlg : public CDialogEx
{
// 构造
public:
	CRemoteCtrlClientDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_REMOTECTRLCLIENT_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
private:
	void UpdateIP();
	void SetIPCtrl(const CString&);
// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnIpnFieldchangedIpaddress1(NMHDR* pNMHDR, LRESULT* pResult);
	CIPAddressCtrl m_IPAddressCtrl;
	afx_msg void OnEnChangeEdit1();
	CString m_strPort;
	CString m_strIP;
	afx_msg void OnBnClickedButtonTest();
	CTabCtrl m_Tab;
	afx_msg void OnBnClickedButtonWatchDlg();
	afx_msg void OnBnClickedButtonGetFileDriver();
};
