#pragma once
#include <string>
#include <sstream>
union Index
{
	Index():Value(0) {}
	Index(uint32_t iValue):Value(iValue){}
	uint32_t Value;

	std::string ToString() const
	{
		std::stringstream ss;
		ss << "0x" << std::hex << Value;
		return ss.str();
	}
	struct Detail
	{
		uint32_t Property : 8;
		uint32_t Package : 8;
		uint32_t Group : 8;
	} Detail;
	struct KeyField
	{
		uint32_t Field : 8;
		uint32_t Key : 16;
	} KeyField;
};


struct NameIndex
{
	std::string	 strGroup;
	std::string	 strPackRepeat;
	std::string	 strField;
	Index		 stIndex;
};