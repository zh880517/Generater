#pragma once

#include <string>
#include <vector>
#include <rapidxml.hpp>
#include <sstream>
#include <functional>
#include <DataDesc.h>
bool	checkActorName(const std::string& strName);

struct ActorStructProperty
{
	bool	Parse(rapidxml::xml_node<>* pNode);
	std::string		strKey;
	std::string		strField;
	std::string		strDesc;
	bool			bOwner;
};

struct ActorRepeatProperty
{
	bool	Parse(rapidxml::xml_node<>* pNode);

	std::string		strField;
	bool			bOwner;
};

struct ActorRepeat
{
	bool					Parse(rapidxml::xml_node<>* pNode);
	bool					Check();
	ActorRepeatProperty*	GetProperty(const std::string& strName);
	std::string					strName;
	std::string					strRef;
	bool						bOwner;
	std::string					strDesc;
	std::string					strClassName;

	std::vector<ActorRepeatProperty> vProperty;
};

struct ActorStruct
{
	bool					Parse(rapidxml::xml_node<>* pNode);
	bool					Check();
	ActorStructProperty*	GetProperty(const std::string& strName);
	ActorRepeat*			GetRepeat(const std::string& strName);

	std::string					strName;
	std::string					strGroup;
	std::string					strDesc;
	std::vector<ActorStructProperty>	vProperty;
	std::vector<ActorRepeat>			vRepeat;

	std::string					strClassName;
};

struct ActorMap : public ActorStruct
{
	bool					Parse(rapidxml::xml_node<>* pNode);
	bool					Check();
};

struct ActorEntity
{
	bool						Parse(rapidxml::xml_node<>* pNode);
	bool						Check();
	ActorStruct*				GetStruct(const std::string& strName);
	ActorMap*					GetMap(const std::string& strName);

	std::string					strName;
	std::vector<ActorStruct>	vStruct;
	std::vector<ActorMap>		vMap;
};

struct ActorFile
{

	bool	Parse(const std::string& strFile);

	bool	Check();

	std::string					strFullFileName;
	std::vector<ActorEntity>	vActor;
};

