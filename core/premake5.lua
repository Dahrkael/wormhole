--

project "core"
	location "../_temp"
	kind "WindowedApp"
	targetname "wormhole"
	
	project_files = {
		["src"] = { 
			"src/main.cpp",
			"src/wormhole.cpp",
			"src/launcher.cpp",
		},
		["hdr"] = {
			"src/wormhole.h",
			"src/launcher.h",
		},
		["res"] = {
			"src/wormhole.rc",
		}
	}
	vpaths(project_files)
	files(_flatten(project_files))
	
	-- global settings
	includedirs { 
		"../3rdparty",
		"../3rdparty/imgui",
		"../3rdparty/imgui/backends",
		"../3rdparty/SDL2/include",
	}
	
	links { 
		"imgui_backend",
		"imgui",
		"sdl2", 
	}
	
	-- debugdir points to the game client folder
	debugdir "../client 1.16.5.0"
	
	-- windows settings
	filter "system:Windows"
		-- sdl2 dependencies
		links {
			"setupapi",
			"winmm",
			"imm32",
			"version"
		}
		
	-- linux settings
	filter "system:Linux"
		
	filter {}
