-- 

include "utils"

workspace "supercoolproject"
	location "./_temp/"
	
	targetname "%{prj.name}_%{cfg.architecture}"
	targetdir "./_bin/%{cfg.buildcfg}_%{cfg.architecture}"
	objdir "./_build/%{cfg.buildcfg}_%{cfg.architecture}/%{prj.name}"
	debugdir "."
	
	-- CONFIGURATIONS
	configurations { "debug", "release", "profile" }
	platforms { "64bits", "32bits" }
	
	-- global settings
	cppdialect "C++14"
	staticruntime "On"
	characterset "Unicode"
	floatingpoint "Strict"
	rtti "Off"
	exceptionhandling "On"
	editandcontinue "Off"
	
	flags { "MultiProcessorCompile", "FatalWarnings", "NoImplicitLink", 
			"NoIncrementalLink", "NoManifest", "NoPCH", "RelativeLinks", "UndefinedIdentifiers" }
			
	defines { "_HAS_EXCEPTIONS=0", "UNICODE" }
	
	-- windows settings
	filter "system:Windows"
		systemversion "latest"
		defines { "WINVER=0x0601", "_WIN32_WINNT=0x0601", "_CRT_SECURE_NO_WARNINGS", "_CRT_NONSTDC_NO_WARNINGS", "NOMINMAX", "WIN32_LEAN_AND_MEAN" }
		--links { "ws2_32" }
	
	filter { "system:Windows", "kind:ConsoleApp" }
		targetextension ".exe"
		
	-- linux settings
	filter "system:Linux"
		buildoptions { "-fmax-errors=0", "-fno-math-errno", "-fPIC", "-finput-charset=UTF-8", "-Wno-undef", "-Wno-unused-command-line-argument" }
		links { "pthread", "dl", "m" }
	
	filter { "system:Linux", "kind:ConsoleApp" }
		targetextension ".elf"
		
	-- global 32 bits settings
	filter "platforms:32bits"
		architecture "x86"
		
	-- global 64 bits settings
	filter "platforms:64bits"
		architecture "x86_64"
		
	-- global debug settings
	filter "configurations:debug"
		defines { "DEBUG" }
		optimize "Off"
		symbols "On"
	
	-- global release settings	
	filter "configurations:release"
		defines { "NDEBUG" }
		optimize "Speed"
		
	filter "configurations:profile"
		defines { "NDEBUG", "AB_PROFILE", "TRACY_ENABLE" }
		optimize "Speed"
	
	filter {}
		
-- PROJECTS
include "3rdparty"
include "core"		
