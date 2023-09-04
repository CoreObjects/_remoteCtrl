#include "pch.h"
#include "ClientSocket.h"

int CClientSocket::setSocketTimeout(int timeoutMillisec) {
	// �����׽��ֽ��ճ�ʱѡ��
	int result = setsockopt(ConnectSocket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeoutMillisec, sizeof(timeoutMillisec));
	if (result == SOCKET_ERROR) {
		std::cerr << "Failed to set socket receive timeout." << std::endl;
	}
	return result;
}

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
	return setMode(1);
}

bool CClientSocket::connectToServer() {
	
	int iResult = connect(ConnectSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		if (WSAGetLastError() == WSAEWOULDBLOCK) {
			// WSAEWOULDBLOCK��ʾ�����׽����Ƿ������ģ����Ӳ����������
			FD_SET setW;
			TIMEVAL Timeout;
			FD_ZERO(&setW);
			FD_SET(ConnectSocket, &setW);

			// ���ó�ʱʱ��
			Timeout.tv_sec = 3; // 3��
			Timeout.tv_usec = 0;

			iResult = select(0, NULL, &setW, NULL, &Timeout);
			if (iResult == 0) {
				// select��ʱ
				TRACE("Timeout\n");
				closesocket(ConnectSocket);
				ConnectSocket = INVALID_SOCKET;
			}
			else {
				// select����1������׽����Ƿ��д����ȷ�������Ƿ�ɹ�
				if (FD_ISSET(ConnectSocket, &setW)) {
					int error = 0;
					int len = sizeof(error);
					getsockopt(ConnectSocket, SOL_SOCKET, SO_ERROR, (char*)&error, &len);
					if (error == 0) {
						TRACE("Connected\n");
					}
					else {
						TRACE("Error connecting: %d\n", error);
						closesocket(ConnectSocket);
						ConnectSocket = INVALID_SOCKET;
					}
				}
				else {
					TRACE("Error: select returned 1 but ConnectSocket is not in the set\n");
					closesocket(ConnectSocket);
					ConnectSocket = INVALID_SOCKET;
				}
			}
		}
		else {
			TRACE("Error at connect(): %ld\n", WSAGetLastError());
			closesocket(ConnectSocket);
			ConnectSocket = INVALID_SOCKET;
		}
	}

	freeaddrinfo(result);
	setMode(0);
	setSocketTimeout(3000);
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
		// ��������ѽ��գ�������ӵ�recvBuffer
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
		// ���ӹر�
		closeSocket();
		delete[] recvbuf;
		return false;
	}
	else {
		// recvʧ��
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
	hints.ai_family = AF_UNSPEC;   // �����ڿͻ���ʹ�� AF_UNSPEC���Ա���Դ���IPv4��IPv6���κ����͵ĵ�ַ��
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
}

CClientSocket::~CClientSocket() {
	closeSocket();
	WSACleanup();
}

