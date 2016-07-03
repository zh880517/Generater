#include <iostream>
#include <Manager.h>
#include <cmdline.h>

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
	

	DataMgr dataMgr;
	ActorMgr actorMgr(&dataMgr);
	if (dataMgr.Load(cmd.get<std::string>("xml")) && actorMgr.Load(cmd.get<std::string>("xml"), cmd.get<std::string>("cpp")))
	{
		actorMgr.Generate();
	}

	system("pause");
	return 0;
}