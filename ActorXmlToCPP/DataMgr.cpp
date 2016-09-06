#include "DataMgr.h"
#include <FileUtils.h>
#include <fstream>
#include <set>
bool DataMgr::Load(const std::string & strXmlPath, const std::string& strPath)
{
	std::list<FileSystem::path> listFile;
	FileUtils::GetAllFile(strXmlPath, listFile, ".xml");
	std::stringstream ssRegist;
	ssRegist << "#include \"stdafx.h\"\n";
	ssRegist << "#include <TData/keyFieldMgr.h>\n";
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
				for (auto& itPackage:itGroup.vPackage)
				{
					if (!itPackage.strDB.empty())
					{
						m_PackageDB[itPackage.strDB].push_back(&itPackage);
					}
				}
				for (auto& itRepeated : itGroup.vRepeated)
				{
					if (!itRepeated.strDB.empty())
					{
						m_RepeatedDB[itRepeated.strDB]= &itRepeated;
					}
				}
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

bool DataMgr::GenerateSQL(std::string strFilePath)
{
	try
	{
		std::stringstream ss;
		for (auto & it : m_PackageDB)
		{
			ss << "CREATE TABLE `" << it.first << "` (\n";
			ss << "\t`Key` bigint(20) NOT NULL AUTO_INCREMENT,\n";
			ss << "\tPRIMARY KEY (`Key`)\n";
			ss << ") ENGINE=InnoDB DEFAULT CHARSET=utf8;\n" << std::endl;
			std::set<std::string> vName;
			for (auto& itPacakege : it.second)
			{
				for (auto& itField : itPacakege->vProperty)
				{
					if (vName.find(itField.strName) != vName.end())
					{
						LOG_FILE;
						LOG_ERROR("repeated field name in table:" << it.first << " filed name:" << itField.strName << " in pacake:" << itPacakege->strName);
						return false;
					}
					vName.insert(itField.strName);
					ss << "alter table " << it.first << " add " << itField.strName << " " << TypeToSql(itField.strType) << "/* COMMENT '" << itField.strDesc << "'*/;\n";
				}
			}
			ss << "\n\n";
		}

		for (auto &it : m_RepeatedDB)
		{
			ss << "CREATE TABLE `" << it.first << "` (\n";
			ss << "\t`Key` bigint(20) NOT NULL,\n";
			ss << "\t`Index` bigint(20) NOT NULL,\n";
			ss << "\tPRIMARY KEY (`Key`,`Index`)\n";
			ss << ") ENGINE=InnoDB DEFAULT CHARSET=utf8;\n" << std::endl;
			for (auto& itField : it.second->vProperty)
			{
				ss << "alter table " << it.first << " add " << itField.strName << " " << TypeToSql(itField.strType) << " /*COMMENT '" << itField.strDesc << "'*/;\n";
			}
			ss << "\n\n";
		}

		WriteFile(strFilePath, ss.str(), false);

	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
		return false;
	}
	
	return true;
}

DataMgr & DataMgr::Instance()
{
	static DataMgr s_Instance;
	return s_Instance;
}

std::string DataMgr::TypeToSql(const std::string & strType)
{
	if (strType == "uint32_t")
	{
		return "INT(11) UNSIGNED NOT NULL DEFAULT '0'";
	}
	else if(strType == "int32_t")
	{
		return "INT(11) NOT NULL DEFAULT '0'";
	}
	else if(strType == "uint64_t")
	{
		return "BIGINT(20) UNSIGNED NOT NULL DEFAULT '0'";
	}
	else if(strType == "int64_t")
	{
		return "BIGINT(20) NOT NULL DEFAULT '0'";
	}
	else if(strType == "float")
	{
		return "FLOAT NOT NULL DEFAULT '0.0'";
	}
	else if (strType == "double")
	{
		return "DOUBLE NOT NULL DEFAULT '1.0'";
	}
	else if(strType == "bool")
	{
		return "tinyint(1) NOT NULL DEFAULT '0'";
	}
	else if(strType == "datetime")
	{
		return "datetime NOT NULL DEFAULT '1970-01-01 00:00:00'";
	}
	else if(strType == "string")
	{
		return "TINYTEXT NOT NULL DEFAULT ''";
	}
	throw std::runtime_error("UnHandle SQL type!");
	return "";
}
