#pragma once

#include <string>
#include <vector>
#include <rapidxml.hpp>
#include <list>

bool	checkDataName(const std::string& strName);
bool	checkType(const std::string& strName);

enum DataNodeType
{
	eNull,
	eDataProperty,
	eDataArray,
	eDataKey,
	eDataRepeat,
	eDataPackage,
	eDataGroup,
	eDataFile,
};

#define  CONSTRUCT(Name) Name( ):DataNode(e##Name){}
#ifndef LOG_FILE
#define  LOG_FILE std::cout << (__FILE__) << " in line:" << (__LINE__) << std::endl;
#endif

struct DataNode
{
	DataNode(DataNodeType type):eType(type){}
	DataNodeType eType;

	virtual DataNode* GetNode(std::list<std::string> listName){ return nullptr; }
};

struct DataProperty : DataNode
{
	CONSTRUCT(DataProperty)
	bool	Parse(rapidxml::xml_node<>* pNode);
	std::string		strName;
	std::string		strType;
	std::string		strDefault;
	std::string		strDesc;
};

struct DataArray : DataNode
{
	CONSTRUCT(DataArray)
	bool	Parse(rapidxml::xml_node<>* pNode);
	std::string		strName;
	std::string		strType;
	std::string		strLen;
	std::string		strDesc;
};


struct DataKey : DataNode
{
	CONSTRUCT(DataKey)
	bool	Parse(rapidxml::xml_node<>* pNode);
	std::string		strName;
	std::string		strType;
	std::string		strDesc;
};

struct DataRepeat : DataNode
{
	CONSTRUCT(DataRepeat)
	bool			Parse(rapidxml::xml_node<>* pNode);
	bool			Check();
	DataProperty*	GetProperty(const std::string& strName);

	virtual DataNode* GetNode(std::list<std::string> listName) override;

	std::string					strName;
	std::string					strDesc;
	DataKey						stKey;
	std::vector<DataProperty>	vProperty;

};

struct DataPackage : DataNode
{
	CONSTRUCT(DataPackage)
	bool			Parse(rapidxml::xml_node<>* pNode);
	bool			Check();
	DataProperty*	GetProperty(const std::string& strName);
	DataRepeat*		GetRepeat(const std::string& strName);

	virtual DataNode* GetNode(std::list<std::string> listName) override;

	std::string					strName;
	std::string					strDesc;
	std::vector<DataProperty>	vProperty;
	std::vector<DataRepeat>		vRepeat;
	std::vector<DataArray>		vArray;
};

struct DataGroup : DataNode
{
	CONSTRUCT(DataGroup)
	bool				Parse(rapidxml::xml_node<>* pNode);
	bool				Check();
	DataPackage*		GetPacket(const std::string& strName);

	virtual DataNode* GetNode(std::list<std::string> listName) override;

	std::string					strName;
	DataKey						stKey;
	std::vector<DataPackage>	vPackage;
};

struct DataFile : DataNode
{
	CONSTRUCT(DataFile)

	bool	Parse(const std::string& strFile);
	bool	Check();

	DataGroup*		GetGroup(const std::string& strName);

	virtual DataNode* GetNode(std::list<std::string> listName) override;

	std::vector<DataGroup>	vGroup;
};
