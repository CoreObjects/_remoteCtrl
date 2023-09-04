// FilePage.cpp: 实现文件
//

#include "pch.h"
#include "RemoteCtrlClient.h"
#include "FilePage.h"
#include "afxdialogex.h"


// CFilePage 对话框

IMPLEMENT_DYNAMIC(CFilePage, CDialogEx)

CFilePage::CFilePage(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_FILE, pParent) {

}

CFilePage::~CFilePage() {
}

void CFilePage::DoDataExchange(CDataExchange* pDX) {
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_DIR, m_Tree);
	DDX_Control(pDX, IDC_LIST_FILE, m_List);
}


BOOL CFilePage::OnInitDialog() {
	CDialogEx::OnInitDialog();

	return TRUE;

}

BEGIN_MESSAGE_MAP(CFilePage, CDialogEx)
	ON_NOTIFY(NM_CLICK, IDC_TREE_DIR, &CFilePage::OnNMClickTreeDir)
	ON_NOTIFY(NM_DBLCLK, IDC_TREE_DIR, &CFilePage::OnNMDblclkTreeDir)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_FILE, &CFilePage::OnNMRClickListFile)
	ON_COMMAND(ID_DOWNLOAD_FILE, &CFilePage::OnDownloadFile)
	ON_COMMAND(ID_DELET_FILE, &CFilePage::OnDeletFile)
	ON_COMMAND(ID_RUN_FILE, &CFilePage::OnRunFile)
	ON_MESSAGE(WM_SHOW_DIR, &CFilePage::OnShowDir)
END_MESSAGE_MAP()


// CFilePage 消息处理程序


void CFilePage::OnNMClickTreeDir(NMHDR* pNMHDR, LRESULT* pResult) {
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
	CPoint ptMouse;
	GetCursorPos(&ptMouse);
	m_Tree.ScreenToClient(&ptMouse);
	HTREEITEM hSelected = m_Tree.HitTest(ptMouse, 0);
	if (hSelected == NULL) {
		return;
	}
	CString strPath = GetFilePath(hSelected);
	DeleteTreeChildItem(hSelected);
	m_List.DeleteAllItems();
	MSG msg{ 0 };
	msg.message = WM_GET_FILE_INFO;
	msg.wParam = (WPARAM)strPath.GetBuffer();
	std::list<FILEINFO>* pFileList = (std::list<FILEINFO>*)\
		CController::getInstance().SendMessage(msg);
	for (const auto& fileInfo : *pFileList) {
		if (fileInfo.IsDirectory) {
			if (CString(fileInfo.szFileName) == "." || (CString)fileInfo.szFileName == "..") {
				continue;
			}
			HTREEITEM hTemp = m_Tree.InsertItem(fileInfo.szFileName, hSelected, TVI_LAST);
			m_Tree.InsertItem(NULL, hTemp, TVI_LAST);
		}
		else {
			m_List.InsertItem(0, fileInfo.szFileName);
		}
	}

}

void CFilePage::OnNMDblclkTreeDir(NMHDR* pNMHDR, LRESULT* pResult) {
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
	CPoint ptMouse;
	GetCursorPos(&ptMouse);
	m_Tree.ScreenToClient(&ptMouse);
	HTREEITEM hSelected = m_Tree.HitTest(ptMouse, 0);
	if (hSelected == NULL) {
		return;
	}
	CString strPath = GetFilePath(hSelected);
	DeleteTreeChildItem(hSelected);
	m_List.DeleteAllItems();
	MSG msg{ 0 };
	msg.message = WM_GET_FILE_INFO;
	msg.wParam = (WPARAM)strPath.GetBuffer();
	std::list<FILEINFO>* pFileList = (std::list<FILEINFO>*)\
		CController::getInstance().SendMessage(msg);
	for (const auto& fileInfo : *pFileList) {
		if (fileInfo.IsDirectory) {
			if (CString(fileInfo.szFileName) == "." || (CString)fileInfo.szFileName == "..") {
				continue;
			}
			HTREEITEM hTemp = m_Tree.InsertItem(fileInfo.szFileName, hSelected, TVI_LAST);
			m_Tree.InsertItem(NULL, hTemp, TVI_LAST);
		}
		else {
			m_List.InsertItem(0, fileInfo.szFileName);
		}
	}
}



void CFilePage::OnNMRClickListFile(NMHDR* pNMHDR, LRESULT* pResult) {
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
	HMENU hMenu = LoadMenu((HINSTANCE)GetWindowLongPtr(m_hWnd, GWLP_HINSTANCE), (LPCTSTR)MAKEINTRESOURCEW(IDR_MENU_FILE));
	HMENU hSubMenu = GetSubMenu(hMenu, 0);
	POINT pt;
	GetCursorPos(&pt);
	::TrackPopupMenu(hSubMenu, TPM_CENTERALIGN, pt.x, pt.y, 0, m_hWnd, NULL);
}


void CFilePage::OnDownloadFile() {
	// TODO: 在此添加命令处理程序代码
	HTREEITEM hSelected = m_Tree.GetSelectedItem();
	if (hSelected == NULL) {
		return;
	}
	CString strPath = GetFilePath(hSelected);
	int nItem = m_List.GetNextItem(-1, LVNI_SELECTED);
	CString strFile = m_List.GetItemText(nItem, 0); // 获取第0列（即第一列）的文本
	strPath += strFile;
	CFileDialog fdlg(FALSE, "*", strFile, OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY, NULL, this);
	INT_PTR fDlgRet = fdlg.DoModal();
	if (fDlgRet == IDOK) {
		MSG msg{ 0 };
		msg.message = WM_DOWNLOAD_FILE;
		msg.wParam = (WPARAM)strPath.GetBuffer();
		CString strLoaclFilePath = fdlg.GetPathName();
		msg.lParam = (LPARAM)strLoaclFilePath.GetBuffer();
		CController::getInstance().SendMessage(msg);
	}
}


void CFilePage::OnDeletFile() {
	// TODO: 在此添加命令处理程序代码
	HTREEITEM hSelected = m_Tree.GetSelectedItem();
	if (hSelected == NULL) {
		return;
	}
	CString strPath = GetFilePath(hSelected);
	int nItem = m_List.GetNextItem(-1, LVNI_SELECTED);
	CString strFile = m_List.GetItemText(nItem, 0); // 获取第0列（即第一列）的文本
	strPath += strFile;
	MSG msg{ 0 };
	msg.message = WM_DELET_FILE;
	msg.wParam = (WPARAM)strPath.GetBuffer();
	CController::getInstance().SendMessage(msg);
}


void CFilePage::OnRunFile() {
	// TODO: 在此添加命令处理程序代码
	HTREEITEM hSelected = m_Tree.GetSelectedItem();
	if (hSelected == NULL) {
		return;
	}
	CString strPath = GetFilePath(hSelected);
	int nItem = m_List.GetNextItem(-1, LVNI_SELECTED);
	CString strFile = m_List.GetItemText(nItem, 0); // 获取第0列（即第一列）的文本
	strPath += strFile;
	MSG msg{ 0 };
	msg.message = WM_RUN_FILE;
	msg.wParam = (WPARAM)strPath.GetBuffer();
	CController::getInstance().SendMessage(msg);
}


afx_msg LRESULT CFilePage::OnShowDir(WPARAM wParam, LPARAM lParam) {
	std::vector<std::string>* pdriverInfo = (std::vector<std::string>*)wParam;
	m_Tree.DeleteAllItems();
	for (auto i : *pdriverInfo) {
		HTREEITEM hTemp = m_Tree.InsertItem(i.c_str(), TVI_ROOT, TVI_LAST);
		//m_Tree.InsertItem(NULL, hTemp, TVI_LAST);
	}
	m_Tree.Select(NULL, TVGN_CARET);
	return 0;
}
