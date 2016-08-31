#include "DataMgr.h"
#include <FileUtils.h>
#include <fstream>
bool DataMgr::Load(const std::string & strXmlPath, const std::string& strPath)
{
	std::list<FileSystem::path> listFile;
	FileUtils::GetAllFile(strXmlPath, listFile, ".xml");
	std::stringstream ssRegist;
	ssRegist << "#include \"stdafx.h\"\n";
	for (auto& it : listFile)
	{
		DataFile& stFile = m_File[it.generic_string()];
		if (!stFile.Parse(it.generic_string()))
		{
			return false;
		}
		if (stFile.vGroup.size() == 0)
			continue;

		std::string strFileName = FileUtils::PathSubWithOutExt(it.generic_string(), strXmlPath);
	
		std::string strOut;
		std::string strRegist;
		stFile.Generate(strOut, strRegist);
		ssRegist << "#include <" << strFileName << ".h>\n";
		ssRegist << strRegist;
		if (!WriteFile(strPath + strFileName + ".h", strOut, true))
			return false;

		if (stFile.vGroup.size() > 0)
		{
			for (auto&itGroup : stFile.vGroup)
			{
				if (m_Group.find(itGroup.strName) != m_Group.end())
				{
					LOG_FILE;
					std::cout << "Group Repeated, name = " << itGroup.strName << " in file: " << it.generic_string() << std::endl;
					return false;
				}
				itGroup.strFileName = strFileName;
				m_Group[itGroup.strName] = &itGroup;
			}
		}
	}

	if (!WriteFile(strPath + "key_field_regist.cpp", ssRegist.str(), true))
		return false;

	return true;
}


DataGroup * DataMgr::GetDataGroup(const std::string & strName)
{
	auto it = m_Group.find(strName);
	if (it != m_Group.end())
		return it->second;
	return nullptr;
}

bool DataMgr::WriteFile(const std::string & strFileName, const std::string & strOut, bool bCheck)
{
	{
		std::ifstream ifs(strFileName);
		if (ifs.is_open())
		{
			std::stringstream buffer;
			buffer << ifs.rdbuf();
			if (buffer.str() == strOut)
				return true;
		}
	}
	std::ofstream fs(strFileName);
	if (fs.is_open())
	{
		fs << strOut;
	}
	else
	{
		LOG_ERROR("Create File Faile! please check directory or use Administrator user, file name = " << strFileName);
		return false;
	}
	return true;
}

DataMgr & DataMgr::Instance()
{
	static DataMgr s_Instance;
	return s_Instance;
}
