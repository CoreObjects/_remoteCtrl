#include "pch.h"
#include "Controller.h"

CController::CController() :m_filePage(&m_mainDlg), m_watchDlg(&m_mainDlg) {
	while (!InitController());
	struct { UINT nMsg; MSGFUNC func; }MsgFuncs[] = {
		{WM_SET_IP_PORT,&CController::setIPandPort},
		{WM_GET_DRIVER ,&CController::getDriverInfo},
		{WM_TEST_CONNECT ,&CController::testConnect},
		{WM_GET_FILE_INFO ,&CController::getFileInfo},
		{WM_DOWNLOAD_FILE ,&CController::downLoadFile},
		{WM_DELET_FILE ,&CController::deleteFile},
		{WM_RUN_FILE ,&CController::runFile},
		{WM_MOUSE_EVENT ,&CController::MouseEvent},
		{WM_LOCK_MACHINE ,&CController::lockMachine},
		{WM_UNLOCK_MACHINE ,&CController::unlockMachine},
		{WM_SHOW_PIC,&CController::ShowPicture},
		{(UINT)-1,NULL}
	};
	for (int i = 0; MsgFuncs[i].func != NULL; i++) {
		m_mapFunc.insert(std::make_pair(MsgFuncs[i].nMsg, MsgFuncs[i].func));
	}
}

bool CController::InitController() {
	m_hThread = CreateThread(NULL, 0, CController::threadEntry, this, 0, &m_nThreadId);
	if (m_hThread == INVALID_HANDLE_VALUE) {
		return false;
	}
	else {
		return true;
	}
}

DWORD __stdcall CController::threadEntry(void* arg) {
	CController* thiz = (CController*)arg;
	thiz->threadFunc();
	return 0;
}

void CController::threadFunc() {
	MSG msg;
	while (::GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		if (msg.message == WM_SEND_MESSAGE) {
			MSGINFO* pMsg = (MSGINFO*)msg.wParam;
			HANDLE hEvent = (HANDLE)msg.lParam;
			auto it = m_mapFunc.find(pMsg->msg.message);
			if (it != m_mapFunc.end()) {

				pMsg->result = (this->*it->second)(pMsg->msg.message, pMsg->msg.wParam, pMsg->msg.lParam);
			}
			else {
				pMsg->result = -1;
			}
			SetEvent(hEvent);
		}
		else {
			auto it = m_mapFunc.find(msg.message);
			if (it != m_mapFunc.end()) {
				(this->*it->second)(msg.message, msg.wParam, msg.lParam);
			}
		}
	}
}

LRESULT CController::SendMessage(MSG msg) {
	HANDLE hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (hEvent == NULL) {
		return -2;
	}
	MSGINFO info(msg);
	PostThreadMessage(m_nThreadId, WM_SEND_MESSAGE, (WPARAM)&info, (LPARAM)hEvent);
	WaitForSingleObject(hEvent, -1);
	return info.result;
}

LRESULT CController::SendMessage(UINT message, WPARAM wParam, LPARAM lParam) {
	MSG msg;
	msg.message = message;
	msg.wParam = wParam;
	msg.lParam = lParam;
	return SendMessage(msg);
}

int CController::InvokeMainDlg(CWnd*& pMainWnd) {
	pMainWnd = &m_mainDlg;
	return m_mainDlg.DoModal();
}

LRESULT CController::InvokeWatchDlg() {
	m_hMutex = CreateMutex(NULL, FALSE, "123");
	DWORD dwThreadID;
	m_WatchThread = CreateThread(0, 0, CController::ThreadEntryForWatchData, this, 0, &dwThreadID);
	if (m_WatchThread == nullptr) {
		return -1;
	}
	m_isGetImage = true;
	m_watchDlg.DoModal();
	m_isGetImage = false;
	if (WaitForSingleObject(m_WatchThread, 500) == WAIT_TIMEOUT) {
		::TerminateThread(m_WatchThread, 0);
	}
	CloseHandle(m_WatchThread);
	CloseHandle(m_hMutex);
	return 0;
}

LRESULT CController::ShowPicture(UINT message, WPARAM wParam, LPARAM lParam) {
	if (m_isFull) {
		CRect rect;
		m_watchDlg.m_picture.GetWindowRect(rect);
		if (m_watchDlg.m_nObjWidth == -1) {
			m_watchDlg.m_nObjWidth = m_image.GetWidth();
		}
		if (m_watchDlg.m_nObjHeight == -1) {
			m_watchDlg.m_nObjHeight = m_image.GetHeight();
		}
		m_image.StretchBlt(m_watchDlg.m_picture.GetDC()->GetSafeHdc(), \
			0, 0, rect.Width(), rect.Height() - 50, SRCCOPY);
		m_image.Destroy();
		m_watchDlg.m_picture.InvalidateRect(NULL);
		m_isFull = false;
	}
	return true;
}

LRESULT CController::setIPandPort(UINT message, WPARAM wParam, LPARAM lParam/*std::string strIP, std::string strPort*/) {
	if (message != WM_SET_IP_PORT) {
		return -1;
	}
	LPCSTR szIP = (LPCSTR)wParam;
	LPCSTR szPort = (LPCSTR)lParam;
	m_model.setIPandPort(szIP, szPort);
	return 0;
}


LRESULT CController::testConnect(UINT message, WPARAM wParam, LPARAM lParam) {
	if (message != WM_TEST_CONNECT) {
		return -1;
	}
	return m_model.testConnect();
}

LRESULT CController::getDriverInfo(UINT message, WPARAM wParam, LPARAM lParam) {
	if (message != WM_GET_DRIVER) {
		return -1;
	}
	std::string strDriver;
	bool bRet = m_model.getDriverInfo(strDriver);
	if (bRet == false) {
		return -1;
	}
	std::string dr;
	for (char i : strDriver) {
		if (i == ',') {
			dr += ":";
			m_driverInfo.push_back(dr);
			dr.clear();
			continue;
		}
		dr += i;
	}
	dr.push_back(':');
	m_driverInfo.push_back(dr);
	m_filePage.PostMessageA(WM_SHOW_DIR, (WPARAM)&m_driverInfo, NULL);
	return true;
}

LRESULT CController::getFileInfo(UINT message, WPARAM wParam, LPARAM lParam) {
	m_strFilePath = (LPSTR)(wParam);
	m_model.getFileInfo(m_strFilePath, m_lstFileInfo);
	return (LRESULT)&m_lstFileInfo;
}


LRESULT CController::runFile(UINT message, WPARAM wParam, LPARAM lParam) {
	m_strFilePath = (LPSTR)(wParam);
	return m_model.runFile(m_strFilePath);
}

LRESULT CController::downLoadFile(UINT message, WPARAM wParam, LPARAM lParam) {
	m_strFilePath = (LPSTR)(wParam);
	m_strLocalFilePath = (LPSTR)(lParam);
	m_model.downLoadFile(m_strFilePath, m_strLocalFilePath);
	return true;
}

DWORD WINAPI CController::ThreadEntryForWatchData(void* arg) {
	CController* thiz = (CController*)arg;
	thiz->getScreen();
	return 0;
}

void CController::getScreen() {
	while (m_isGetImage) {
		if (m_isFull == false) {
			WaitForSingleObject(m_hMutex, -1);
			if (m_model.getScreen(m_image))
				m_isFull = true;
			ReleaseMutex(m_hMutex);
		}
		else {
			Sleep(5);
		}
	}
}

LRESULT CController::deleteFile(UINT message, WPARAM wParam, LPARAM lParam) {
	m_strFilePath = (LPSTR)(wParam);
	return m_model.deleteFile(m_strFilePath);
}

LRESULT CController::lockMachine(UINT message, WPARAM wParam, LPARAM lParam) {
	WaitForSingleObject(m_hMutex, -1);
	bool ret = m_model.lockMachine();
	ReleaseMutex(m_hMutex);
	return ret;
}

LRESULT CController::unlockMachine(UINT message, WPARAM wParam, LPARAM lParam) {
	WaitForSingleObject(m_hMutex, -1);
	bool ret = m_model.unlockMachine();
	ReleaseMutex(m_hMutex);
	return ret;
}

LRESULT CController::MouseEvent(UINT message, WPARAM wParam, LPARAM lParam) {
	MOUSEEV mouse;
	memcpy(&mouse, (char*)wParam, sizeof(MOUSEEV));
	WaitForSingleObject(m_hMutex, -1);
	bool ret = m_model.sendMouseEvent(mouse);
	ReleaseMutex(m_hMutex);
	return ret;
}

void CController::updataFileInfo(std::string szFilePath, std::string szLocalFilePath) {
	m_strFilePath = szFilePath;
	m_strLocalFilePath = szLocalFilePath;
}

CController::~CController() {
	TerminateThread(m_hThread, 0);
	CloseHandle(m_hThread);
}

