#pragma once
#include <DataDesc.h>
#include <ActorDesc.h>
#include <map>
#include <functional>
struct Property
{
	std::string Name;
	std::string Type;
	std::string Default;
	bool		Owner;
	std::string RedisKey;
};

class GenerateCPP
{
public:
	GenerateCPP(ActorFile* pActorFile);
	~GenerateCPP() = default;

	bool	Generate();

	const std::stringstream& GetResult() { return m_ssOut; }

private:

	bool	HandleActorEntry(ActorEntity& actorEntity);
	bool	HandleStruct(ActorStruct& actorStruct);
	bool	HandleRepeat(ActorRepeat& actorRepeat, DataGroup* pGroup, std::stringstream& ssRepeat);
	
	std::stringstream GenerateRepeatClass(std::vector<Property>& vPro, ActorRepeat & actorRepeat);
	bool	GenerateStructClass(ActorStruct& actorStruct, std::stringstream& ssOut);
	bool	GenerateActor(ActorEntity& actorEntity);
private:
	std::list<std::string>							m_listName;
	std::stringstream								m_ssOut;
	ActorFile*										m_pActorFile;
public:

	std::function<DataGroup*(const std::string&)>	GetDataGroup;
};
