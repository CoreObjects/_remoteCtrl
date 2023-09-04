#pragma once

#include "LockDlg.h"
#define WM_UNLOCK (WM_USER+1)
class CCommand {
public:
	// 使用std::function来实现多态性的回调函数
	//using CommandFunction = int(*)(const CPacket&);
	typedef int (CCommand::* pCommandFunction)(const CPacket& ,std::queue<CPacket>&);
	// 返回单例
	static CCommand& getInstance() {
		static CCommand instance;
		return instance;
	}
	// 执行命令
	int executeCommand(const CPacket& packet ,std::queue<CPacket>& sendPacketQueue) {
		uint32_t command = packet.getCommand();
		auto ite = m_commandMap.find(command);
		if (ite != m_commandMap.end()) {
			return (this->*ite->second)(packet,sendPacketQueue);
		}
		else {
			return -1;
		}
	}
	CLockDlg lockdlg;
protected:
	// 注册命令
	void registerCommand(uint32_t command, pCommandFunction pfun) {
		m_commandMap.insert(std::make_pair(command, pfun));
	}
private:
	int SendDirverInfo(const CPacket& packet, std::queue<CPacket>& sendPacketQueue);
	int RunFile(const CPacket& packet, std::queue<CPacket>& sendPacketQueue);
	int DownloadFile(const CPacket& packet, std::queue<CPacket>& sendPacketQueue);
	int SendDirectoryInfo(const CPacket& packet, std::queue<CPacket>& sendPacketQueue);
	int MouseEvent(const CPacket& packet, std::queue<CPacket>& sendPacketQueue);
	int SendScreen(const CPacket& packet, std::queue<CPacket>& sendPacketQueue);
	int LockMachine(const CPacket& packet, std::queue<CPacket>& sendPacketQueue);
	int UnlockMachine(const CPacket& packet, std::queue<CPacket>& sendPacketQueue);
	int TestConnect(const CPacket& packet, std::queue<CPacket>& sendPacketQueue);
	int DeleteLocalFile(const CPacket& packet, std::queue<CPacket>& sendPacketQueue);
public:
	static void ThreadLoackDlg(void* arg);
private:
	
	CCommand() {
		lockdlg.m_hWnd = NULL;
		isLocked = false;
		struct{
			uint32_t uCmd;
			pCommandFunction pfun;
		}CommandMap[] = {
			{1, &CCommand::SendDirverInfo},
			{2, &CCommand::SendDirectoryInfo},
			{3, &CCommand::RunFile},
			{4, &CCommand::DownloadFile},
			{5, &CCommand::MouseEvent},
			{6, &CCommand::SendScreen},
			{7, &CCommand::LockMachine},
			{8, &CCommand::UnlockMachine},
			{9, &CCommand::DeleteLocalFile},
			{1981,&CCommand::TestConnect},
			{0xFFFFFFFF,NULL}
		};
		for (int i = 0; CommandMap[i].uCmd != 0xFFFFFFFF; i++) {
			registerCommand(CommandMap[i].uCmd, CommandMap[i].pfun);
		}
	}

	~CCommand() {}

	CCommand(CCommand const&) = delete;
	void operator=(CCommand const&) = delete;

	// 存储命令和处理函数的映射
	std::unordered_map<uint32_t, pCommandFunction> m_commandMap;
	bool isLocked;
};
extern CCommand& g_myCmd;