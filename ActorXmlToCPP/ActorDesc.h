#pragma once
#include <set>
#include <string>
#include <vector>
#include <rapidxml.hpp>
#include <sstream>
#include <functional>
#include <DataDesc.h>
bool	checkActorName(const std::string& strName);

using KeyString = std::tuple<std::string, std::string>;

KeyString Split(const std::string& strString);

struct ActorProperty
{
	bool			Parse(rapidxml::xml_node<>* pNode);
	std::string		strKey;
	std::string		strField;
	std::string		strDesc;
	bool			bOwner;
};

struct ActorRepeat
{
	bool					Parse(rapidxml::xml_node<>* pNode);
	std::string				strkey;
	bool					bOwner;
	std::string				strDesc;
	std::string				strName;
};

struct ActorStruct
{
	bool					Parse(rapidxml::xml_node<>* pNode);
	bool					Check();
	ActorProperty*			GetProperty(const std::string& strName);
	ActorRepeat*			GetRepeat(const std::string& strName);

	bool					GenCode(std::stringstream& strCode, std::set<std::string>& vDataFile, const std::string& strActor);

	std::string					strName;
	std::string					strDesc;
	std::vector<ActorProperty>	vProperty;
	std::vector<ActorRepeat>	vRepeat;

};

struct ActorEntity
{
	bool						Parse(rapidxml::xml_node<>* pNode);
	bool						Check();
	ActorStruct*				GetStruct(const std::string& strName);

	bool						GenCode(std::stringstream& ssCode, std::set<std::string>& vDataFile);

	std::string					strName;
	std::vector<ActorStruct>	vStruct;
};

struct ActorFile
{

	bool	Parse(const std::string& strFile);

	bool	Check();

	bool	Generate(std::string& strOut);

	std::string					strFullFileName;
	std::vector<ActorEntity>	vActor;
};

