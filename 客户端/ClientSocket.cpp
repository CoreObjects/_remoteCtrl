#include "ClientSocket.h"

//CClientSocket& m_Client = CClientSocket::getInstance();

bool CClientSocket::initSocket(PCSTR pcServer, PCSTR pcPort /*= DEFAULT_PORT*/) {
	if (ConnectSocket != INVALID_SOCKET) {
		closesocket(ConnectSocket);
	}
	int iResult = getaddrinfo(pcServer, pcPort, &hints, &result);
	if (iResult != 0) {
		std::cout << "getaddrinfo failed: " << iResult << std::endl;
		return false;
	}

	ConnectSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ConnectSocket == INVALID_SOCKET) {
		std::cout << "Error at socket: " << WSAGetLastError() << std::endl;
		freeaddrinfo(result);
		return false;
	}
	return true;
}

bool CClientSocket::connectToServer() {
	int iResult = connect(ConnectSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		closesocket(ConnectSocket);
		ConnectSocket = INVALID_SOCKET;
	}

	freeaddrinfo(result);

	if (ConnectSocket == INVALID_SOCKET) {
		std::cout << "Unable to connect to server!" << std::endl;
		return false;
	}
	return true;
}

bool CClientSocket::sendPacket(const CPacket& packet) {
	const std::vector<uint8_t>& data = packet.data();
	int data_size = static_cast<int>(data.size());

	int iResult = send(ConnectSocket, reinterpret_cast<const char*>(data.data()), data_size, 0);
	if (iResult == SOCKET_ERROR) {
		std::cout << "send failed: " << WSAGetLastError() << std::endl;
		closesocket(ConnectSocket);
		return false;
	}

	return true;
}

void CClientSocket::closeSocket() {
	if (ConnectSocket != INVALID_SOCKET) {
		closesocket(ConnectSocket);
		ConnectSocket = INVALID_SOCKET;
	}
}

bool CClientSocket::receiveData() {
	char* recvbuf = new char[MAXBUFLEN];
	int iResult = recv(ConnectSocket, recvbuf, MAXBUFLEN, 0);
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
					packetQueue.push(packet);
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

CClientSocket::CClientSocket() {
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		std::cout << "WSAStartup failed: " << iResult << std::endl;
		return;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;   // 我们在客户端使用 AF_UNSPEC，以便可以处理IPv4和IPv6的任何类型的地址。
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
}

CClientSocket::~CClientSocket() {
	closeSocket();
	WSACleanup();
}

