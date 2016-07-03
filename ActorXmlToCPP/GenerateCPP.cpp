#include <GenerateCPP.h>
#include <FileUtils.h>
#include <StringUtils.h>
#include <iostream>

GenerateCPP::GenerateCPP(ActorFile * pActorFile)
	: m_pActorFile(pActorFile)
{
}

bool GenerateCPP::Generate()
{
	m_ssOut << "#include <string>" << std::endl;
	m_ssOut << "#include <map>" << std::endl;
	m_ssOut << "#include <Name.h>" << std::endl;
	m_ssOut << "#include <UtilsMap.h>" << std::endl;
	for (auto& it : m_pActorFile->vActor)
	{
		if (!HandleActorEntry(it))
			return false;
	}
	return true;
}

bool GenerateCPP::HandleActorEntry(ActorEntity & actorEntity)
{
	m_listName.clear();
	m_listName.push_back(actorEntity.strName);
	for (auto& it : actorEntity.vStruct)
	{
		m_listName.push_back(it.strName);
		if (!HandleStruct(it))
			return false;
		m_listName.pop_back();
	}
	for (auto& it : actorEntity.vMap)
	{
		m_listName.push_back(it.strName);
		if (!HandleStruct(it)) //Map和Struct的内部结构其实完全一样，所以这里共用一个接口
			return false;
		m_listName.pop_back();
	}

	return GenerateActor(actorEntity);
}

bool GenerateCPP::HandleStruct(ActorStruct & actorStruct)
{
	DataGroup* pGroup = GetDataGroup(actorStruct.strGroup);
	if (pGroup == nullptr)
	{
		LOG_FILE
		std::cout << "Error : Group:" << actorStruct.strGroup << " not exited !" << std::endl;
		return false;
	}
	for (auto& it : m_listName)
	{
		actorStruct.strClassName += it;
	}
	actorStruct.strClassName += "Data";

	std::stringstream ssOut;
	for (auto& it : actorStruct.vRepeat)
	{
		m_listName.push_back(it.strName);
		if (!HandleRepeat(it, pGroup, ssOut))
			return false;
		m_listName.pop_back();
	}
	if (!GenerateStructClass(actorStruct, ssOut))
		return  false;

	m_ssOut << ssOut.str();
	return true;
}

std::string TranslateType(const std::string& strType)
{
	if (strType == "datetime")
	{
		return "uint32_t";
	}
	else if(strType == "string")
	{
		return "std::string";
	}
	return strType;
}

bool IsNumber(const std::string& strType)
{
	if (strType == "int32_t" || strType == "uint32_t" || strType == "int64_t" || strType == "uint64_t" || strType == "float" || strType == "double")
	{
		return true;
	}
	return false;
}

void Translate(Property& stRepeat, const DataProperty& stData )
{
	stRepeat.Name = stData.strName;
	stRepeat.Type = TranslateType(stData.strType);
	stRepeat.Default = stData.strDefault;
	if (stRepeat.Default.empty())
	{
		if (IsNumber(stData.strType))
		{
			stRepeat.Default = "0";
		}
		else if (stData.strType == "bool")
		{
			stRepeat.Default = "false";
		}
		else if(stData.strType == "string")
		{
			if (!stRepeat.Default.empty())
			{
				stRepeat.Default = "\"" + stRepeat.Default;
				stRepeat.Default += "\"";
			}
		}
	}
}

bool GenerateCPP::HandleRepeat(ActorRepeat & actorRepeat, DataGroup* pGroup, std::stringstream& ssRepeat)
{
	std::list<std::string> listName;
	StringUtils::Split(actorRepeat.strRef, listName, ".");
	DataRepeat* pRepeat = static_cast<DataRepeat*>(pGroup->GetNode(listName));
	if (pRepeat == nullptr)
	{
		LOG_FILE
		std::cout << "Error: can't find repeat ref:" << actorRepeat.strRef << " in group : " << pGroup->strName << std::endl;
		return false;
	}
	if (pRepeat->eType != eDataRepeat)
	{
		LOG_FILE
		std::cout << "Error: repeat ref:" << actorRepeat.strRef << " is not a repeat in group : " << pGroup->strName << std::endl;
		return false;
	}
	std::vector<Property> vPro;
	if (actorRepeat.bOwner)
	{
		for (auto& it:pRepeat->vProperty)
		{
			Property stPro;
			stPro.Owner = true;
			Translate(stPro, it);
			stPro.RedisKey = pGroup->strName + std::string(".") + actorRepeat.strRef;
			vPro.push_back(stPro);
		}
	}
	else
	{
		for (auto& it : actorRepeat.vProperty)
		{
			Property stPro;
			stPro.Owner = it.bOwner;
			DataProperty* pPro = pRepeat->GetProperty(it.strField);
			if (pPro == nullptr )
			{
				LOG_FILE
				std::cout << "Error: can't find field :" << it.strField << " in repeat : " << actorRepeat.strRef << std::endl;
				return false;
			}
			stPro.Owner = false;
			Translate(stPro, *pPro);
			stPro.RedisKey = pGroup->strName + std::string(".") + actorRepeat.strRef;
			vPro.push_back(stPro);
		}
	}
	for (auto& it : m_listName)
	{
		actorRepeat.strClassName += it;
	}
	actorRepeat.strClassName += "Data";
	ssRepeat << GenerateRepeatClass(vPro, actorRepeat).str();

	return true;
}

std::string GenerateProGet(Property& stPro)
{
	char buff[200];
	snprintf(buff, sizeof(buff), "\t%s Get%s()const { return %s; }\n", stPro.Type.c_str(), stPro.Name.c_str(), stPro.Name.c_str());
	return buff;
}

std::string GenerateProSet(Property& stPro, bool bRepeat = false)
{
	std::stringstream ssOut;
	if (stPro.Owner)
	{
		ssOut << "\n\t" << "template<typename T>" << std::endl;
		{
			char buff[200];
			snprintf(buff, sizeof(buff), "\tbool Set%s(T &t, %s t_%s)\n\t{\n", stPro.Name.c_str(), stPro.Type.c_str(), stPro.Name.c_str());
			ssOut << buff;
		}
		{
			char buff[200];
			snprintf(buff, sizeof(buff), "\t\tif (t_%s == %s) return false;\n", stPro.Name.c_str(), stPro.Name.c_str());
			ssOut << buff;
		}
		{
			char buff[200];
			if (bRepeat)
			{
				snprintf(buff, sizeof(buff), "\t\tt.Set(Name(\"%s\", %u), Name(\"%s\", %u), %s, KeyFirst, KeySecond);\n",
					stPro.RedisKey.c_str(), StringUtils::Hash(stPro.RedisKey.c_str()), stPro.Name.c_str(), StringUtils::Hash(stPro.Name.c_str()), stPro.Name.c_str());
			}
			else
			{
				snprintf(buff, sizeof(buff), "\t\tt.Set(Name(\"%s\", %u), Name(\"%s\", %u), %s, Key);\n",
					stPro.RedisKey.c_str(), StringUtils::Hash(stPro.RedisKey.c_str()), stPro.Name.c_str(), StringUtils::Hash(stPro.Name.c_str()), stPro.Name.c_str());
			}
			
			ssOut << buff;
		}
		ssOut << "\t\treturn true;\n\t}" << std::endl;
	}
	return ssOut.str();
}

std::string GenerateProVisit(Property& stPro)
{
	std::stringstream ssOut;
	std::string strOwner("true");
	if (!stPro.Owner)
	{
		strOwner = "false";
	}
	char buff[200];
	snprintf(buff, sizeof(buff), "\t\tt.Property(Name(\"%s\", %u), Name(\"%s\", %u), %s, %s);\n",
		stPro.RedisKey.c_str(), StringUtils::Hash(stPro.RedisKey.c_str()), stPro.Name.c_str(), StringUtils::Hash(stPro.Name.c_str()), stPro.Name.c_str(), strOwner.c_str());
	ssOut << buff;
	return ssOut.str();
}

std::string GenerateRepeatVisit(ActorRepeat& stRepeat, const std::string& strGroupName)
{
	std::stringstream ssOut;
	std::string strOwner("true");
	if (!stRepeat.bOwner)
	{
		strOwner = "false";
	}
	std::string strRedisKey = strGroupName + stRepeat.strRef;
	char buff[200];
	snprintf(buff, sizeof(buff), "\t\tt.Repeat(Name(\"%s\", %u), m%s, %s);\n",
		strRedisKey.c_str(), StringUtils::Hash(strRedisKey.c_str()), stRepeat.strName.c_str(), strOwner.c_str());
	ssOut << buff;
	return ssOut.str();
}

std::string GenerateRepeatInterface(ActorRepeat& stRepeat)
{
	std::stringstream ssOut;
	{
		char buff[200];
		snprintf(buff, sizeof(buff), "\t%s* Get%sData(uint64_t iIndex) { return FindMapPtr(m%s, iIndex); }\n"
			, stRepeat.strClassName.c_str(), stRepeat.strName.c_str(), stRepeat.strName.c_str());
		ssOut << buff;
	}
	if (stRepeat.bOwner)
	{
		ssOut << "\n\ttemplate<typename T>" << std::endl;
		{
			char buff[200];
			snprintf(buff, sizeof(buff), "\tbool Add%sData(T& t, const %s& stData, uint64_t iIndex)\n\t{\n"
				, stRepeat.strName.c_str(), stRepeat.strClassName.c_str());
			ssOut << buff;
		}
		{
			char buff[200];
			snprintf(buff, sizeof(buff), "\t\tif (FindMapPtr(m%s, iIndex) != nullptr) return false;\n", stRepeat.strName.c_str());
			ssOut << buff;
		}
		{
			char buff[200];
			snprintf(buff, sizeof(buff), "\t\tm%s[iIndex] = stData;\n", stRepeat.strName.c_str());
			ssOut << buff;
		}
		{
			ssOut << "\t\t" << "t.Add(Key, iIndex);" << std::endl;
			ssOut << "\t\t" << "stData.visit(t);" << std::endl;
			ssOut << "\t\t" << "t.Done();" << std::endl;
			ssOut << "\t\t" << "return true;" << std::endl;
		}
		ssOut << "\t}" << std::endl;

		ssOut << "\n\ttemplate<typename T>" << std::endl;
		{
			char buff[200];
			snprintf(buff, sizeof(buff), "\tbool Delete%sData(T& t, uint64_t iIndex)\n\t{\n"
				, stRepeat.strName.c_str());
			ssOut << buff;
		}
		{
			char buff[200];
			snprintf(buff, sizeof(buff), "\t\tauto it = m%s.find(iIndex);\n\t\tif (it != m%s.end())\n\t\t{\n", stRepeat.strName.c_str(), stRepeat.strName.c_str());
			ssOut << buff;
		}
		{
			ssOut << "\t\t\t" << "t.Delete(Key, iIndex);" << std::endl;
			ssOut << "\t\t\t" << "it->second.visit(t);" << std::endl;
			ssOut << "\t\t\t" << "m"<< stRepeat.strName <<".erase(it);" << std::endl;
			ssOut << "\t\t" << "t.Done();" << std::endl;
			ssOut << "\t\t\t" << "return true;" << std::endl;
		}
		ssOut << "\t\t}\n\t\treturn false;\n\t}" << std::endl;

		ssOut << "\n\ttemplate<typename T>" << std::endl;
		ssOut << "\tvoid ForEache" << stRepeat.strName << "Data(T&t)" << std::endl;
		ssOut << "\t{" << std::endl;
		ssOut << "\t\t" << "for (auto it : m" << stRepeat.strName << ")" << std::endl;
		ssOut << "\t\t\t" << "if (!t(it->first, it->second))" << std::endl;
		ssOut << "\t\t\t\t" << "return;" << std::endl;
		ssOut << "\t}" << std::endl;
	}
	return ssOut.str();
}

std::stringstream GenerateCPP::GenerateRepeatClass(std::vector<Property>& vPro, ActorRepeat & actorRepeat)
{
	std::stringstream ssOut;
	ssOut << "class " << actorRepeat.strClassName << std::endl << "{ \n" << "public:" << std::endl;
	ssOut << "\t" << actorRepeat.strClassName << "(uint64_t iKeyFirst, uint64_t ikeySecond):KeyFirst(iKeyFirst),KeySecond(iKeySecond)";
	{
		for (auto& it : vPro)
		{
			if (!it.Default.empty())
			{
				ssOut << ",";
				ssOut << it.Name << "(" << it.Default << ")";
			}
		}
	}
	ssOut << "{}\n" << std::endl;
	ssOut << "\tuint64_t GetKeyFirst()const { return KeyFirst; }" <<  std::endl;
	ssOut << "\tuint64_t GetKeySecond()const { return KeySecond; }" << std::endl;
	ssOut << std::endl;
	for (auto& it: vPro)
	{
		ssOut << GenerateProGet(it);
	}
	for (auto& it : vPro)
	{
		ssOut << GenerateProSet(it, true);
	}
	ssOut << "\n\t" << "template<typename T> \n\tvoid visit(T& t)\n\t{" << std::endl;
	for (auto& it : vPro)
	{
		ssOut << GenerateProVisit(it);
	}
	ssOut << "\t}" << std::endl;
	ssOut << "private:" << std::endl;
	for (auto& it : vPro)
	{
		char buff[200];
		snprintf(buff, sizeof(buff), "\t%s %s;\n", it.Type.c_str(), it.Name.c_str());
		ssOut << buff;
	}
	ssOut << "\tuint64_t KeyFirst;\n\tuint64_t KeySecond;" << std::endl;
	ssOut << "}\n" << std::endl;
	return ssOut;
}

bool GenerateCPP::GenerateStructClass(ActorStruct & actorStruct, std::stringstream & ssOut)
{
	DataGroup* pGroup = GetDataGroup(actorStruct.strGroup);
	std::vector<Property> vPro;
	for (auto& it : actorStruct.vProperty)
	{
		DataPackage* pPackage = pGroup->GetPacket(it.strKey);
		if (pPackage == nullptr)
		{
			LOG_FILE
			std::cout << "Error : Package:" << it.strKey << " not exited !" << std::endl;
			return false;
		}
		DataProperty* pPro = pPackage->GetProperty(it.strField);
		if (pPro == nullptr)
		{
			LOG_FILE
			std::cout << "Error: can't find field :" << it.strField << " in package : " << it.strKey << std::endl;
			return false;
		}

		Property stPro;
		stPro.Owner = it.bOwner;
		Translate(stPro, *pPro);
		stPro.RedisKey = pGroup->strName + std::string(".") + it.strKey;
		vPro.push_back(stPro);
	}
	//生成代码
	ssOut << "class " << actorStruct.strClassName << std::endl << "{ \n" << "public:" << std::endl;
	ssOut << "\t" << actorStruct.strClassName << "(uint64_t iKey):Key(iKey)";
	{
		for (auto& it : vPro)
		{
			if (!it.Default.empty())
			{
				ssOut << ",";
				ssOut << it.Name << "(" << it.Default << ")";
			}
		}
	}
	ssOut << "{}\n" << std::endl;
	ssOut << "\tuint64_t GetKey()const { return Key; }\n" << std::endl;
	std::stringstream ssMember;
	for (auto& it : vPro)
	{
		ssOut << GenerateProGet(it);
		ssOut << GenerateProSet(it);
		ssMember << "\t" << it.Type << " " << it.Name << ";" << std::endl;
	}

	for (auto& it : actorStruct.vRepeat)
	{
		ssOut << GenerateRepeatInterface(it);
		ssMember << "\t" << "std::map<uint64_t, "<< it.strClassName <<"> m" << it.strName << ";" << std::endl;
	}

	ssOut << "\n\t" << "template<typename T> \n\tvoid visit(T& t)\n\t{" << std::endl;
	for (auto& it : vPro)
	{
		ssOut << GenerateProVisit(it);
	}
	for (auto& it : actorStruct.vRepeat)
	{
		ssOut << GenerateRepeatVisit(it, actorStruct.strGroup);
	}
	ssOut << "\t}" << std::endl;

	ssOut << "private:" << std::endl;
	ssOut << ssMember.str();
	ssOut << "\tuint64_t Key;" << std::endl;
	ssOut << "}\n" << std::endl;
	return true;
}

bool GenerateCPP::GenerateActor(ActorEntity& actorEntity)
{
	std::string strClassName("Actor_");
	strClassName += actorEntity.strName;
	strClassName += "Data";

	m_ssOut << "struct " << strClassName << std::endl;
	m_ssOut << "{" << std::endl;
//	m_ssOut << "public:" << std::endl;
	for (auto& it : actorEntity.vStruct)
	{
		m_ssOut << "\t" << it.strClassName << " " << it.strName << ";" << std::endl;
	}

	for (auto& it : actorEntity.vMap)
	{
// 		//Get 操作
// 		m_ssOut << "\t" << it.strClassName << "* Get" << it.strName << "(uint64_t iIndex) { return FindMapPtr(m" << it.strName << ", iIndex); }" << std::endl;
// 		//Add 操作
// 		m_ssOut << "\ttemplate<typename T>" << std::endl;
// 		m_ssOut << "\t" << it.strClassName << "* Add" << it.strNameF << "(T& t, uint64_t iIndex)" << std::endl;
// 		m_ssOut << "\t{" << std::endl;
// 		m_ssOut << "\t\t" << it.strClassName << "& stData = m" << it.strName << "[iIndex];";
// 		m_ssOut << "\t\t" << "t.Load(iIndex);\n\t\tstData.visit(t);\n\t\tt.Done();\n\t\treturn &stData;\n\t}" << std::endl;

		m_ssOut << "\t" << "std::map<uint64_t, " << it.strClassName <<"> m" << it.strName << ";" << std::endl;

	}

	m_ssOut << "}\n" << std::endl;

	return true;
}

