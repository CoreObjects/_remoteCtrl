#include "pch.h"
#include "Command.h"
CCommand& g_myCmd = CCommand::getInstance();
int CCommand::SendDirverInfo(const CPacket& packet, std::queue<CPacket>& sendPacketQueue) {
	if (packet.getCommand() != 1) {
		return -1;
	}
	std::string result;
	for (int i = 1; i <= 26; i++) {
		if (_chdrive(i) == 0) {
			if (result.size() > 0) result += ',';
			result += 'A' + i - 1;
		}
	}
	CPacket packet2Client(1, result.c_str(), result.size());
	sendPacketQueue.push(packet2Client);

	return 0;
}
int CCommand::SendDirectoryInfo(const CPacket& packet, std::queue<CPacket>& sendPacketQueue) {
	if (packet.getCommand() != 2) {
		return -1;
	}
	typedef struct file_info {
		file_info() {
			IsInvalid = 0;
			IsDirectory = -1;
			HasNext = TRUE;
			memset(szFileName, 0, MAX_PATH);
		}
		BOOL IsInvalid;//是否无效
		BOOL IsDirectory;
		BOOL HasNext;//是否还有后续0没有，1有
		char szFileName[MAX_PATH];
	}FILEINFO, * PFILEINFO;
	std::string strPath(packet.getPacketSize(), 0);
	memcpy((void*)strPath.c_str(), packet.getContent().data(), packet.getContent().size());
	if (_chdir(strPath.c_str()) != 0) {
		FILEINFO fInfo;
		fInfo.IsInvalid = TRUE;
		fInfo.IsDirectory = TRUE;
		fInfo.HasNext = FALSE;
		memcpy(fInfo.szFileName, strPath.c_str(), strPath.size());
		/*lstFileInfos.push_back(fInfo);*/
		CPacket packet2Client(2, (const char*)&fInfo, sizeof(fInfo));
		std::cout << fInfo.szFileName << "  " << "hasnext" << fInfo.HasNext << std::endl;
		sendPacketQueue.push(packet2Client);
		OutputDebugString(L"没有权限，访问目录！！！");
		return -2;
	}
	_finddata_t fdata;
	int hfind = 0;

	if ((hfind = _findfirst("*", &fdata)) == -1) {
		OutputDebugString(L"没有找到任何文件！！！");
		FILEINFO fInfo;
		fInfo.HasNext = FALSE;
		CPacket packet2Client(2, (const char*)&fInfo, sizeof(fInfo));
		std::cout << fInfo.szFileName << "  " << "hasnext" << fInfo.HasNext << std::endl;
		sendPacketQueue.push(packet2Client);
		return -3;
	}
	int nCount = 0;
	do {
		FILEINFO fInfo;
		fInfo.IsDirectory = (fdata.attrib & _A_SUBDIR) != 0;
		memcpy(fInfo.szFileName, fdata.name, strlen(fdata.name));
		CPacket packet2Client(2, (const char*)&fInfo, sizeof(fInfo));
		std::cout << fInfo.szFileName << "  " << "hasnext" << fInfo.HasNext << std::endl;
		sendPacketQueue.push(packet2Client);
		nCount++;
	} while (!_findnext(hfind, &fdata));
	//发送信息到控制端。
	FILEINFO fInfo;
	fInfo.HasNext = FALSE;
	CPacket packet2Client(2, (const char*)&fInfo, sizeof(fInfo));
	std::cout << fInfo.szFileName << "  " << "hasnext" << fInfo.HasNext << std::endl;
	sendPacketQueue.push(packet2Client);
	return 0;
}
int CCommand::RunFile(const CPacket& packet, std::queue<CPacket>& sendPacketQueue) {
	if (packet.getCommand() != 3) {
		return -1;
	}
	std::string strPath(packet.getPacketSize(), 0);
	//std::vector<uint8_t> vp = packet.getContent();
	memcpy((void*)strPath.c_str(), packet.getContent().data(), packet.getContent().size());
	::ShellExecuteA(NULL, NULL, strPath.c_str(), NULL, NULL, SW_SHOWNORMAL);
	CPacket packet2Client(3);
	sendPacketQueue.push(packet2Client);
	return 0;
}
int CCommand::DownloadFile(const CPacket& packet, std::queue<CPacket>& sendPacketQueue) {
	if (packet.getCommand() != 4) {
		return -1;
	}
	std::string strPath(packet.getPacketSize(), 0);

	memcpy((void*)strPath.c_str(), packet.getContent().data(), packet.getContent().size());
	long long data = 0;
	FILE* pFile = nullptr;
	errno_t err = fopen_s(&pFile, strPath.c_str(), "rb");

	if (err != 0) {
		CPacket packet2Client(4, (char*)&data, 8);
		sendPacketQueue.push(packet2Client);
		return -1;
	}
	if (pFile != NULL) {
		fseek(pFile, 0, SEEK_END);
		data = _ftelli64(pFile);
		CPacket head(4, (char*)&data, 8);
		sendPacketQueue.push(head);
		fseek(pFile, 0, SEEK_SET);
		char buffer[0x400]{ 0 };
		size_t rLen = 0;
		do {
			rLen = fread(buffer, 1, 1024, pFile);
			CPacket packet2Client(4, buffer, rLen);
			sendPacketQueue.push(packet2Client);
		} while (rLen >= 1024);
		fclose(pFile);
	}
	CPacket packet2Client(4);
	sendPacketQueue.push(packet2Client);
	return 0;
}
int CCommand::MouseEvent(const CPacket& packet, std::queue<CPacket>& sendPacketQueue) {
	if (packet.getCommand() != 5) {
		return -1;
	}
	typedef struct  SMouseEvent {
		SMouseEvent() {
			nAction = 0;
			nButton = -1;
			ptXY.x = 0;
			ptXY.y = 0;
		}
		WORD nAction;//点击、移动、双击
		WORD nButton;//左键、右键、中键
		POINT ptXY;//坐标
	}MOUSEEV, PMOUSEEV;
	MOUSEEV mouse;
	if (sizeof(MOUSEEV) != packet.getContent().size())
		return -1;
	memcpy(&mouse, packet.getContent().data(), sizeof(MOUSEEV));

	DWORD nFlag = 0;
	switch (mouse.nButton) {
	case 0://左键
		nFlag = 1;
		break;
	case 1://右键
		nFlag = 2;
		break;
	case 2://中键
		nFlag = 4;
		break;
	case 4://没有按键
		nFlag = 8;
		break;
	}
	if (nFlag != 8) {
		SetCursorPos(mouse.ptXY.x, mouse.ptXY.y);
	}
	switch (mouse.nAction) {
	case 0://单击
		nFlag |= 0x10;
		break;
	case 1://双击
		nFlag |= 0x20;
		break;
	case 2://按下
		nFlag |= 0x40;
		break;
	case 3: //放开
		nFlag |= 0x80;
		break;
	}

	switch (nFlag) {
	case 0x21://左键双机
		mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, GetMessageExtraInfo());
		mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, GetMessageExtraInfo());
	case 0x11://左键单机
		mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, GetMessageExtraInfo());
		mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, GetMessageExtraInfo());
		break;
	case 0x41://左键按下
		mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, GetMessageExtraInfo());
		break;
	case 0x81://左键放开
		mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, GetMessageExtraInfo());
		break;
	case 0x22://右键双机
		mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, GetMessageExtraInfo());
		mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, GetMessageExtraInfo());
	case 0x12://→键单机
		mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, GetMessageExtraInfo());
		mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, GetMessageExtraInfo());
		break;
	case 0x42://右键按下
		mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, GetMessageExtraInfo());
		break;
	case 0x82://右键放开
		mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, GetMessageExtraInfo());
		break;
	case 0x24://中键双机
		mouse_event(MOUSEEVENTF_MIDDLEDOWN, 0, 0, 0, GetMessageExtraInfo());
		mouse_event(MOUSEEVENTF_MIDDLEUP, 0, 0, 0, GetMessageExtraInfo());
	case 0x14://中键单机
		mouse_event(MOUSEEVENTF_MIDDLEDOWN, 0, 0, 0, GetMessageExtraInfo());
		mouse_event(MOUSEEVENTF_MIDDLEUP, 0, 0, 0, GetMessageExtraInfo());
		break;
	case 0x44://中键按下
		mouse_event(MOUSEEVENTF_MIDDLEDOWN, 0, 0, 0, GetMessageExtraInfo());
		break;
	case 0x84://中键放开
		mouse_event(MOUSEEVENTF_MIDDLEUP, 0, 0, 0, GetMessageExtraInfo());
		break;
	case 0x08://单纯的鼠标移动
		mouse_event(MOUSEEVENTF_MOVE, mouse.ptXY.x, mouse.ptXY.y, 0, GetMessageExtraInfo());
		break;
	}
	CPacket packet2Client(5);
	sendPacketQueue.push(packet2Client);

	return 0;
}
int CCommand::SendScreen(const CPacket& packet, std::queue<CPacket>& sendPacketQueue) {
	if (packet.getCommand() != 6) {
		return -1;
	}
	CImage screen; //GDI
	HDC hScreen = ::GetDC(NULL);
	int nBitPerPixel = ::GetDeviceCaps(hScreen, BITSPIXEL);
	int nWidth = GetDeviceCaps(hScreen, HORZRES);
	int nHeigth = GetDeviceCaps(hScreen, VERTRES);
	screen.Create(nWidth, nHeigth, nBitPerPixel);
	BitBlt(screen.GetDC(), 0, 0, nWidth, nHeigth, hScreen, 0, 0, SRCCOPY);
	ReleaseDC(NULL, hScreen);
	HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, 0);
	IStream* pStream = NULL;
	HRESULT ret = CreateStreamOnHGlobal(hMem, TRUE, &pStream);
	if (ret == S_OK) {
		screen.Save(pStream, Gdiplus::ImageFormatJPEG);
		LARGE_INTEGER bg{ 0 };
		pStream->Seek(bg, STREAM_SEEK_SET, NULL);
		char* pData = (char*)GlobalLock(hMem);
		size_t nSize = GlobalSize(hMem);
		CPacket packet2Client(6, pData, nSize);
		//CTool::Dump(packet2Client);
		sendPacketQueue.push(packet2Client);
		GlobalUnlock(hMem);
	}
	//screen.Save(_T("test2023.jpg"), Gdiplus::ImageFormatJPEG);
	pStream->Release();
	screen.ReleaseDC();
	GlobalFree(hMem);
 	return 0;
}
int CCommand::LockMachine(const CPacket& packet, std::queue<CPacket>& sendPacketQueue) {
	if (packet.getCommand() != 7) {
		return -1;
	}
	if (lockdlg.m_hWnd == NULL || lockdlg.m_hWnd == INVALID_HANDLE_VALUE) {
		_beginthread(CCommand::ThreadLoackDlg, 0, this);
		isLocked = true;
	}
	CPacket packet2Client(7);
	sendPacketQueue.push(packet2Client);
	return 0;
}
int CCommand::UnlockMachine(const CPacket& packet, std::queue<CPacket>& sendPacketQueue) {
	if (packet.getCommand() != 8) {
		return -1;
	}
	if (isLocked) {
		lockdlg.PostMessage(WM_UNLOCK, 0x1234, 0x4321);
		isLocked = false;
	}
	CPacket packet2Client(8);
	sendPacketQueue.push(packet2Client);
	return 0;
}
int CCommand::TestConnect(const CPacket& packet, std::queue<CPacket>& sendPacketQueue) {
	if (packet.getCommand() != 1981) {
		return -1;
	}
	CPacket packet2Client(1981);
	sendPacketQueue.push(packet2Client);
	return 0;
}
int CCommand::DeleteLocalFile(const CPacket& packet, std::queue<CPacket>& sendPacketQueue) {
	if (packet.getCommand() != 9) {
		return -1;
	}
	std::string strPath(packet.getPacketSize(), 0);
	memcpy((void*)strPath.c_str(), packet.getContent().data(), packet.getContent().size());
	//mbstowcs()
	DeleteFileA(strPath.c_str());
	CPacket packet2Client(9);
	sendPacketQueue.push(packet2Client);
	return 0;
}
void CCommand::ThreadLoackDlg(void* arg) {
	CCommand* thiz = (CCommand*)arg;
	/*CLockDlg dlg = ;*/
	BOOL bRet = thiz->lockdlg.Create(IDD_DIALOG1);
	thiz->lockdlg.ShowWindow(SW_SHOW);
	CRect rect;
	rect.left = 0;
	rect.top = 0;
	rect.right = GetSystemMetrics(SM_CXFULLSCREEN);
	rect.bottom = (LONG)(GetSystemMetrics(SM_CYFULLSCREEN) * 1.1);
	thiz->lockdlg.MoveWindow(rect);
	//窗口置顶
	thiz->lockdlg.SetWindowPos(&thiz->lockdlg.wndTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
	//限制鼠标功能
	ShowCursor(FALSE);
	//隐藏任务栏
	::ShowWindow(::FindWindow(_T("Shell_TrayWnd"), NULL), SW_HIDE);
	//限制鼠标活动范围；
	thiz->lockdlg.GetWindowRect(rect);
	ClipCursor(rect);
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {
		if(msg.message == IDCANCEL|| msg.message == IDOK)
			continue;
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		if (msg.message == WM_UNLOCK) {
			TRACE("msg:%08x wParam:%08x lParam:%08x\r\n", msg.message, msg.wParam, msg.lParam);
			if (msg.wParam == 0x1234 && msg.lParam == 0x4321) {//按下esc退出
				break;
			}
		}
	}
	thiz->isLocked = false;
	thiz->lockdlg.DestroyWindow();
	ShowCursor(true);
	::ShowWindow(::FindWindow(_T("Shell_TrayWnd"), NULL), SW_SHOW);
	_endthread();
}


