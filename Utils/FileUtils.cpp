#include <FileUtils.h>

#include <iostream>

void FileUtils::GetAllFile(const FileSystem::path & path, std::list<FileSystem::path>& listFile, const std::string strFileType)
{
	if (FileSystem::is_directory(path))
	{
		FileSystem::directory_iterator itBegin(path);
		FileSystem::directory_iterator itEnd;
		while (itBegin != itEnd)
		{
			GetAllFile(*itBegin, listFile, strFileType);
			++itBegin;
		}
	}
	else
	{
		if (strFileType.empty() || path.extension() == strFileType)
		{
			listFile.push_back(path);
		}
	}
}

bool FileUtils::CreadteDir(const std::string & strDir)
{

// 	std::string strTemp(strFullPath);
// 	std::string::size_type nPoint = strTemp.find_last_of(".");
// 	if (nPoint != std::string::npos && nPoint != 0 && strTemp[nPoint-1] != '/')
// 	{
// 		std::string::size_type nPos = strTemp.find_last_of("/");
// 		if (nPos != std::string::npos)
// 		{
// 			strTemp.erase(strTemp.begin() + nPos, strTemp.end());
// 		}
// 	}
	//std::error_code  eCode;
	bool bRet = FileSystem::create_directory(FileSystem::path(strDir));
	//std::cout << eCode.message();
	return bRet;
}

std::string FileUtils::PathSub(const std::string & strFull, const std::string & strPath)
{
	std::string strOut;
	std::string::size_type nPos = strFull.find(strPath);
	if (nPos != std::string::npos)
	{
		strOut.append(strFull.begin() + nPos + strPath.size(), strFull.end());
		return strOut;
	}
	return strFull;
}
