#include "ActorMgr.h"
#include <FileUtils.h>
#include <DataMgr.h>

bool ActorMgr::Load(const std::string & strXmlPath, const std::string & strPath)
{
	std::list<FileSystem::path> listFile;
	FileUtils::GetAllFile(strXmlPath, listFile, ".xml");
	for (auto& it : listFile)
	{
		ActorFile stFile;
		if (!stFile.Parse(it.generic_string()))
			return false;
		if (stFile.vActor.size() == 0)
			continue;

		std::string strOut;
		if (!stFile.Generate(strOut))
			return false;
		std::string strFileName = strPath + FileUtils::PathSubWithOutExt(it.generic_string(), strXmlPath) + ".h";
		m_mOut[strFileName].swap(strOut);
	}

	for (auto& it:m_mOut)
	{
		if (!DataMgr::WriteFile(it.first, it.second, true))
			return false;
	}

	return true;
}

ActorMgr & ActorMgr::Instance()
{
	static ActorMgr s_ActorMgr;
	return s_ActorMgr;
}
