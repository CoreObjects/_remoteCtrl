#ifndef CPACKET_H
#define CPACKET_H

#include <vector>
#include "CRC32.h"
#define MAXBUFLEN 512000
#define MAXCMD 65532
//数据包的格式 header 2字节, command 4个字节, length 4个字节, content length个字节, checksum 4个字节
class CPacket {
public:
	CPacket(uint32_t command, const std::vector<uint8_t>& content);

	CPacket(uint32_t command, const uint8_t* data, uint32_t size);

	CPacket(uint32_t command, const char* data, uint32_t size);

	CPacket(const char* data);

	CPacket(uint32_t command);

	CPacket(const CPacket& other) : packetData(other.packetData) {}  // Copy constructor

	CPacket& operator=(const CPacket& other);

	const std::vector<uint8_t>& data() const { return packetData; }

	uint32_t getCommand() const;

	std::vector<uint8_t> getContent() const;

	bool isValid() const;

	uint32_t getPacketSize() const;

	const uint8_t* getPacketData() const;

	uint32_t getContentLength()const;
private:
	void constructPacket(uint32_t command, const uint8_t* data, uint32_t size);

	std::vector<uint8_t> packetData;
};

#endif
