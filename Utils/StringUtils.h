#pragma once
#include <string>
#include <list>
class StringUtils
{
public:
	static void	Split(const std::string& str, std::list<std::string>& ret_, std::string sep);

	static uint32_t Hash(const char* pStr);
};
