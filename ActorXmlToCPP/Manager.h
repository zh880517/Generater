#pragma once
#include <DataDesc.h>
#include <ActorDesc.h>
#include <map>
class DataMgr
{
public:
	DataMgr() = default;
	~DataMgr() = default;

	bool	Load(const std::string& strXmlPath);

	DataGroup* GetDataGroup(const std::string& strName);

private:
	std::string						m_strXmlPath;
	std::map<std::string, DataFile> m_mFiles;
};

class ActorMgr
{
public:
	ActorMgr(DataMgr* pMgr);
	~ActorMgr() = default;

	bool	Load(const std::string& strXmlPath, const std::string& strCppPath);

	bool	Check();

	bool	Generate();

private:
	DataMgr*							m_pDataMgr;
	std::string							m_strXmlPath;
	std::string							m_strCppPath;
	std::map<std::string, ActorFile>	m_mFiles;
};
