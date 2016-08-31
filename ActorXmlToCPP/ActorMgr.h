#pragma once

#include <map>
#include <ActorDesc.h>

class ActorMgr
{
public:
	ActorMgr() = default;
	~ActorMgr() = default;

	bool	Load(const std::string& strXmlPath, const std::string& strPath);

	static ActorMgr& Instance();
private:
	std::map < std::string, std::string > m_mOut;
};
