#pragma once
#include "Model.h"
#include "FilePage.h"
#include "RemoteCtrlClientDlg.h"
#include "WatchDialog.h"
#include "resource.h"
#define WM_SET_IP_PORT (WM_USER +1)
#define WM_TEST_CONNECT (WM_USER +2)
#define WM_GET_DRIVER (WM_USER + 3)
#define WM_GET_FILE_INFO (WM_USER + 4)
#define WM_DOWNLOAD_FILE (WM_USER + 5)
#define WM_DELET_FILE (WM_USER + 6)
#define WM_RUN_FILE (WM_USER + 7)
#define WM_MOUSE_EVENT (WM_USER + 8)
#define WM_LOCK_MACHINE (WM_USER + 9)
#define WM_UNLOCK_MACHINE (WM_USER + 10)
#define WM_SHOW_PIC (WM_USER + 11)
#define WM_SEND_MESSAGE (WM_USER + 0x1000)


class CController {
	typedef LRESULT(CController::* MSGFUNC)(UINT, WPARAM, LPARAM);
	std::unordered_map<UINT, MSGFUNC>m_mapFunc;

private:
	bool InitController();
	static DWORD __stdcall threadEntry(void* arg);
	void threadFunc();
private:
	HANDLE m_hThread;
	DWORD m_nThreadId;
	HANDLE m_WatchThread;
	CRemoteCtrlClientDlg m_mainDlg;
 	CFilePage m_filePage;
	CWatchDialog m_watchDlg;
	HANDLE m_hMutex;
public:
	static CController& getInstance() {
		static CController instance;
		return instance;
	}
	LRESULT SendMessage(MSG msg);
	LRESULT SendMessage(UINT message, WPARAM wParam, LPARAM lParam);
	int InvokeMainDlg(CWnd*& pMainWnd);
	bool InitAllWnd(CTabCtrl* pTab) {
		m_filePage.Create(IDD_DIALOG_FILE, pTab);
		CRect rs;
		pTab->GetClientRect(rs);
		rs.top += 23;
		m_filePage.MoveWindow(rs);
		m_filePage.ShowWindow(SW_SHOW);
		return true;
	}
	LRESULT InvokeWatchDlg();
	
	
private:
	LRESULT setIPandPort(UINT message, WPARAM wParam, LPARAM lParam/*std::string strIP, std::string strPort*/);
	LRESULT testConnect(UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT getDriverInfo(UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT getFileInfo(UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT runFile(UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT downLoadFile(UINT message, WPARAM wParam, LPARAM lParam);
	static DWORD WINAPI ThreadEntryForWatchData(void* arg);
	void getScreen();
	LRESULT ShowPicture(UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT deleteFile(UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT lockMachine(UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT unlockMachine(UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT MouseEvent(UINT message, WPARAM wParam, LPARAM lParam);
	void updataFileInfo(std::string m_strFilePath, std::string m_strLocalFilePath);
	std::vector<std::string> m_driverInfo;
	std::list<FILEINFO> m_lstFileInfo;
	std::string m_strFilePath;
	std::string m_strLocalFilePath;
	CImage m_image;
	bool m_isGetImage = false;
	bool m_isFull = false;
private:
	typedef struct MsgInfo {
		MSG msg;
		LRESULT result;
		MsgInfo(MSG m) {
			result = 0;
			memcpy(&msg, &m, sizeof(MSG));
		}
		MsgInfo(const MsgInfo& m) {
			result = m.result;
			memcpy(&msg, &m.msg, sizeof(MSG));
		}
		MsgInfo& operator=(const MsgInfo& m) {
			if (this != &m) {
				result = m.result;
				memcpy(&msg, &m.msg, sizeof(MSG));
			}
			return *this;
		}
	}MSGINFO;
	CController();
	CController(CController const&) = delete;
	void operator=(CController const&) = delete;
	~CController();
	CModel& m_model = CModel::getInstance();

};
/*extern CController& g_Ctrl;*/
