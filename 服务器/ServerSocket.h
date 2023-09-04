#pragma once

#include "pch.h"

#define DEFAULT_PORT "27015"

class CServerSocket {
public:
	static CServerSocket& getInstance() {
		static CServerSocket instance;
		return instance;
	}
	int ExecuteShortConnection() {
		if (!initSocket()) {
			return -1;//�����ʼ���쳣��δ�ܳɹ���ʼ������������״̬��
		}
		while (true) {
			acceptClient();
			receiveData();
			
			g_myCmd.executeCommand(popRecvPacket(),sendPacketQueue);
			sendAllPacket();
			closeClient();
		}
	}
protected:
	bool initSocket(PCSTR pcPort = DEFAULT_PORT);
	bool acceptClient();
	bool sendPacket(const CPacket& packet);
	bool sendAllPacket();
	void closeSocket();
	void closeClient();
	bool receiveData();
//	std::queue<CPacket>& getPacketQueue() { return recvPacketQueue; }
	CPacket popRecvPacket();
	CPacket popSendPacket();
private:
	CServerSocket();
	~CServerSocket();
	CServerSocket(CServerSocket const&) = delete;
	void operator=(CServerSocket const&) = delete;
	struct addrinfo* result = NULL, hints;
	SOCKET ListenSocket = INVALID_SOCKET;
	SOCKET ClientSocket = INVALID_SOCKET;
	std::vector<char> recvBuffer;
	std::queue<CPacket> recvPacketQueue;
	std::queue<CPacket> sendPacketQueue;
};
extern CServerSocket& g_myServer;