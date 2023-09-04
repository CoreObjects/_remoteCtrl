#pragma once
#include "pch.h"
#include "ClientSocket.h"
#include <iostream>
#include "Packet.h"
#include "MyString.h"
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
class CModel {
public:
	static CModel& getInstance() {
		static CModel instance;
		return instance;
	}
	bool getDriverInfo(std::string& driverInfo)const;
	bool getFileInfo(const std::string& szFilePath, std::list<FILEINFO>& lstFileInfo)const;
	bool runFile(const std::string& strFilePath)const;
	void downLoadFile(const std::string& strFilePath, const std::string& strLocalFilePath);
	//	bool downLoadFile(const std::string& strFilePath, const std::string& strLocalFilePath)const;
	static DWORD WINAPI threadDownloadFile(void* arg);
	bool getScreen(CImage& image)const;
	bool deleteFile(std::string& strFilePath)const;
	bool testConnect()const;
	bool lockMachine()const;
	bool unlockMachine()const;
	void setIPandPort(std::string strIP, std::string strPort);
private:
	CModel(const char* szIP = DEFAULT_IP, const char* szPort = DEFAULT_PORT);
	~CModel();
	CModel(CModel const&) = delete;
	void operator=(CModel const&) = delete;
	std::string  m_strIP;
	std::string m_strPort;
	CClientSocket& m_Client = CClientSocket::getInstance();
	std::string strServerDownloadPath;
	std::string strLocalDownloadFilePath;
};

