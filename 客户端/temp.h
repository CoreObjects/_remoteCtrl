#pragma once
// 客户端.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include "Model.h"
CClientSocket& m_Client = CClientSocket::getInstance();
bool getDriverInfo(MyString& driverInfo) {
	m_Client.initSocket("127.0.0.1");
	m_Client.connectToServer();
	CPacket packet(1);
	m_Client.sendPacket(packet);
	m_Client.receiveData();
	CPacket recPacket = m_Client.popPacket();
	if (recPacket.getCommand() == (uint32_t)-1) {
		return false;
	}
	driverInfo = (char*)recPacket.getContent().data();
	m_Client.closeSocket();
	return true;
}

bool getFileInfo(std::string szFilePath) {
	m_Client.initSocket("127.0.0.1");
	m_Client.connectToServer();
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
	m_Client.sendPacket(CPacket(2, szFilePath.c_str(), szFilePath.length()));
	FILEINFO fileInfo;
	while (fileInfo.HasNext) {
		m_Client.receiveData();
		std::queue<CPacket>& packetQueue = m_Client.GetPacketQueue();
		while (!packetQueue.empty()) {
			CPacket recPacket = packetQueue.front();
			packetQueue.pop();
			if (recPacket.getContent().size() != sizeof(FILEINFO)) {
				continue;
			}
			if (recPacket.getCommand() == (uint32_t)-1) {
				continue;
			}
			memcpy(&fileInfo, recPacket.getContent().data(), recPacket.getContent().size());
			if (fileInfo.HasNext == FALSE) {
				break;
			}
			if (fileInfo.IsDirectory) {
				std::cout << "目录：" << fileInfo.szFileName << std::endl;
			}
			else {
				std::cout << "文件：" << fileInfo.szFileName << std::endl;
			}
		}
	}
	m_Client.closeSocket();
	return true;
}

bool runFile(std::string& strFilePath) {
	m_Client.initSocket("127.0.0.1");
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

bool downLoadFile(std::string& strFilePath) {
	m_Client.initSocket("127.0.0.1");
	m_Client.connectToServer();
	FILE* pFile;
	errno_t err = fopen_s(&pFile, "1.jpg", "wb+");
	if (err != 0) {
		char errorBuffer[256];
		strerror_s(errorBuffer, sizeof(errorBuffer), err);
		printf("无法打开文件：%s\n", errorBuffer);
		return false;
	}
	CPacket packet(4, strFilePath.c_str(), strFilePath.length());
	m_Client.sendPacket(packet);
	m_Client.receiveData();
	CPacket recPacket = m_Client.popPacket();
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
		m_Client.receiveData();
		std::queue<CPacket>& packetQueue = m_Client.GetPacketQueue();
		while (!packetQueue.empty()) {
			CPacket recPacket = packetQueue.front();
			packetQueue.pop();
			fwrite((const void*)recPacket.getContent().data(), 1, recPacket.getContentLength(), pFile);
			i += recPacket.getContentLength();
		}
	}
	fclose(pFile);
	m_Client.closeSocket();
	return true;
}

bool getScreen() {
	m_Client.initSocket("127.0.0.1");
	m_Client.connectToServer();
	FILE* pFile;
	errno_t err = fopen_s(&pFile, "2.jpg", "wb+");
	if (err != 0) {
		char errorBuffer[256];
		strerror_s(errorBuffer, sizeof(errorBuffer), err);
		printf("无法打开文件：%s\n", errorBuffer);
		return false;
	}
	CPacket packet(6);
	m_Client.sendPacket(packet);
	m_Client.receiveData();
	CPacket recPacket = m_Client.popPacket();
	if (recPacket.getCommand() != 6) {
		m_Client.closeSocket();
		fclose(pFile);
		return false;
	}
	fwrite((const void*)recPacket.getContent().data(), 1, recPacket.getContentLength(), pFile);
	fclose(pFile);
	m_Client.closeSocket();
	return true;
}

bool deleteFile(std::string& strFilePath) {
	m_Client.initSocket("127.0.0.1");
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

bool testConnect() {
	m_Client.initSocket("127.0.0.1");
	m_Client.connectToServer();
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

bool lockMachine() {
	m_Client.initSocket("127.0.0.1");
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

bool unlockMachine() {
	m_Client.initSocket("127.0.0.1");
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

