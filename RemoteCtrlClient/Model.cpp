#include "pch.h"
#include "Model.h"

CModel::CModel(const char* szIP, const char* szPort) {
	m_strIP = szIP;
	m_strPort = szPort;
}

bool CModel::getDriverInfo(std::string& driverInfo)const {
	m_Client.initSocket(m_strIP.c_str(), m_strPort.c_str());
	m_Client.connectToServer();
	CPacket packet(1);
	m_Client.sendPacket(packet);
	m_Client.receiveData();
	CPacket recPacket = m_Client.popPacket();
	if (recPacket.getCommand() == (uint32_t)-1) {
		return false;
	}
	driverInfo.resize(recPacket.getContentLength(), 0);
	memcpy((void*)driverInfo.c_str(), recPacket.getContent().data(), recPacket.getContentLength());
	m_Client.closeSocket();
	return true;
}

bool CModel::getFileInfo(const std::string& szFilePath, std::list<FILEINFO>& lstFileInfo)const {
	bool bRet = true;
	lstFileInfo.clear();
	m_Client.initSocket(m_strIP.c_str(), m_strPort.c_str());
	m_Client.connectToServer();
	m_Client.sendPacket(CPacket(2, szFilePath.c_str(), szFilePath.length()));
	FILEINFO fileInfo;
	while (fileInfo.HasNext) {
		m_Client.receiveData();
		std::queue<CPacket>& packetQueue = m_Client.GetPacketQueue();
		while (!packetQueue.empty()) {
			CPacket recPacket = packetQueue.front();
			packetQueue.pop();
			if ((recPacket.getContent().size() != sizeof(FILEINFO)) | \
				(recPacket.getCommand() == (uint32_t)-1)) {
				bRet = false;
				continue;
			}
			memcpy(&fileInfo, recPacket.getContent().data(), recPacket.getContent().size());
			if (fileInfo.HasNext) {
				lstFileInfo.push_back(fileInfo);
			}
		}
	}
	m_Client.closeSocket();
	return bRet;
}

bool CModel::runFile(const std::string& strFilePath)const {
	m_Client.initSocket(m_strIP.c_str(), m_strPort.c_str());
	m_Client.connectToServer();
	CPacket packet(3, strFilePath.c_str(), strFilePath.length());
	m_Client.sendPacket(packet);
	m_Client.receiveData();
	CPacket recPacket = m_Client.popPacket();
	if (recPacket.getCommand() != 3) {
		m_Client.closeSocket();
		return false;
	}
	m_Client.closeSocket();
	return true;
}

void CModel::downLoadFile(const std::string& strFilePath, const std::string& strLocalFilePath) {
	strServerDownloadPath = strFilePath;
	strLocalDownloadFilePath = strLocalFilePath;
	DWORD dwThreadId;
	HANDLE hThread = CreateThread(NULL, 0, CModel::threadDownloadFile, this, 0, &dwThreadId);
	CloseHandle(hThread);
}

DWORD WINAPI CModel::threadDownloadFile(void* arg) {
	CModel* thiz = (CModel*)arg;

	thiz->m_Client.initSocket(thiz->m_strIP.c_str(), thiz->m_strPort.c_str());
	thiz->m_Client.connectToServer();
	FILE* pFile;
	errno_t err = fopen_s(&pFile, thiz->strLocalDownloadFilePath.c_str(), "wb+");
	if (err != 0) {
		char errorBuffer[256];
		strerror_s(errorBuffer, sizeof(errorBuffer), err);
		printf("无法打开文件：%s\n", errorBuffer);
		return false;
	}
	CPacket packet(4, thiz->strServerDownloadPath.c_str(), thiz->strServerDownloadPath.length());
	thiz->m_Client.sendPacket(packet);
	thiz->m_Client.receiveData();
	CPacket recPacket = thiz->m_Client.popPacket();
	if (recPacket.getCommand() != 4) {
		fclose(pFile);
		return false;
	}
	long long nlength = *(long long*)recPacket.getContent().data();
	if (nlength == 0) {
		fclose(pFile);
		return false;
	}
	for (long long i = 0; i < nlength; i++) {
		thiz->m_Client.receiveData();
		std::queue<CPacket>& packetQueue = thiz->m_Client.GetPacketQueue();
		while (!packetQueue.empty()) {
			CPacket recPacket = packetQueue.front();
			packetQueue.pop();
			fwrite((const void*)recPacket.getContent().data(), 1, recPacket.getContentLength(), pFile);
			i += recPacket.getContentLength();
		}
	}
	fclose(pFile);
	thiz->m_Client.closeSocket();
	return true;
}

bool CModel::getScreen(CImage& image) const {
	m_Client.initSocket(m_strIP.c_str(), m_strPort.c_str());
	m_Client.connectToServer();
	CPacket packet(6);
	m_Client.sendPacket(packet);
	m_Client.receiveData();
	CPacket recPacket = m_Client.popPacket();
	if (recPacket.getCommand() != 6) {
		m_Client.closeSocket();
		return false;
	}
	m_Client.closeSocket();
	char* pData = new char[recPacket.getContentLength()];
	memcpy(pData, recPacket.getContent().data(), recPacket.getContentLength());
	HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, 0);
	if (hMem == nullptr) {
		TRACE("内存不足：\r\n");
		Sleep(1);
		return false;
	}
	IStream* pStream = NULL;
	HRESULT hRet = CreateStreamOnHGlobal(hMem, TRUE, &pStream);
	if (hRet == S_OK) {
		ULONG length = 0;
		pStream->Write(pData, recPacket.getContentLength(), &length);
		LARGE_INTEGER bg{ 0 };
		pStream->Seek(bg, STREAM_SEEK_SET, NULL);
		if ((HBITMAP)image != NULL)image.Destroy();
		image.Load(pStream);
		return true;
	}
	return false;
}

bool CModel::deleteFile(std::string& strFilePath) const {
	m_Client.initSocket(m_strIP.c_str(), m_strPort.c_str());
	m_Client.connectToServer();
	CPacket packet(9, strFilePath.c_str(), strFilePath.length());
	m_Client.sendPacket(packet);
	m_Client.receiveData();
	CPacket recPacket = m_Client.popPacket();
	if (recPacket.getCommand() != 9) {
		m_Client.closeSocket();
		return false;
	}
	m_Client.closeSocket();
	return true;
}

bool CModel::testConnect() const {
	if (m_Client.initSocket(m_strIP.c_str(), m_strPort.c_str()) == false) {
		return false;
	};
	if (m_Client.connectToServer() == false) {
		return false;
	};
	CPacket packet(1981);
	m_Client.sendPacket(packet);
	m_Client.receiveData();
	CPacket recPacket = m_Client.popPacket();
	if (recPacket.getCommand() != 1981) {
		m_Client.closeSocket();
		return false;
	}
	m_Client.closeSocket();
	return true;
}

bool CModel::lockMachine() const {
	
	m_Client.initSocket(m_strIP.c_str(), m_strPort.c_str());
	m_Client.connectToServer();
	CPacket packet(7);
	m_Client.sendPacket(packet);
	m_Client.receiveData();
	CPacket recPacket = m_Client.popPacket();
	if (recPacket.getCommand() != 7) {
		m_Client.closeSocket();
		return false;
	}
	m_Client.closeSocket();
	return true;
}

bool CModel::unlockMachine() const {
	m_Client.initSocket(m_strIP.c_str(), m_strPort.c_str());
	m_Client.connectToServer();
	CPacket packet(8);
	m_Client.sendPacket(packet);
	m_Client.receiveData();
	CPacket recPacket = m_Client.popPacket();
	if (recPacket.getCommand() != 8) {
		m_Client.closeSocket();
		return false;
	}
	m_Client.closeSocket();
	return true;
}

bool CModel::sendMouseEvent(const MOUSEEV& mouseInfo) const {
	m_Client.initSocket(m_strIP.c_str(), m_strPort.c_str());
	m_Client.connectToServer();
	CPacket packet(5, (const char*)&mouseInfo, sizeof(MOUSEEV));
	m_Client.sendPacket(packet);
	m_Client.receiveData();
	CPacket recPacket = m_Client.popPacket();
	if (recPacket.getCommand() != 5) {
		m_Client.closeSocket();
		return false;
	}
	m_Client.closeSocket();
	return true;
}

void CModel::setIPandPort(std::string strIP, std::string strPort) {
	m_strIP = strIP;
	m_strPort = strPort;
}

CModel::~CModel() {
	m_strIP.clear();
	m_strPort.clear();
}
