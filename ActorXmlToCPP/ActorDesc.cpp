#include <ActorDesc.h>
#include <rapidxml_utils.hpp>
#include <iostream>
#include <set>

bool checkActorName(const std::string & strName)
{
	if (strName.find('.', 0) != std::string::npos)
	{
		LOG_FILE
		std::cout << "Error : name can't use . symbol where name = " << strName << std::endl;
		return false;
	}
	return true;
}


bool ActorStructProperty::Parse(rapidxml::xml_node<>* pNode)
{
	if (pNode == nullptr) return false;
	rapidxml::xml_attribute<>* pAtt = pNode->first_attribute();
	bOwner = false;
	while (pAtt != nullptr)
	{
		if (pAtt->name() == std::string("key"))
		{
			strKey = pAtt->value();
			if (!checkActorName(strKey)) return false;
		}
		else if(pAtt->name() == std::string("field"))
		{
			strField = pAtt->value();
		}
		else if (pAtt->name() == std::string("owner"))
		{
			std::string strOwner = pAtt->value();
			if (strOwner == "true")
			{
				bOwner = true;
			}
		}
		else if (pAtt->name() == std::string("desc"))
		{
			strDesc = pAtt->value();
		}
		pAtt = pAtt->next_attribute();
	}
	if (strKey.empty() || strField.empty())
	{
		LOG_FILE
		std::cout << "Error : property node need key and field" << std::endl;
		return false;
	}
	return true;
}


bool ActorRepeatProperty::Parse(rapidxml::xml_node<>* pNode)
{
	if (pNode == nullptr) return false;
	rapidxml::xml_attribute<>* pAtt = pNode->first_attribute();
	bOwner = false;
	while (pAtt != nullptr)
	{
		if (pAtt->name() == std::string("field"))
		{
			strField = pAtt->value();
			if (!checkActorName(strField)) return false;
		}
		else if (pAtt->name() == std::string("owner"))
		{
			if (pAtt->value() == std::string("true"))
			{
				bOwner = true;
			}
		}
		pAtt = pAtt->next_attribute();
	}
	if (strField.empty())
	{
		LOG_FILE
		std::cout << "Error : repeat property node need field " << std::endl;
		return false;
	}
	return true;
}



bool ActorRepeat::Parse(rapidxml::xml_node<>* pNode)
{
	if (pNode == nullptr) return false;
	rapidxml::xml_attribute<>* pAtt = pNode->first_attribute();
	bOwner = false;
	while (pAtt != nullptr)
	{
		if (pAtt->name() == std::string("name"))
		{
			strName = pAtt->value();
			if (!checkActorName(strName)) return false;
		}
		else if (pAtt->name() == std::string("ref"))
		{
			strRef = pAtt->value();
		}
		else if (pAtt->name() == std::string("owner"))
		{
			std::string strOwner = pAtt->value();
			if (strOwner == "true")
			{
				bOwner = true;
			}
		}
		else if (pAtt->name() == std::string("desc"))
		{
			strDesc = pAtt->value();
		}
		pAtt = pAtt->next_attribute();
	}
	if (strName.empty() || strRef.empty())
	{
		LOG_FILE
		std::cout << "Error : repeat node need name ref " << std::endl;
		return false;
	}
	if (!bOwner)
	{
		rapidxml::xml_node<>* pProNode = pNode->first_node("property");
		while (pProNode != nullptr)
		{
			ActorRepeatProperty stPro;
			if (!stPro.Parse(pProNode))
			{
				LOG_FILE
				std::cout << "Error : property in repeat :" << strName << std::endl;
			}
			vProperty.push_back(stPro);
			pProNode = pProNode->next_sibling("property");
		}
		if (vProperty.empty())
		{
			LOG_FILE
			std::cout << "Error : because owner is false, so need property in repeat:"<< strName << std::endl;
			return false;
		}
	}
	return Check();
}

bool ActorRepeat::Check()
{
	std::set<std::string> vName;
	for (auto& it: vProperty)
	{
		if (vName.find(it.strField) != vName.end())
		{
			LOG_FILE
			std::cout << "Error : property name repeated field = " << it.strField << std::endl;
			return false;
		}
		vName.insert(it.strField);
	}
	return true;
}

ActorRepeatProperty * ActorRepeat::GetProperty(const std::string & strName)
{
	for (auto& it: vProperty)
	{
		if (it.strField == strName)
		{
			return &it;
		}
	}
	return nullptr;
}

bool ActorStruct::Parse(rapidxml::xml_node<>* pNode)
{
	if (pNode == nullptr) return false;
	rapidxml::xml_attribute<>* pAtt = pNode->first_attribute();
	while (pAtt != nullptr)
	{
		if (pAtt->name() == std::string("name"))
		{
			strName = pAtt->value();
			if (!checkActorName(strName)) return false;
		}
		else if (pAtt->name() == std::string("group"))
		{
			strGroup = pAtt->value();
		}
		else if (pAtt->name() == std::string("desc"))
		{
			strDesc = pAtt->value();
		}
		pAtt = pAtt->next_attribute();
	}
	if (strName.empty()|| strGroup.empty())
	{
		LOG_FILE
		std::cout << "Error : package node need name and group " << std::endl;
		return false;
	}

	{
		rapidxml::xml_node<>* pProNode = pNode->first_node("property");
		while (pProNode != nullptr)
		{
			ActorStructProperty stPro;
			if (!stPro.Parse(pProNode))
			{
				LOG_FILE
				std::cout << "Error : property in struct :" << strName << std::endl;
			}
			vProperty.push_back(stPro);
			pProNode = pProNode->next_sibling("property");
		}
	}

	{
		rapidxml::xml_node<>* pRepeatNode = pNode->first_node("repeat");
		while (pRepeatNode != nullptr)
		{
			ActorRepeat stActor;
			if (!stActor.Parse(pRepeatNode))
			{
				LOG_FILE
				std::cout << "Error : repeat in struct :" << strName << std::endl;
			}
			vRepeat.push_back(stActor);
			pRepeatNode = pRepeatNode->next_sibling("repeat");
		}

	}

	return Check();
}

bool ActorStruct::Check()
{
	std::set<std::string> vName;
	for (auto& it : vProperty)
	{
		if (vName.find(it.strField) != vName.end())
		{
			LOG_FILE
			std::cout << "Error : property field repeated, field = " << it.strField << std::endl;
			return false;
		}
		vName.insert(it.strField);
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

ActorStructProperty * ActorStruct::GetProperty(const std::string & strName)
{
	for (auto &it : vProperty)
	{
		if (it.strKey == strName)
		{
			return &it;
		}
	}
	return nullptr;
}

ActorRepeat * ActorStruct::GetRepeat(const std::string & strName)
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

bool ActorMap::Parse(rapidxml::xml_node<>* pNode)
{
	if (pNode == nullptr) return false;
	rapidxml::xml_attribute<>* pAtt = pNode->first_attribute();
	while (pAtt != nullptr)
	{
		if (pAtt->name() == std::string("name"))
		{
			strName = pAtt->value();
			if (!checkActorName(strName)) return false;
		}
		else if (pAtt->name() == std::string("group"))
		{
			strGroup = pAtt->value();
		}
		else if (pAtt->name() == std::string("desc"))
		{
			strDesc = pAtt->value();
		}
		pAtt = pAtt->next_attribute();
	}
	if (strName.empty() || strGroup.empty())
	{
		LOG_FILE
		std::cout << "Error : map node need name and group" << std::endl;
		return false;
	}

	{
		rapidxml::xml_node<>* pProNode = pNode->first_node("property");
		while (pProNode != nullptr)
		{
			ActorStructProperty stPro;
			if (!stPro.Parse(pProNode))
			{
				LOG_FILE
				std::cout << "Error : property in struct :" << strName << std::endl;
			}
			vProperty.push_back(stPro);
			pProNode = pProNode->next_sibling("property");
		}
	}

	{
		rapidxml::xml_node<>* pRepeatNode = pNode->first_node("repeat");
		while (pRepeatNode != nullptr)
		{
			ActorRepeat stActor;
			if (!stActor.Parse(pRepeatNode))
			{
				LOG_FILE
				std::cout << "Error : repeat in struct :" << strName << std::endl;
			}
			vRepeat.push_back(stActor);
			pRepeatNode = pRepeatNode->next_sibling("repeat");
		}

	}

	return Check();
}

bool ActorMap::Check()
{
	std::set<std::string> vName;
	for (auto& it : vProperty)
	{
		if (vName.find(it.strKey) != vName.end())
		{
			LOG_FILE
			std::cout << "Error : map key repeated name = " << it.strKey << std::endl;
			return false;
		}
		vName.insert(it.strKey);
	}
	vName.clear();
	for (auto& it : vRepeat)
	{
		if (vName.find(it.strName) != vName.end())
		{
			LOG_FILE
			std::cout << "Error : map repeat name repeated name = " << it.strName << std::endl;
			return false;
		}
		vName.insert(it.strName);
	}
	return true;
}

bool ActorEntity::Parse(rapidxml::xml_node<>* pNode)
{
	if (pNode == nullptr) return false;
	rapidxml::xml_attribute<>* pAtt = pNode->first_attribute();
	while (pAtt != nullptr)
	{
		if (pAtt->name() == std::string("name"))
		{
			strName = pAtt->value();
			if (!checkActorName(strName)) return false;
		}
		pAtt = pAtt->next_attribute();
	}
	if (strName.empty())
	{
		LOG_FILE
		std::cout << "Error : actor node need name " << std::endl;
		return false;
	}

	{
		rapidxml::xml_node<>* pSubNode = pNode->first_node("struct");
		while (pSubNode != nullptr)
		{
			ActorStruct stData;
			if (!stData.Parse(pSubNode))
			{
				LOG_FILE
				std::cout << "Error : struct in package :" << strName << std::endl;
			}
			vStruct.push_back(stData);
			pSubNode = pSubNode->next_sibling("struct");
		}
	}
	{
		rapidxml::xml_node<>* pSubNode = pNode->first_node("map");
		while (pSubNode != nullptr)
		{
			ActorMap stData;
			if (!stData.Parse(pSubNode))
			{
				LOG_FILE
				std::cout << "Error : map in package :" << strName << std::endl;
			}
			vMap.push_back(stData);
			pSubNode = pSubNode->next_sibling("map");
		}
	}
	return Check();
}

bool ActorEntity::Check()
{
	std::set<std::string> vName;
	for (auto& it : vStruct)
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

ActorStruct * ActorEntity::GetStruct(const std::string & strName)
{
	for (auto &it : vStruct)
	{
		if (it.strName == strName)
		{
			return &it;
		}
	}
	return nullptr;
}

ActorMap * ActorEntity::GetMap(const std::string & strName)
{
	for (auto &it : vMap)
	{
		if (it.strName == strName)
		{
			return &it;
		}
	}
	return nullptr;
}


bool ActorFile::Parse(const std::string & strFile)
{
	try
	{
		strFullFileName = strFile;
		rapidxml::file<> file(strFile.c_str());
		rapidxml::xml_document<> doc;
		doc.parse<0>(file.data());

		rapidxml::xml_node<>* pActorNode = doc.first_node("actor");
		while (pActorNode != nullptr)
		{
			ActorEntity stActor;
			if (!stActor.Parse(pActorNode))
			{
				LOG_FILE
				std::cout << "Error: parse failed file name = " << strFile;
				return false;
			}
			vActor.push_back(stActor);
			pActorNode = pActorNode->next_sibling("actor");
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

bool ActorFile::Check()
{
	std::set<std::string> vName;
	for (auto& it : vActor)
	{
		if (vName.find(it.strName) != vName.end())
		{
			LOG_FILE
			std::cout << "Error : actor name repeated name = " << it.strName << std::endl;
			return false;
		}
		vName.insert(it.strName);
	}
	return true;
}
