#include <iostream>
#include <cmdline.h>
#include <fstream>
#include <DataDesc.h>
#include <DataMgr.h>
#include <ActorMgr.h>
#include <FileUtils.h>
int main(int argc, char *argv[])
{
	cmdline::parser cmd;
	cmd.add<std::string>("xml", 'x', "Read XML File Directory");
	cmd.add<std::string>("cpp", 'c', "Generate Cpp File Directory");

	if (!cmd.parse_check(argc, argv))
	{
		system("pause");
		return 0;
	}
	std::string strXmlPath = FileUtils::TransPath(cmd.get<std::string>("xml"));
	std::string strCppPath = FileUtils::TransPath(cmd.get<std::string>("cpp"));
	

	//strXmlPath.replace()
	
	if (!DataMgr::Instance().Load(strXmlPath, strCppPath))
	{
		system("pause");
		return 0;
	}
	if (!ActorMgr::Instance().Load(strXmlPath, strCppPath))
	{
		system("pause");
		return 0;
	}
	
	return 0;
}