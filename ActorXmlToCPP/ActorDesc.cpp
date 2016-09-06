#include <ActorDesc.h>
#include <rapidxml_utils.hpp>
#include <iostream>
#include <set>
#include <StringUtils.h>
#include <DataMgr.h>

bool checkActorName(const std::string & strName)
{
	if (strName.find('.', 0) != std::string::npos)
	{
		LOG_FILE;
		std::cout << "Error : name can't use . symbol where name = " << strName << std::endl;
		return false;
	}
	return true;
}

KeyString Split(const std::string & strString)
{
	std::list<std::string> listResult;
	StringUtils::Split(strString, listResult, ".");
	if (listResult.size() != 2)
	{
		LOG_FILE;
		std::cout << "Error: key must use . split, wrong key name = " << strString << std::endl;
		return std::make_tuple ( "", "");
	}
	return std::make_tuple(listResult.front(), listResult.back());
}


bool ActorProperty::Parse(rapidxml::xml_node<>* pNode)
{
	if (pNode == nullptr) return false;
	rapidxml::xml_attribute<>* pAtt = pNode->first_attribute();
	bOwner = false;
	while (pAtt != nullptr)
	{
		if (pAtt->name() == std::string("key"))
		{
			strKey = pAtt->value();
			//if (!checkActorName(strKey)) return false;
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
		LOG_FILE;
		std::cout << "Error : property node need key and field" << std::endl;
		return false;
	}
	if (bOwner)
	{
		std::string strKeyFiled = strKey + "." + strField;
		static std::set<std::string> setOwner;
		if (setOwner.find(strKeyFiled) != setOwner.end())
		{
			LOG_FILE;
			std::cout << "Error : property more than one owner, keyfield = " << strKeyFiled << std::endl;
			return false;
		}
		setOwner.insert(strKeyFiled);
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
		if (pAtt->name() == std::string("key"))
		{
			strkey = pAtt->value();
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
	if (strkey.empty())
	{
		LOG_FILE;
		std::cout << "Error : repeated node need key " << std::endl;
		return false;
	}
	if (bOwner)
	{
		static std::set<std::string> setKey;
		if (setKey.find(strkey) != setKey.end())
		{
			LOG_FILE;
			std::cout << "Error: repeated more zhan one owner, key = " << strkey << std::endl;
			return false;
		}
		setKey.insert(strkey);
	}
	std::list<std::string> listStr;
	StringUtils::Split(strkey, listStr, ".");
	if (listStr.size() > 0)
	{
		strName = listStr.back();
	}
	return true;
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
		else if (pAtt->name() == std::string("desc"))
		{
			strDesc = pAtt->value();
		}
		pAtt = pAtt->next_attribute();
	}
	if (strName.empty())
	{
		LOG_FILE;
		std::cout << "Error : package node need name " << std::endl;
		return false;
	}

	{
		rapidxml::xml_node<>* pProNode = pNode->first_node("property");
		while (pProNode != nullptr)
		{
			ActorProperty stPro;
			if (!stPro.Parse(pProNode))
			{
				LOG_FILE;
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
				LOG_FILE;
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
			LOG_FILE;
			std::cout << "Error : property field repeated, field = " << it.strField << std::endl;
			return false;
		}
		vName.insert(it.strField);
	}
	vName.clear();
	for (auto& it : vRepeat)
	{
		if (vName.find(it.strkey) != vName.end())
		{
			LOG_FILE;
			std::cout << "Error : repeat key repeated name = " << it.strkey << std::endl;
			return false;
		}
		vName.insert(it.strkey);
	}
	return true;
}

ActorProperty * ActorStruct::GetProperty(const std::string & strName)
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

bool ActorStruct::GenCode(std::stringstream& strCode, std::set<std::string>& vDataFile, const std::string& strActor)
{
	std::stringstream& ssOut(strCode);
	std::string strClassName = strActor + strName + "Data";
	ssOut << "struct " << strClassName << " : public Data::TStruct<Data::IDataAdapter>\n";
	ssOut << "{\n";

	ssOut << "\t" << strClassName << "()\n";
	ssOut << "\t{\n";
	for (auto& it : vProperty)
	{
		ssOut << "\t\t" << it.strField << ".SetDataHandle(this);\n";
	}
	for (auto& it:vRepeat)
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
		ssOut << it.strField << "(other." << it.strField << ")\n";
	}
	for (size_t i = 0; i < vRepeat.size(); ++i)
	{
		if (vProperty.size() > 0)
			ssOut << "\t\t, ";
		auto& it = vRepeat[i];
		ssOut << it.strName << "(other." << it.strName << ")\n";
	}
	ssOut << "\t{\n";
	for (auto& it : vProperty)
	{
		ssOut << "\t\t" << it.strField << ".SetDataHandle(this);\n";
	}
	for (auto& it : vRepeat)
	{
		ssOut << "\t\t" << it.strName << ".SetDataHandle(this);\n";
	}
	ssOut << "\t}\n\n";
	std::stringstream ssMember;
	for (auto& it:vProperty)
	{
		KeyString stKey = Split(it.strKey);
		DataGroup* pGroup = DataMgr::Instance().GetDataGroup(std::get<0>(stKey));
		if (pGroup == nullptr)
		{
			LOG_FILE;
			LOG_ERROR("have no group, name = " << std::get<0>(stKey));
			return false;
		}
		DataPackage* pPackage = pGroup->GetPacket(std::get<1>(stKey));
		if (pPackage == nullptr)
		{
			LOG_FILE;
			LOG_ERROR("have no package in group:" << pGroup->strName << ", name = " << std::get<1>(stKey));
			return false;
		}
		DataProperty* pPro = pPackage->GetProperty(it.strField);
		if (pPro == nullptr)
		{
			LOG_FILE;
			LOG_ERROR("have no field in :" << it.strKey <<", name = " << it.strField);
			return false;
		}
		vDataFile.insert(pGroup->strFileName);
		std::string strEnum = pGroup->strName + "_" + pPackage->strName + "_" + pPro->strName;
		if (it.bOwner)
		{
			ssOut << "\tusing " << pPro->strName << "Type = Data::TValue<" << DataFile::RealType(pPro->strType) << ", " << strClassName << ", " << strEnum << ">;\n";
		}
		else
		{
			ssOut << "\tusing " << pPro->strName << "Type = Data::TBaseValue<" << DataFile::RealType(pPro->strType) << ", " << strClassName << ", " << strEnum << ">;\n";
		}
		ssMember << "\t" << pPro->strName << "Type " << pPro->strName << "; //" << pPro->strDesc << "\n";
	}

	for (auto& it : vRepeat)
	{
		KeyString stKey = Split(it.strkey);
		DataGroup* pGroup = DataMgr::Instance().GetDataGroup(std::get<0>(stKey));
		if (pGroup == nullptr)
		{
			LOG_FILE;
			LOG_ERROR("have no group, name = " << std::get<0>(stKey));
			return false;
		}
		DataRepeated* pRepeated = pGroup->GetRepeated(std::get<1>(stKey));
		if (pRepeated == nullptr)
		{
			LOG_FILE;
			LOG_ERROR("have no repeated in group:" << pGroup->strName << ", name = " << std::get<1>(stKey));
			return false;
		}
		vDataFile.insert(pGroup->strFileName);
		std::string strRepeatedClassName = pGroup->strName + "_" + pRepeated->strName + "_data";
		std::string strEnum = pGroup->strName + "_" + pRepeated->strName;
		if (it.bOwner)
		{
			ssOut << "\tusing " << it.strName << "Type = Data::TRepeated<" << strRepeatedClassName << ", " << strClassName << ", " << strEnum << "> ;";
		}
		else
		{
			ssOut << "\tusing " << it.strName << "Type = Data::TBaseRepeat<" << strRepeatedClassName << ", " << strClassName << ", " << strEnum << "> ;";
		}
		ssMember << "\t" << it.strName << "Type " << it.strName << "; //" << pRepeated->strDesc << "\n";
	}

	ssOut << "\n\n";
	ssOut << ssMember.str();
	ssOut << "};\n\n";

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
		LOG_FILE;
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
				LOG_FILE;
				std::cout << "Error : struct in package :" << strName << std::endl;
			}
			vStruct.push_back(stData);
			pSubNode = pSubNode->next_sibling("struct");
		}
	}
	
	return Check();
}

bool ActorEntity::Check()
{
	static std::set<std::string> vName;
	for (auto& it : vStruct)
	{
		if (vName.find(it.strName) != vName.end())
		{
			LOG_FILE;
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

bool ActorEntity::GenCode(std::stringstream& ssCode, std::set<std::string>& vDataFile)
{
	for (auto& it:vStruct)
	{
		if (!it.GenCode(ssCode, vDataFile, strName))
			return false;
	}
	return true;
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
				LOG_FILE;
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
		LOG_FILE;
		std::cout << e.what() << std::endl;
		return false;
	}
}

bool ActorFile::Check()
{
	static std::set<std::string> vName;
	for (auto& it : vActor)
	{
		if (vName.find(it.strName) != vName.end())
		{
			LOG_FILE;
			std::cout << "Error : actor name repeated name = " << it.strName << std::endl;
			return false;
		}
		vName.insert(it.strName);
	}
	return true;
}

bool ActorFile::Generate(std::string& strOut)
{
	std::stringstream ssOut;
	std::stringstream ssCode;
	std::set<std::string> vDataFile;
	for (auto& it:vActor)
	{
		if (!it.GenCode(ssCode, vDataFile))
			return false;
	}
	ssOut << "#pragma once" << std::endl;
	ssOut << "#include <TData/TValue.h>" << std::endl;
	ssOut << "#include <TData/TRepeated.h>" << std::endl;
	ssOut << "#include <TData/TStruct.h>" << std::endl;
	ssOut << "#include <TData/IDataAdapter.h>" << std::endl;
	for (auto& it:vDataFile)
	{
		ssOut << "#include <" << it << ".h>" << std::endl;
	}

	ssOut << "\n" << ssCode.str() << std::endl;
	strOut = ssOut.str();
	return true;
}
