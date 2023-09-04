// LockDlg.cpp: 实现文件
//

#include "pch.h"
#include "LockDlg.h"
#include "afxdialogex.h"


// CLockDlg 对话框

IMPLEMENT_DYNAMIC(CLockDlg, CDialogEx)

CLockDlg::CLockDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG1, pParent)
{

}

CLockDlg::~CLockDlg()
{
}

void CLockDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CLockDlg, CDialogEx)
END_MESSAGE_MAP()


// CLockDlg 消息处理程序
