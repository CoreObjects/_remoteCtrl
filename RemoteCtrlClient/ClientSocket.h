#pragma once
#include "pch.h"
#include "Packet.h"

#pragma comment(lib, "ws2_32.lib")
#define DEFAULT_IP "127.0.0.1"
#define DEFAULT_PORT "27015"
class CClientSocket {
public:
	static CClientSocket& getInstance() {
		static CClientSocket instance;
		return instance;
	}

	bool initSocket(PCSTR pcServer = DEFAULT_IP, PCSTR pcPort = DEFAULT_PORT);
	bool connectToServer();
	bool sendPacket(const CPacket& packet);
	void closeSocket();
	bool receiveData();
	std::queue<CPacket>& GetPacketQueue() { return packetQueue; }
	CPacket popPacket() {
		if (!packetQueue.empty()) {
			CPacket packet = packetQueue.front();
			packetQueue.pop();
			return packet;
		}
		return CPacket(0xFFFFFFFF);
	}
	
private:
	CClientSocket();
	~CClientSocket();
	CClientSocket(CClientSocket const&) = delete;
	void operator=(CClientSocket const&) = delete;
	int setSocketTimeout(int);
	bool setMode(u_long mode) {
		int result = ioctlsocket(ConnectSocket, FIONBIO, &mode);
		if (result == SOCKET_ERROR) {
			std::cerr << "Failed to set non-blocking mode." << std::endl;
			return false;
		}
		return true;
	}
	struct addrinfo* result = NULL, * ptr = NULL, hints;
	SOCKET ConnectSocket = INVALID_SOCKET;
	std::vector<char> recvBuffer;
	std::queue<CPacket> packetQueue;
};

//extern CClientSocket&m_Client;