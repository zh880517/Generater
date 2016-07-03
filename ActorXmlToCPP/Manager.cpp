#include <Manager.h>
#include <FileUtils.h>
#include <iostream>
#include <GenerateCPP.h>
#include <fstream>
#include <set>

bool DataMgr::Load(const std::string & strXmlPath)
{
	m_strXmlPath = strXmlPath;
	m_mFiles.clear();

	std::list<FileSystem::path> listFile;
	FileUtils::GetAllFile(m_strXmlPath, listFile, ".xml");
	for (auto& it : listFile)
	{
		DataFile stFile;
		if (!stFile.Parse(it.generic_string()))
		{
			return false;
		}
		if (stFile.vGroup.size() > 0)
		{
			m_mFiles[it.generic_string()] = stFile;
		}
	}
	if (m_mFiles.empty())
	{
		LOG_FILE
		std::cout << "Error : No Data File Parse ." << std::endl;
		return false;
	}
	return true;
}

DataGroup * DataMgr::GetDataGroup(const std::string & strName)
{
	for (auto& it : m_mFiles)
	{
		for (auto& group : it.second.vGroup)
		{
			if (group.strName == strName)
			{
				return &group;
			}
		}
	}
	return nullptr;
}

ActorMgr::ActorMgr(DataMgr * pMgr)
	: m_pDataMgr(pMgr)
{
}

bool ActorMgr::Load(const std::string & strXmlPath, const std::string & strCppPath)
{
	m_strXmlPath = strXmlPath;
	m_strCppPath = strCppPath;
	m_mFiles.clear();

	std::list<FileSystem::path> listFile;
	FileUtils::GetAllFile(m_strXmlPath, listFile, ".xml");
	for (auto& it : listFile)
	{
		ActorFile stFile;
		if (!stFile.Parse(it.generic_string()))
		{
			return false;
		}
		if (stFile.vActor.size() > 0)
		{
			m_mFiles[it.generic_string()] = stFile;
		}
	}
	if (m_mFiles.empty())
	{
		LOG_FILE
		std::cout << "Error : No Actor File Parse ." << std::endl;
		return false;
	}
	return true;
}

bool ActorMgr::Check()
{
	std::set<std::string> vName;

	auto AddName = [&vName]( std::string strGroup, const std::string& strNmae) ->bool
	{
		strGroup += ".";
		strGroup += strNmae;
		if (vName.find(strGroup) != vName.end())
		{
			std::cout << "Error: " << strGroup << " has been owner" << std::endl;
			return false;
		}
		vName.insert(strGroup);
		return true;
	};

	for (auto& file: m_mFiles)
	{
		for (auto& actor : file.second.vActor )
		{
			for (auto& itStruct:actor.vStruct)
			{
				for (auto& it:itStruct.vProperty)
				{
					if (it.bOwner && !AddName(itStruct.strGroup, it.strKey + "."+ it.strField))
					{
						std::cout << "Error : " << "in property field : " << it.strField << " in struct : " << itStruct.strName << std::endl;
						return false;
					}
				}
				for (auto& it : itStruct.vRepeat)
				{
					if (it.bOwner && !AddName(itStruct.strGroup, it.strRef))
					{
						std::cout << "Error : " << "in repeat name : " << it.strName << " in struct : " << itStruct.strName << std::endl;
						return false;
					}
				}
			}
			for (auto& itStruct : actor.vMap)
			{
				for (auto& it : itStruct.vProperty)
				{
					if (it.bOwner && !AddName(itStruct.strGroup, it.strKey + "." + it.strField))
					{
						std::cout << "Error : " << "in property field : " << it.strField << " in map : " << itStruct.strName << std::endl;
						return false;
					}
				}
				for (auto& it : itStruct.vRepeat)
				{
					if (it.bOwner && !AddName(itStruct.strGroup, it.strRef))
					{
						std::cout << "Error : " << "in repeat name : " << it.strName << " in struct : " << itStruct.strName << std::endl;
						return false;
					}
				}
			}
		}
	}

	return true;
}

bool ActorMgr::Generate()
{
	for (auto it : m_mFiles)
	{
		GenerateCPP stGen(&it.second);
		stGen.GetDataGroup = [this](const std::string& strName) ->DataGroup*
		{
			return m_pDataMgr->GetDataGroup(strName);
		};
		if (!stGen.Generate())
		{
			LOG_FILE
			std::cout << "Error: gen cpp file fail in actor file:" << it.first << std::endl;
			return false;
		}
		std::string strOut = FileUtils::PathSub(it.first, m_strXmlPath);
		strOut = m_strCppPath + strOut;
		std::string::size_type nPos = strOut.rfind(".");
		strOut[nPos + 1] = 'h';
		strOut.erase(nPos + 2, 2);

		std::ofstream fs(strOut);
		if (fs.is_open())
		{
			fs.clear();
			fs << stGen.GetResult().str();
			fs.flush();
			fs.close();
		}
		else
		{
			LOG_FILE
			std::cout << "Error : write file fail! please check file directory. file name : " << strOut << std::endl;
			return false;
		}
	}
	return Check();
}
