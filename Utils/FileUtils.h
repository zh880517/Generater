#pragma once
#include <filesystem>

//VS2013
#if _MSC_VER == 1800
namespace FileSystem = std::tr2::sys;
#endif

//VS2015
#if _MSC_VER == 1900
namespace FileSystem = std::experimental::filesystem::v1;
#endif

class FileUtils
{
public:
	static void GetAllFile(const FileSystem::path& path, std::list<FileSystem::path>& listFile, const std::string strFileType = "");

	static bool CreadteDir(const std::string & strDir);

	static std::string GetFileName(const std::string& strFullPath);

	static std::string PathSub(const std::string& strFull, const std::string& strPath);
	static std::string PathSubWithOutExt(const std::string& strFull, const std::string& strPath);

	static std::string TransPath(const std::string& strPath);

};
