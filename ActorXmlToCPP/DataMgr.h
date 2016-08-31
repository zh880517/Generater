#pragma once

#include <DataDesc.h>
#include <map>
class DataMgr
{
public:
	DataMgr() = default;
	~DataMgr() = default;

	bool	Load(const std::string& strXmlPath, const std::string& strPath);

	DataGroup* GetDataGroup(const std::string& strName);

	static bool	WriteFile(const std::string& strFileName, const std::string& strOut, bool bCheck);

	static DataMgr& Instance();
private:
	std::map<std::string, DataGroup*>	m_Group;
	std::map < std::string, DataFile > m_File;
};