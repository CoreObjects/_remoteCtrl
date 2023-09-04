#pragma once
#include "pch.h"

class CTool {
	CTool() {

	}
public:
	static void Dump(const char* pData, size_t nSize) {
		std::string strOut;
		for (size_t i = 0; i < nSize; i++) {
			char buf[8] = "";
			snprintf(buf, sizeof(buf), "%02X ", pData[i] & 0xff);
			strOut += buf;
			strOut += "\n";
		}
		OutputDebugStringA(strOut.c_str());
	}
	static void Dump(CPacket packet) {
		Dump((const char*)packet.getPacketData(), packet.getPacketSize());
	}
};

