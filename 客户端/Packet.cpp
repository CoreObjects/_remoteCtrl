#include <stdexcept>
#include "Packet.h"

CPacket::CPacket(uint32_t command, const uint8_t* data, uint32_t size)
	: packetData(2 + 4 + 4 + size + 4) // header, command, length, content, checksum
{
	constructPacket(command, data, size);
}

CPacket::CPacket(uint32_t command, const std::vector<uint8_t>& content)
	: packetData(2 + 4 + 4 + content.size() + 4) // header, command, length, content, checksum
{
	constructPacket(command, content.data(), content.size());
}

CPacket::CPacket(const char* data) {
	// Check header
	uint16_t header = ((uint8_t)data[0] << 8) | (uint8_t)data[1];
	if (header != 0xFFFE) {
		throw std::invalid_argument("Invalid header");
	}

	// Parse command
	uint32_t command = 0;
	for (int i = 0; i < 4; ++i) {
		command = (command << 8) | (uint8_t)data[2 + i];
	}
	if (command > MAXCMD) {
		throw std::invalid_argument("Invalid command");
	}
	// Parse length
	uint32_t length = 0;
	for (int i = 0; i < 4; ++i) {
		length = (length << 8) | (uint8_t)data[6 + i];
	}
	if (length > MAXBUFLEN) {
		throw std::invalid_argument("Invalid length");
	}
	// Calculate checksum
	uint32_t checksum = CRC32::getInstance().calculate(data + 10, length);
	uint32_t receivedChecksum = 0;
	for (int i = 0; i < 4; ++i) {
		receivedChecksum = (receivedChecksum << 8) | (uint8_t)data[10 + length + i];
	}

	if (checksum != receivedChecksum) {
		throw std::invalid_argument("Invalid checksum");
	}

	// Copy content
	std::vector<uint8_t> content(data + 10, data + 10 + length);

	// All checks passed, construct the packet
	// alloc mem for packetData
	packetData.resize(2 + 4 + 4 + length + 4);
	constructPacket(command, content.data(), length);
}


CPacket& CPacket::operator=(const CPacket& other) {
	// Copy assignment operator
	if (this != &other) {
		packetData = other.packetData;
	}
	return *this;

}

CPacket::CPacket(uint32_t command)\
	: packetData(2 + 4 + 4 + 4) // header, command, length, content, checksum
{
	constructPacket(command, NULL, 0);
}

CPacket::CPacket(uint32_t command, const char* data, uint32_t size)\
	: packetData(2 + 4 + 4 + size + 4) {// header, command, length, content, checksum
	constructPacket(command, (const uint8_t*)data, size);
}

uint32_t CPacket::getCommand() const {
	uint32_t command = 0;
	for (int i = 0; i < 4; ++i) {
		command |= (packetData[2 + i] << (8 * (3 - i)));
	}
	return command;
}

std::vector<uint8_t> CPacket::getContent() const {
	uint32_t length = 0;
	for (int i = 0; i < 4; ++i) {
		length |= (packetData[6 + i] << (8 * (3 - i)));
	}
	return std::vector<uint8_t>(packetData.begin() + 10, packetData.begin() + 10 + length);
}

bool CPacket::isValid() const {
	uint32_t length = 0;
	for (int i = 0; i < 4; ++i) {
		length |= (packetData[6 + i] << (8 * (3 - i)));
	}
	std::vector<uint8_t> content = std::vector<uint8_t>(packetData.begin() + 10, packetData.begin() + 10 + length);
	uint32_t computedChecksum = CRC32::getInstance().calculate((const char*)content.data(), content.size());
	uint32_t packetChecksum = 0;
	for (int i = 0; i < 4; ++i) {
		packetChecksum |= (packetData[10 + length + i] << (8 * (3 - i)));
	}
	return computedChecksum == packetChecksum;
}

uint32_t CPacket::getPacketSize() const {
	return packetData.size();
}

const uint8_t* CPacket::getPacketData() const {
	return packetData.data();
}

uint32_t CPacket::getContentLength() const {
	return getContent().size();
}

void CPacket::constructPacket(uint32_t command, const uint8_t* data, uint32_t size) {
	// header
	uint16_t header = 0xFFFE;
	packetData[0] = (header >> 8) & 0xFF;
	packetData[1] = header & 0xFF;

	// command
	for (int i = 0; i < 4; ++i) {
		packetData[2 + i] = (command >> (8 * (3 - i))) & 0xFF;
	}

	// length
	for (int i = 0; i < 4; ++i) {
		packetData[6 + i] = (size >> (8 * (3 - i))) & 0xFF;
	}

	// content
	if (nullptr != data && size > 0) {
		std::copy(data, data + size, packetData.begin() + 10);
	}

	// checksum
	uint32_t checksum = CRC32::getInstance().calculate((const char*)data, size);
	for (int i = 0; i < 4; ++i) {
		packetData[10 + size + i] = (checksum >> (8 * (3 - i))) & 0xFF;
	}
}
