#include "pch.h"
#include "ServerSocket.h"

CServerSocket& g_myServer = CServerSocket::getInstance();
bool CServerSocket::initSocket(PCSTR pcPort) {
	int iResult = getaddrinfo(NULL, pcPort, &hints, &result);
	if (iResult != 0) {
		std::cout << "getaddrinfo failed: " << iResult << std::endl;
		return false;
	}

	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ListenSocket == INVALID_SOCKET) {
		std::cout << "Error at socket: " << WSAGetLastError() << std::endl;
		freeaddrinfo(result);
		return false;
	}

	iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		std::cout << "bind failed with error: " << WSAGetLastError() << std::endl;
		freeaddrinfo(result);
		closesocket(ListenSocket);
		return false;
	}
	iResult = listen(ListenSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR) {
		std::cout << "listen failed with error: " << WSAGetLastError() << std::endl;
		closesocket(ListenSocket);
		return false;
	}
	freeaddrinfo(result);

	return true;
}

bool CServerSocket::acceptClient() {

	ClientSocket = accept(ListenSocket, NULL, NULL);
	if (ClientSocket == INVALID_SOCKET) {
		std::cout << "accept failed: " << WSAGetLastError() << std::endl;
		closesocket(ListenSocket);
		return false;
	}
	return true;

}

bool CServerSocket::sendPacket(const CPacket& packet) {
	const std::vector<uint8_t>& data = packet.data();
	int iResult = send(ClientSocket, (const char*)data.data(), static_cast<int>(data.size()), 0);
	if (iResult == SOCKET_ERROR) {
		std::cout << "send failed: " << WSAGetLastError() << std::endl;
		std::cout << "需要发送的数据包为：" << std::endl;
		for (auto i : data) {
			printf("%02x ", i);
		}
		closesocket(ClientSocket);
		ClientSocket = INVALID_SOCKET;
		return false;
	}
	return true;
}

bool CServerSocket::sendAllPacket() {
	while (!sendPacketQueue.empty()) {
		if (!sendPacket(popSendPacket())) {
			return false;
		}
	}
	return true;
}

void CServerSocket::closeSocket() {
	closesocket(ListenSocket);
	ListenSocket = INVALID_SOCKET;
	if (ClientSocket != INVALID_SOCKET) {
		closesocket(ClientSocket);
		ClientSocket = INVALID_SOCKET;
	}
}

void CServerSocket::closeClient() {
	if (ClientSocket != INVALID_SOCKET) {
		closesocket(ClientSocket);
		ClientSocket = INVALID_SOCKET;
	}
}

bool CServerSocket::receiveData() {
	char* recvbuf = new char[MAXBUFLEN];
	int iResult = recv(ClientSocket, recvbuf, MAXBUFLEN, 0);
	if (iResult > 0) {
		// 如果数据已接收，将其添加到recvBuffer
		recvBuffer.insert(recvBuffer.end(), recvbuf, recvbuf + iResult);
		auto it = recvBuffer.begin();
		while (std::distance(it, recvBuffer.end()) >= 14) {
			// Check packet header
			uint16_t header = ((uint8_t)*it << 8) | (uint8_t) * (it + 1);
			if (header != 0xFFFE) {
				// Header mismatch, advance the iterator
				++it;
				continue;
			}
			// Try to construct packet
			try {
				CPacket packet((const char*)&(*it));
				// Check packet validity
				if (packet.isValid()) {
					recvPacketQueue.push(packet);
				}
				// Advance the iterator past the successfully parsed packet
				std::advance(it, packet.getPacketSize());
				// Erase parsed data from recvBuffer
				recvBuffer.erase(recvBuffer.begin(), it);
				it = recvBuffer.begin();
			}
			catch (...) {
				// Construction failed, advance the iterator
				++it;
			}
		}
		delete[] recvbuf;
		return true;
	}
	else if (iResult == 0) {
		// 连接关闭
		closeSocket();
		delete[] recvbuf;
		return false;
	}
	else {
		// recv失败
		closeSocket();
		delete[] recvbuf;
		return false;
	}
}


CPacket CServerSocket::popRecvPacket() {
	if (!recvPacketQueue.empty()) {
		CPacket packet = recvPacketQueue.front();
		recvPacketQueue.pop();
		return packet;
	}
	return CPacket(0xFFFFFFFF);
}

CPacket CServerSocket::popSendPacket() {
	if (!sendPacketQueue.empty()) {
		CPacket packet = sendPacketQueue.front();
		sendPacketQueue.pop();
		return packet;
	}
	return CPacket(0xFFFFFFFF);
}

CServerSocket::CServerSocket() {
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		std::cout << "WSAStartup failed: " << iResult << std::endl;
		return;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;
}

CServerSocket::~CServerSocket() {
	closeSocket();
	WSACleanup();
}
