#include <DataDesc.h>
#include <iostream>
#include <set>
#include <map>
#include <rapidxml_utils.hpp>
#include <FileUtils.h>

bool checkDataName(const std::string & strName)
{
	if (strName.find('.', 0) != std::string::npos)
	{
		LOG_FILE
		std::cout << "Error : name can't use . symbol where name = " << strName << std::endl;
		return false;
	}
	return true;
}

bool checkType(const std::string & strName)
{
	if (strName == "uint64_t" || strName == "int64_t" 
		|| strName == "int32_t" || strName == "uint32_t" 
		|| strName=="bool" || strName=="datetime" || strName == "string"
		|| strName == "float" || strName == "double")
	{
		return true;
	}
	LOG_FILE;
	std::cout << "Error : type must one of : uint64_t int64_t int32_t uint32_t bool datetime string float double" << std::endl;
	return false;
}


bool DataProperty::Parse(rapidxml::xml_node<>* pNode)
{
	if (pNode == nullptr) return false;
	rapidxml::xml_attribute<>* pAtt = pNode->first_attribute();
	while (pAtt != nullptr)
	{
		if (pAtt->name() == std::string("name"))
		{
			strName = pAtt->value();
			if (!checkDataName(strName)) return false;
		}
		else if(pAtt->name() == std::string("type"))
		{
			strType = pAtt->value();
			if (!checkType(strType)) return false;
		}
		else if (pAtt->name() == std::string("desc"))
		{
			strDesc = pAtt->value();
		}
		else if (pAtt->name() == std::string("index"))
		{
			iIndex = atoi(pAtt->value());
		}

		pAtt = pAtt->next_attribute();
	}
	if (strName.empty() || strType.empty() || iIndex == -1)
	{
		LOG_FILE;
		std::cout << "Error : property node need name and type ¡¢Index" << std::endl;
		return false;
	}
	if (strName == "Key" || strName == "Index")
	{
		LOG_FILE;
		LOG_ERROR("Key¡¢Index can't use in property name.");
		return false;
	}
	return true;
}

bool DataRepeated::Parse(rapidxml::xml_node<>* pNode)
{
	if (pNode == nullptr) return false;
	rapidxml::xml_attribute<>* pAtt = pNode->first_attribute();
	while (pAtt != nullptr)
	{
		if (pAtt->name() == std::string("name"))
		{
			strName = pAtt->value();
			if (!checkDataName(strName)) return false;
		}
		else if(pAtt->name() == std::string("index"))
		{
			iIndex = atoi(pAtt->value());
		}
		else if (pAtt->name() == std::string("desc"))
		{
			strDesc = pAtt->value();
		}
		else if (pAtt->name() == std::string("DB"))
		{
			strDB = pAtt->value();
		}

		pAtt = pAtt->next_attribute();
	}
	if (strName.empty() || strDB.empty())
	{
		LOG_FILE;
		LOG_ERROR("repeat node need name and DB. ");
		return false;
	}
	if (iIndex == -1)
	{
		LOG_FILE;
		LOG_ERROR("repeat node need index ");
		return false;
	}
	rapidxml::xml_node<>* pProNode = pNode->first_node("property");
	while (pProNode != nullptr)
	{
		DataProperty stPro;
		if (!stPro.Parse(pProNode))
		{
			LOG_FILE
			std::cout << "Error : property in repeat :" << strName << std::endl;
		}
		vProperty.push_back(stPro);
		pProNode = pProNode->next_sibling("property");
	}

	return Check();
}

bool DataRepeated::Check()
{
	std::set<std::string> vName;
	for (auto& it: vProperty)
	{
		if (vName.find(it.strName) != vName.end())
		{
			LOG_FILE;
			std::cout << "Error : property name repeated name = " << it.strName << std::endl;
			return false;
		}
		vName.insert(it.strName);
	}
	std::set<int32_t> vIndex;
	for (auto & it:vProperty)
	{
		if (vIndex.find(it.iIndex) != vIndex.end())
		{
			LOG_FILE;
			std::cout << "Error : property index existed name = " << it.strName << std::endl;
			return false;
		}
		vIndex.insert(it.iIndex);
	}
	static std::set < std::string > vDB;
	if (!strDB.empty())
	{
		if (vDB.find(strDB) != vDB.end())
		{
			LOG_FILE;
			LOG_ERROR("Repeated DB existed, Repeated name = " << strName);
			return false;
		}
		vDB.insert(strDB);
	}
	return true;
}

DataProperty * DataRepeated::GetProperty(const std::string & strName)
{
	for (auto& it: vProperty)
	{
		if (it.strName == strName)
		{
			return &it;
		}
	}
	return nullptr;
}


DataNode* DataRepeated::GetNode(std::list<std::string> listName)
{
	
	for (auto& it:vProperty)
	{
		if (it.strName == listName.front())
		{
			return &it;
		}
	}
	return nullptr;
}

void DataRepeated::GenIndex(std::list<NameIndex>& listIndex, NameIndex stIndex)
{
	stIndex.strPackRepeat = strName;
	stIndex.strDB = strDB;
	stIndex.IsRepeated = true;
	stIndex.stIndex.Detail.Package = iIndex;
	listIndex.push_back(stIndex);
	for (auto& it : vProperty)
	{
		stIndex.strField = it.strName;
		stIndex.stIndex.Detail.Property = it.iIndex;
		stIndex.strType = it.strType;
		listIndex.push_back(stIndex);
	}
}

std::string DataRepeated::GenCode(const std::string & strGroup)
{
	std::stringstream ssOut;
	{
		std::string strClassName = strGroup + "_" + strName + "_data";
		ssOut << "struct " << strClassName << " : public Data::TStruct<Data::IRepeated>\n";
		ssOut << "{\n";

		ssOut << "\t" << strClassName << "()\n";
		ssOut << "\t{\n";
		for (auto& it : vProperty)
		{
			ssOut << "\t\t" << it.strName << ".SetDataHandle(this);\n";
		}
		ssOut << "\t}\n";

		ssOut << "\t" << strClassName << "(const " << strClassName << "& other)\n";
		ssOut << "\t\t: ";
		for (size_t i = 0; i < vProperty.size(); ++i)
		{
			if (i != 0)
				ssOut << "\t\t, ";
			auto& it = vProperty[i];
			ssOut << it.strName << "(other." << it.strName << ")\n";
		}
		ssOut << "\t{\n";
		for (auto& it : vProperty)
		{
			ssOut << "\t\t" << it.strName << ".SetDataHandle(this);\n";
		}
		ssOut << "\t}\n\n";

		for (auto& it : vProperty)
		{
			ssOut << "\tusing " << it.strName << "Type = Data::TValue<" << DataFile::RealType(it.strType) << ", " << strClassName << ", " << strGroup << "_" << strName << "_" << it.strName << ">;\n";
		}

		for (auto& it : vProperty)
		{
			ssOut << "\t" << it.strName << "Type " << it.strName << "; //" << it.strDesc << "\n";
		}
		ssOut << "};\n\n";
	}
	return ssOut.str();
}

bool DataPackage::Parse(rapidxml::xml_node<>* pNode)
{
	if (pNode == nullptr) return false;
	rapidxml::xml_attribute<>* pAtt = pNode->first_attribute();
	while (pAtt != nullptr)
	{
		if (pAtt->name() == std::string("name"))
		{
			strName = pAtt->value();
			if (!checkDataName(strName)) return false;
		}
		else if(pAtt->name() == std::string("index"))
		{
			iIndex = atoi(pAtt->value());
		}
		else if (pAtt->name() == std::string("desc"))
		{
			strDesc = pAtt->value();
		}
		else if(pAtt->name() == std::string("DB"))
		{
			strDB = pAtt->value();
		}
		pAtt = pAtt->next_attribute();
	}
	if (strName.empty() )
	{
		LOG_FILE;
		LOG_ERROR("package node need name .");
		return false;
	}
	if (iIndex == -1)
	{
		LOG_FILE;
		LOG_ERROR("package node need index ");
		return false;
	}
	{
		rapidxml::xml_node<>* pProNode = pNode->first_node("property");
		while (pProNode != nullptr)
		{
			DataProperty stPro;
			if (!stPro.Parse(pProNode))
			{
				LOG_FILE;
				std::cout << "Error : property in package :" << strName << std::endl;
			}
			vProperty.push_back(stPro);
			pProNode = pProNode->next_sibling("property");
		}
	}

	return Check();
}

bool DataPackage::Check()
{
	std::set<std::string> vName;
	std::set<int32_t> vIndex;
	for (auto& it : vProperty)
	{
		if (vName.find(it.strName) != vName.end())
		{
			LOG_FILE;
			std::cout << "Error : property name repeated, name = " << it.strName << std::endl;
			return false;
		}
		if (vIndex.find(it.iIndex) != vIndex.end())
		{
			LOG_FILE;
			std::cout << "Error : property index repeated, name = " << it.strName << std::endl;
			return false;
		}
		vName.insert(it.strName);
		vIndex.insert(it.iIndex);
	}
	return true;
}

DataProperty * DataPackage::GetProperty(const std::string & strName)
{
	for (auto &it : vProperty)
	{
		if (it.strName == strName)
		{
			return &it;
		}
	}
	return nullptr;
}


DataNode* DataPackage::GetNode(std::list<std::string> listName)
{
	for (auto& it:vProperty)
	{
		if (it.strName == listName.front())
		{
			listName.pop_front();
			if (listName.size() > 0)
			{
				return it.GetNode(listName);
			}
			return &it;
		}
	}

	return nullptr;
}

void DataPackage::GenIndex(std::list < NameIndex >& listIndex, NameIndex stIndex)
{
	stIndex.strPackRepeat = strName;
	stIndex.strDB = strDB;
	stIndex.IsRepeated = false;
	stIndex.stIndex.Detail.Package = iIndex;
	listIndex.push_back(stIndex);
	for (auto& it:vProperty)
	{
		stIndex.strField = it.strName;
		stIndex.stIndex.Detail.Property = it.iIndex;
		stIndex.strType = it.strType;
		listIndex.push_back(stIndex);
	}
}

bool DataGroup::Parse(rapidxml::xml_node<>* pNode)
{
	if (pNode == nullptr) return false;
	rapidxml::xml_attribute<>* pAtt = pNode->first_attribute();
	while (pAtt != nullptr)
	{
		if (pAtt->name() == std::string("name"))
		{
			strName = pAtt->value();
			if (!checkDataName(strName)) return false;
		}
		else if(pAtt->name() == std::string("index"))
		{
			iIndex = atoi(pAtt->value());
		}
		pAtt = pAtt->next_attribute();
	}
	if (strName.empty())
	{
		LOG_FILE;
		std::cout << "Error : group node need name " << std::endl;
		return false;
	}
	if (iIndex == -1)
	{
		LOG_FILE;
		std::cout << "Error : group node need index " << std::endl;
		return false;
	}

	{
		rapidxml::xml_node<>* pPackageNode = pNode->first_node("package");
		while (pPackageNode != nullptr)
		{
			DataPackage stData;
			if (!stData.Parse(pPackageNode))
			{
				LOG_FILE;
				std::cout << "Error : package in group :" << strName << std::endl;
			}
			stData.strGroup = strName;
			vPackage.push_back(stData);
			pPackageNode = pPackageNode->next_sibling("package");
		}
	}
	{
		rapidxml::xml_node<>* pRepeatedNode = pNode->first_node("repeated");
		while (pRepeatedNode != nullptr)
		{
			DataRepeated stData;
			if (!stData.Parse(pRepeatedNode))
			{
				LOG_FILE;
				std::cout << "Error : repeated in package :" << strName << std::endl;
			}
			stData.strGroup = strName;
			vRepeated.push_back(stData);
			pRepeatedNode = pRepeatedNode->next_sibling("repeated");
		}
	}
	return Check();
}

bool DataGroup::Check()
{
	std::set<std::string> vName;
	std::set<int32_t> vIndex;
	for (auto& it : vPackage)
	{
		if (vName.find(it.strName) != vName.end())
		{
			LOG_FILE;
			std::cout << "Error : package name repeated, name = " << it.strName << std::endl;
			return false;
		}
		if (vIndex.find(it.iIndex) != vIndex.end())
		{
			LOG_FILE;
			std::cout << "Error : package index repeated, name = " << it.strName << std::endl;
			return false;
		}
		vName.insert(it.strName);
		vIndex.insert(it.iIndex);
	}
	for (auto& it : vRepeated)
	{
		if (vName.find(it.strName) != vName.end())
		{
			LOG_FILE;
			std::cout << "Error : repeated name repeated, name = " << it.strName << std::endl;
			return false;
		}
		if (vIndex.find(it.iIndex) != vIndex.end())
		{
			LOG_FILE;
			std::cout << "Error : repeated index repeated, name = " << it.strName << std::endl;
			return false;
		}
		vName.insert(it.strName);
	}
	return true;
}

DataPackage * DataGroup::GetPacket(const std::string & strName)
{
	for (auto &it : vPackage)
	{
		if (it.strName == strName)
		{
			return &it;
		}
	}
	return nullptr;
}

DataRepeated * DataGroup::GetRepeated(const std::string & strName)
{
	for (auto &it : vRepeated)
	{
		if (it.strName == strName)
		{
			return &it;
		}
	}
	return nullptr;
}

DataNode* DataGroup::GetNode(std::list<std::string> listName)
{

	for (auto& it : vPackage)
	{
		if (it.strName == listName.front())
		{
			listName.pop_front();
			if (listName.size() > 0)
			{
				return it.GetNode(listName);
			}
			return &it;
		}
	}
	for (auto& it : vRepeated)
	{
		if (it.strName == listName.front())
		{
			listName.pop_front();
			if (listName.size() > 0)
			{
				return it.GetNode(listName);
			}
			return &it;
		}
	}
	return nullptr;
}

void DataGroup::GenIndex(std::list<NameIndex>& listIndex)
{
	for (auto& it:vPackage)
	{
		NameIndex stIndex;
		stIndex.strGroup = strName;
		stIndex.stIndex.Detail.Group = iIndex;
		it.GenIndex(listIndex, stIndex);
	}
	for (auto& it : vRepeated)
	{
		NameIndex stIndex;
		stIndex.strGroup = strName;
		stIndex.stIndex.Detail.Group = iIndex;
		it.GenIndex(listIndex, stIndex);
	}
}

bool DataFile::Parse(const std::string & strFile)
{
	try
	{
		rapidxml::file<> file(strFile.c_str());
		rapidxml::xml_document<> doc;
		doc.parse<0>(file.data());
		strFileName = strFile;
		rapidxml::xml_node<>* pActorNode = doc.first_node("group");
		while (pActorNode != nullptr)
		{
			DataGroup stGroup;
			if (!stGroup.Parse(pActorNode))
			{
				LOG_FILE;
				std::cout << "Error: parse failed file name = " << strFile;
				return false;
			}
			vGroup.push_back(stGroup);
			pActorNode = pActorNode->next_sibling("group");
		}
		return Check();

	}
	catch (const std::exception& e)
	{
		LOG_FILE;
		std::cout << e.what() << std::endl;
		return false;
	}
}

bool DataFile::Check()
{
	static std::set<std::string> vName;
	static std::set<int32_t> vIndex;
	for (auto& it : vGroup)
	{
		if (vName.find(it.strName) != vName.end())
		{
			LOG_FILE;
			std::cout << "Error : group name repeated, name = " << it.strName << std::endl;
			return false;
		}
		if (vIndex.find(it.iIndex) != vIndex.end())
		{
			LOG_FILE;
			std::cout << "Error : group index repeated, name = " << it.strName << std::endl;
			return false;
		}
		vName.insert(it.strName);
		vIndex.insert(it.iIndex);
	}
	return true;
}

DataGroup * DataFile::GetGroup(const std::string & strName)
{
	for (auto& it : vGroup)
	{
		if (it.strName == strName)
		{
			return &it;
		}
	}
	return nullptr;
}

DataNode* DataFile::GetNode(std::list<std::string> listName)
{
	for (auto& it : vGroup)
	{
		if (it.strName == listName.front())
		{
			listName.pop_front();
			if (listName.size() > 0)
			{
				return it.GetNode(listName);
			}
			return &it;
		}
	}
	return nullptr;
}

std::string	GenEnum(const std::list<NameIndex>& listIndex, const std::string& strGroup)
{
	std::stringstream ss;
	ss << "enum E_" << strGroup << "_Index\n";
	ss << "{\n";
	for (auto& it: listIndex)
	{
		ss << "\t" << it.strGroup << "_" << it.strPackRepeat ;
		if (!it.strField.empty())
			ss << "_" << it.strField;
		ss << " = " << it.stIndex.ToString() << ",\n";
	}
	ss << "};\n\n";
	return ss.str();
}

bool DataFile::Generate(std::string& strCode, std::string& strRegistKeyField)
{
	std::list<NameIndex> listIndex;
	std::stringstream ssRepeated;
	std::stringstream ssEnum;
	std::stringstream ssOut;
	ssOut << "#pragma once" << std::endl;
	ssOut << "#include <TData/TValue.h>" << std::endl;
	ssOut << "#include <TData/TStruct.h>" << std::endl;
	for (auto& it :vGroup)
	{
		std::list<NameIndex> GrouIndex;
		it.GenIndex(GrouIndex);
		ssEnum << GenEnum(GrouIndex, it.strName);
		listIndex.insert(listIndex.end(), GrouIndex.begin(), GrouIndex.end());
		for (auto& itRepeated:it.vRepeated)
		{
			ssRepeated << itRepeated.GenCode(it.strName);
		}
	}

	ssOut << ssEnum.str();
	std::stringstream ssRegist;
	std::string strInitClassName = FileUtils::GetFileName(strFileName) + "_Index_Regist";
	ssRegist << "class " << strInitClassName << "\n";
	ssRegist << "{\n";
	ssRegist << "public:\n";
	ssRegist << "\t" << strInitClassName << "(){\n";
	for (auto& it : listIndex)
	{
		if (!it.strField.empty())
		{
			ssRegist << "\t\tData::KeyFieldMgr::AddField(\"" << it.strField << "\", " << it.strGroup << "_" << it.strPackRepeat << "_" << it.strField <<", Data::FT_" << it.strType << ");\n";
		} 
		else
		{
			ssRegist << "\t\tData::KeyFieldMgr::AddKey(\"" << it.strGroup << "." << it.strPackRepeat << "\", " << it.strGroup << "_" << it.strPackRepeat << ");\n";
			if (!it.strDB.empty())
			{
				if (it.IsRepeated)
				{
					ssRegist << "\t\tData::KeyFieldMgr::AddRepeatedDB(\"";
				}
				else
				{
					ssRegist << "\t\tData::KeyFieldMgr::AddPackageDB(\"";
				}
				ssRegist << it.strDB << "\", " << it.strGroup << "_" << it.strPackRepeat << ");\n";
			}
		}
	}
	ssRegist << "\t}\n";
	ssRegist << "};\n";

	ssRegist << "static " << strInitClassName << " s_st" << strInitClassName << ";\n\n";

	ssOut << ssRepeated.str();
	strCode = ssOut.str();
	strRegistKeyField = ssRegist.str();
	return true;
}

std::string DataFile::RealType(const std::string & strType)
{
	if (strType == "datetime")
	{
		return "uint64_t";
	}
	else if (strType == "string")
	{
		return "std::string";
	}
	return strType;
}
