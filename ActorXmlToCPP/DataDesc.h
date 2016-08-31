#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <rapidxml.hpp>
#include <list>
#include <Index.h>

bool	checkDataName(const std::string& strName);
bool	checkType(const std::string& strName);

enum DataNodeType
{
	eNull,
	eDataProperty,
	eDataKey,
	eDataRepeated,
	eDataPackage,
	eDataGroup,
	eDataFile,
};

#define  CONSTRUCT(Name) Name( ):DataNode(e##Name){}
#ifndef LOG_FILE
#define  LOG_FILE std::cout << (__FILE__) << " in line:" << (__LINE__) << std::endl;
#endif

#ifndef LOG_ERROR
#define LOG_ERROR(msg) std::cout << "Error: " << msg << std::endl;
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
	int32_t			iIndex = -1;
	std::string		strDesc;
};

struct DataRepeated : DataNode
{
	CONSTRUCT(DataRepeated)
	bool				Parse(rapidxml::xml_node<>* pNode);
	bool				Check();
	DataProperty*		GetProperty(const std::string& strName);

	virtual DataNode*	GetNode(std::list<std::string> listName) override;

	void				GenIndex(std::list < NameIndex >& listIndex, NameIndex stIndex);

	std::string			GenCode(const std::string& strGroup);

	std::string					strName;
	std::string					strDesc;
	std::vector<DataProperty>	vProperty;
	int32_t						iIndex = -1;
};

struct DataPackage : DataNode
{
	CONSTRUCT(DataPackage)
	bool			Parse(rapidxml::xml_node<>* pNode);
	bool			Check();
	DataProperty*	GetProperty(const std::string& strName);

	virtual DataNode* GetNode(std::list<std::string> listName) override;

	void				GenIndex(std::list < NameIndex >& listIndex, NameIndex stIndex);

	std::string					strName;
	std::string					strDesc;
	std::vector<DataProperty>	vProperty;
	int32_t						iIndex = -1;
};

struct DataGroup : DataNode
{
	CONSTRUCT(DataGroup)
	bool				Parse(rapidxml::xml_node<>* pNode);
	bool				Check();
	DataPackage*		GetPacket(const std::string& strName);
	DataRepeated*		GetRepeated(const std::string& strName);
	virtual DataNode*  GetNode(std::list<std::string> listName) override;

	void				GenIndex(std::list < NameIndex >& listIndex);

	std::string					strName;
	std::vector<DataPackage>	vPackage;
	std::vector<DataRepeated>	vRepeated;
	uint32_t					iIndex = -1;
	std::string					strFileName;
};

struct DataFile : DataNode
{
	CONSTRUCT(DataFile)

	bool	Parse(const std::string& strFile);
	bool	Check();

	DataGroup*		GetGroup(const std::string& strName);

	virtual DataNode* GetNode(std::list<std::string> listName) override;

	bool	Generate(std::string& strCode, std::string& strRegistKeyField);

	static std::string RealType(const std::string& strType);
	

	std::vector<DataGroup>	vGroup;
	std::string				strFileName;
};

