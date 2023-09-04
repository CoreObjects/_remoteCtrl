#pragma once
#include "Model.h"
class CController {
public:
	static CController& getInstance() {
		static CController instance;
		return instance;
	}
	void setIPandPort(std::string strIP, std::string strPort);
	bool testConnect()const;
	bool getDriverInfo();
	bool getFileInfo();
	bool runFile();
	void downLoadFile();
	bool getScreen();
	bool deleteFile();
	bool lockMachine();
	bool unlockMachine();
	void updataFileInfo(std::string m_strFilePath,std::string m_strLocalFilePath);
	std::vector<std::string> m_driverInfo;
	std::list<FILEINFO> m_lstFileInfo;
	std::string m_strFilePath;
	std::string m_strLocalFilePath;
	CImage m_image;
	
public:

private:

private:
	CController();
	CController(CController const&) = delete;
	void operator=(CController const&) = delete;
	~CController();
	CModel& m_model = CModel::getInstance();
	

};

