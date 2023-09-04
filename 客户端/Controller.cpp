#include "Controller.h"

void CController::setIPandPort(std::string strIP, std::string strPort) {
	return m_model.setIPandPort(strIP, strPort);
}

bool CController::testConnect() const {
	return m_model.testConnect();
}

bool CController::getDriverInfo() {
	std::string strDriver;
	bool bRet = m_model.getDriverInfo(strDriver);
	if (bRet == false) {
		return false;
	}
	std::string dr;
	for (char i : strDriver) {
		if (i == ',') {
			dr += ":";
			m_driverInfo.push_back(dr);
			dr.clear();
			continue;
		}
		dr += i;
	}
	dr += ":";
	m_driverInfo.push_back(dr);
	return false;
}

bool CController::getFileInfo() {
	return m_model.getFileInfo(m_strFilePath, m_lstFileInfo);
}

bool CController::runFile() {
	return m_model.runFile(m_strFilePath);
}

void CController::downLoadFile() {
	return m_model.downLoadFile(m_strFilePath, m_strLocalFilePath);
}

bool CController::getScreen() {
	return m_model.getScreen(m_image);
}

bool CController::deleteFile() {
	return m_model.deleteFile(m_strFilePath);
}

bool CController::lockMachine() {
	return m_model.lockMachine();
}

bool CController::unlockMachine() {
	return m_model.unlockMachine();
}

void CController::updataFileInfo(std::string szFilePath, std::string szLocalFilePath) {
	m_strFilePath = szFilePath;
	m_strLocalFilePath = szLocalFilePath;
}

CController::~CController() {

}

