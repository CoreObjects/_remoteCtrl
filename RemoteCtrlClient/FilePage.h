#pragma once


// CFilePage 对话框
#define WM_SHOW_DIR (WM_USER + 100)
class CFilePage : public CDialogEx
{
	DECLARE_DYNAMIC(CFilePage)

public:
	CFilePage(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CFilePage();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_FILE };
#endif
private:
	CString GetFilePath(HTREEITEM hTree) {
		CString strRet, strTmp;
		do {
			strTmp = m_Tree.GetItemText(hTree);
			strRet = (strTmp + '\\' + strRet);
			hTree = m_Tree.GetParentItem(hTree);
		} while (hTree != NULL);
		return strRet;
	}
	void DeleteTreeChildItem(HTREEITEM hTree) {
		HTREEITEM hSub = nullptr;
		do {
			hSub = m_Tree.GetChildItem(hTree);
			if (hSub != NULL) {
				m_Tree.DeleteItem(hSub);
			}
		} while (hSub);
		//	m_Tree.DeleteItem(hTree);
	}
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	CTreeCtrl m_Tree;
	CListCtrl m_List;
	afx_msg void OnNMClickTreeDir(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNMDblclkTreeDir(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNMRClickListFile(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDownloadFile();
	afx_msg void OnDeletFile();
	afx_msg void OnRunFile();
protected:
	afx_msg LRESULT OnShowDir(WPARAM wParam, LPARAM lParam);
};
