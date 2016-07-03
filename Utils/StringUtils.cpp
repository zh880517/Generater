#include <StringUtils.h>

#include <iostream>
using std::string;
void StringUtils::Split(const std::string & str, std::list<std::string>& ret_, std::string sep)
{
	if (str.empty())
		return;

	string tmp;
	string::size_type pos_begin = str.find_first_not_of(sep);
	string::size_type comma_pos = 0;

	while (pos_begin != string::npos)
	{
		comma_pos = str.find(sep, pos_begin);
		if (comma_pos != string::npos)
		{
			tmp = str.substr(pos_begin, comma_pos - pos_begin);
			pos_begin = comma_pos + sep.length();
		}
		else
		{
			tmp = str.substr(pos_begin);
			pos_begin = comma_pos;
		}

		if (!tmp.empty())
		{
			ret_.push_back(tmp);
			tmp.clear();
		}
	}
}

uint32_t StringUtils::Hash(const char * pStr)
{
	const char *ch(pStr);
	uint32_t hash(5381);

	while (*ch != '\0')
	{
		hash = ((hash << 5) + hash) + *ch;
		++ch;
	}

	return hash;
}
