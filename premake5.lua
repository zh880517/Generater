
local BuilPath = "Build/" .. _ACTION  --工程路径
local ObjPath = "Build/ObjOut/" .. _ACTION  -- 编译中间文件输出目录
local RapidXml_Path = "./rapidxml"  --rapidxml源码目录
local OutPath = "%{cfg.buildcfg}_%{cfg.platform}"

workspace "Generate"
    configurations { "Debug", "Release" }--项目配置类型
    platforms { "X32", "X64"}
    location(BuilPath)--解决方案目录

	filter "configurations:Debug" --编译配置
		defines { "DEBUG" } --预编译宏
		flags { "Symbols" } -- 生成pdb文件

	filter "configurations:Release"
		defines { "NDEBUG" }
		optimize "On" --开启优化选项

	filter { "platforms:*32" }
    	architecture "x86"
	filter { "platforms:*64" }
    	architecture "x86_64"

    flags {"C++11", "MultiProcessorCompile","StaticRuntime"}--默认为/MT 设置 "StaticRuntime" 会变成/MD
    startproject "GenerateTest" 

    
project "ActorXmlToCPP"
    kind "ConsoleApp" 
    language "C++" 
    targetdir("bin/"..OutPath) --生成文件目录
    location(BuilPath.."/ActorXmlToCPP") --工程目录
    objdir(ObjPath.."/ActorXmlToCPP/%{cfg.buildcfg}")
    includedirs {RapidXml_Path, "ActorXmlToCPP", "Utils"} --头文件目录
    debugdir ""
    debugargs { "--xml=./DescFile", "--cpp=./DescFile" } --调试命令行参数
    files { "ActorXmlToCPP/**.h", "ActorXmlToCPP/**.cpp" , "Utils/**.h", "Utils/**.cpp"} --源码文件目录
    
    
