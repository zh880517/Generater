#include <DataDesc.h>
#include <iostream>
#include <set>
#include <rapidxml_utils.hpp>

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
		else if (pAtt->name() == std::string("default"))
		{
			strDefault = pAtt->value();
		}

		pAtt = pAtt->next_attribute();
	}
	if (strName.empty() || strType.empty())
	{
		LOG_FILE
		std::cout << "Error : property node need name and type" << std::endl;
		return false;
	}
	return true;
}

bool DataKey::Parse(rapidxml::xml_node<>* pNode)
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
		else if (pAtt->name() == std::string("type"))
		{
			strType = pAtt->value();
			if (!checkType(strType)) return false;
		}
		else if (pAtt->name() == std::string("desc"))
		{
			strDesc = pAtt->value();
		}
		pAtt = pAtt->next_attribute();
	}
	if (strName.empty() || strType.empty())
	{
		LOG_FILE
		std::cout << "Error : key node need name and type" << std::endl;
		return false;
	}
	return true;
}

bool DataRepeat::Parse(rapidxml::xml_node<>* pNode)
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
		else if (pAtt->name() == std::string("desc"))
		{
			strDesc = pAtt->value();
		}
		pAtt = pAtt->next_attribute();
	}
	if (strName.empty() )
	{
		LOG_FILE
		std::cout << "Error : repeat node need name " << std::endl;
		return false;
	}
	rapidxml::xml_node<>* pKeyNode = pNode->first_node("key");
	if (pKeyNode == nullptr)
	{
		LOG_FILE
		std::cout << "Error : repeat node need key : where repeat name = " << strName;
		return false;
	}
	if (!stKey.Parse(pKeyNode))
	{
		LOG_FILE
		std::cout << "Error : key in repeat : " << strName << std::endl;
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

bool DataRepeat::Check()
{
	std::set<std::string> vName;
	for (auto& it: vProperty)
	{
		if (vName.find(it.strName) != vName.end())
		{
			LOG_FILE
			std::cout << "Error : property name repeated name = " << it.strName << std::endl;
			return false;
		}
		vName.insert(it.strName);
	}
	return true;
}

DataProperty * DataRepeat::GetProperty(const std::string & strName)
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


DataNode* DataRepeat::GetNode(std::list<std::string> listName)
{
	if (stKey.strName == listName.front())
	{
		if (listName.size() == 1)
		{
			return &stKey;
		}
		else
		{
			listName.pop_front();
			return stKey.GetNode(listName);
		}
	}
	return nullptr;
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
		else if (pAtt->name() == std::string("desc"))
		{
			strDesc = pAtt->value();
		}
		pAtt = pAtt->next_attribute();
	}
	if (strName.empty())
	{
		LOG_FILE
		std::cout << "Error : package node need name " << std::endl;
		return false;
	}

	{
		rapidxml::xml_node<>* pProNode = pNode->first_node("property");
		while (pProNode != nullptr)
		{
			DataProperty stPro;
			if (!stPro.Parse(pProNode))
			{
				LOG_FILE
				std::cout << "Error : property in package :" << strName << std::endl;
			}
			vProperty.push_back(stPro);
			pProNode = pProNode->next_sibling("property");
		}
	}

	{
		rapidxml::xml_node<>* pRepeatNode = pNode->first_node("repeat");
		while (pRepeatNode != nullptr)
		{
			DataRepeat stData;
			if (!stData.Parse(pRepeatNode))
			{
				LOG_FILE
				std::cout << "Error : repeat in package :" << strName << std::endl;
			}
			vRepeat.push_back(stData);
			pRepeatNode = pRepeatNode->next_sibling("repeat");
		}

	}

	return Check();
}

bool DataPackage::Check()
{
	std::set<std::string> vName;
	for (auto& it : vProperty)
	{
		if (vName.find(it.strName) != vName.end())
		{
			LOG_FILE
			std::cout << "Error : property name repeated name = " << it.strName << std::endl;
			return false;
		}
		vName.insert(it.strName);
	}
	vName.clear();
	for (auto& it : vRepeat)
	{
		if (vName.find(it.strName) != vName.end())
		{
			LOG_FILE
			std::cout << "Error : repeat name repeated name = " << it.strName << std::endl;
			return false;
		}
		vName.insert(it.strName);
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

DataRepeat * DataPackage::GetRepeat(const std::string & strName)
{
	for (auto &it : vRepeat)
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

	for (auto& it : vRepeat)
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
		pAtt = pAtt->next_attribute();
	}
	if (strName.empty())
	{
		LOG_FILE
		std::cout << "Error : group node need name " << std::endl;
		return false;
	}
	{
		rapidxml::xml_node<>* pKeyNode = pNode->first_node("key");
		if (pKeyNode == nullptr)
		{
			LOG_FILE
			std::cout << "Error : repeat node need key : where repeat name = " << strName;
			return false;
		}
		if (!stKey.Parse(pKeyNode))
		{
			LOG_FILE
			std::cout << "Error : key in repeat : " << strName << std::endl;
			return false;
		}
	}

	{
		rapidxml::xml_node<>* pPackageNode = pNode->first_node("package");
		while (pPackageNode != nullptr)
		{
			DataPackage stData;
			if (!stData.Parse(pPackageNode))
			{
				LOG_FILE
				std::cout << "Error : repeat in package :" << strName << std::endl;
			}
			vPackage.push_back(stData);
			pPackageNode = pPackageNode->next_sibling("package");
		}
	}
	return Check();
}

bool DataGroup::Check()
{
	std::set<std::string> vName;
	for (auto& it : vPackage)
	{
		if (vName.find(it.strName) != vName.end())
		{
			LOG_FILE
			std::cout << "Error : package name repeated name = " << it.strName << std::endl;
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

DataNode* DataGroup::GetNode(std::list<std::string> listName)
{
	if (stKey.strName == listName.front())
	{
		listName.pop_front();
		if (listName.size() > 0)
		{
			return stKey.GetNode(listName);
		}
		return &stKey;
	}

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
	return nullptr;
}

bool DataFile::Parse(const std::string & strFile)
{
	try
	{
		rapidxml::file<> file(strFile.c_str());
		rapidxml::xml_document<> doc;
		doc.parse<0>(file.data());

		rapidxml::xml_node<>* pActorNode = doc.first_node("group");
		while (pActorNode != nullptr)
		{
			DataGroup stGroup;
			if (!stGroup.Parse(pActorNode))
			{
				LOG_FILE
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
		LOG_FILE
		std::cout << e.what() << std::endl;
		return false;
	}
}

bool DataFile::Check()
{
	std::set<std::string> vName;
	for (auto& it : vGroup)
	{
		if (vName.find(it.strName) != vName.end())
		{
			LOG_FILE
			std::cout << "Error : group name repeated name = " << it.strName << std::endl;
			return false;
		}
		vName.insert(it.strName);
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
