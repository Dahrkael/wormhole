-- header only libraries don't need to be built

-- 3rdparty imGUI
project "imgui"
	location "../_temp"
	kind "StaticLib"
	
	project_files = {
		["imgui"] = { 
			"imgui/*.h",
			"imgui/*.cpp",
		},
		["implot"] = { 
			"implot/**.h",
			"implot/**.cpp",
		}
	}
	vpaths(project_files)
	files(_flatten(project_files))
	
	includedirs { 
		"imgui",
	}
	
	filter {}
	
-- 3rdparty imGUI backend
project "imgui_backend"
	location "../_temp"
	kind "StaticLib"
	
	project_files = {
		["imgui"] = { 
			"imgui/backends/*.h",
			"imgui/backends/*.cpp",
		},
	}
	vpaths(project_files)
	files(_flatten(project_files))
	
	includedirs { 
		"imgui",
		"SDL2/include",
	}
	
	filter {}

-- 3rdparty SDL2
project "sdl2"
	location "../_temp"
	kind "StaticLib"
	
	floatingpoint "Fast"
	disablewarnings { "4723" }
	
	project_files = {
		["include"] = { 
			"SDL2/include/**.h",
		},
		["src"] = { 
			"SDL2/src/*.h",
			"SDL2/src/*.c",
			"SDL2/src/atomic/*.h",
			"SDL2/src/atomic/*.c",
			"SDL2/src/audio/*.h",
			"SDL2/src/audio/*.c",
			"SDL2/src/audio/disk/*.h",
			"SDL2/src/audio/disk/*.c",
			"SDL2/src/audio/dummy/*.h",
			"SDL2/src/audio/dummy/*.c",
			"SDL2/src/cpuinfo/*.h",
			"SDL2/src/cpuinfo/*.c",
			"SDL2/src/dynapi/*.h",
			"SDL2/src/dynapi/*.c",
			"SDL2/src/events/*.h",
			"SDL2/src/events/*.c",
			"SDL2/src/file/*.h",
			"SDL2/src/file/*.c",
			"SDL2/src/haptic/*.h",
			"SDL2/src/haptic/*.c",
			"SDL2/src/haptic/dummy/*.h",
			"SDL2/src/haptic/dummy/*.c",
			"SDL2/src/hidapi/*.h",
			"SDL2/src/hidapi/*.c",
			"SDL2/src/joystick/*.h",
			"SDL2/src/joystick/*.c",
			"SDL2/src/joystick/dummy/*.h",
			"SDL2/src/joystick/dummy/*.c",
			"SDL2/src/joystick/hidapi/*.h",
			"SDL2/src/joystick/hidapi/*.c",
			"SDL2/src/joystick/virtual/*.h",
			"SDL2/src/joystick/virtual/*.c",
			"SDL2/src/libm/*.h",
			"SDL2/src/libm/*.c",
			"SDL2/src/locale/*.h",
			"SDL2/src/locale/*.c",
			"SDL2/src/misc/*.h",
			"SDL2/src/misc/*.c",
			"SDL2/src/power/*.h",
			"SDL2/src/power/*.c",
			"SDL2/src/render/*.h",
			"SDL2/src/render/*.c",
			"SDL2/src/render/software/*.h",
			"SDL2/src/render/software/*.c",
			"SDL2/src/sensor/*.h",
			"SDL2/src/sensor/*.c",
			"SDL2/src/sensor/dummy/*.h",
			"SDL2/src/sensor/dummy/*.c",
			"SDL2/src/stdlib/*.h",
			"SDL2/src/stdlib/*.c",
			"SDL2/src/thread/*.h",
			"SDL2/src/thread/*.c",
			"SDL2/src/thread/generic/SDL_syscond.h",
			"SDL2/src/thread/generic/SDL_syscond.c",
			"SDL2/src/timer/*.h",
			"SDL2/src/timer/*.c",
			"SDL2/src/video/*.h",
			"SDL2/src/video/*.c",
			"SDL2/src/video/dummy/*.h",
			"SDL2/src/video/dummy/*.c",
		}
	}
	vpaths(project_files)
	files(_flatten(project_files))
	
	-- global settings
	defines { "HAVE_LIBC=1" }
	
	includedirs {
		"SDL2/include",
	}
	
	-- windows settings
	filter "system:Windows"
		windows_files = {
			["windows"] = { 
				"SDL2/src/main/windows/SDL_windows_main.c",
				
				"SDL2/src/audio/directsound/*.h",
				"SDL2/src/audio/directsound/*.c",
				"SDL2/src/audio/winmm/*.h",
				"SDL2/src/audio/winmm/*.c",
				"SDL2/src/audio/wasapi/*.h",
				"SDL2/src/audio/wasapi/*.c",
				"SDL2/src/core/windows/*.h",
				"SDL2/src/core/windows/*.c",
				"SDL2/src/filesystem/windows/*.h",
				"SDL2/src/filesystem/windows/*.c",
				"SDL2/src/haptic/windows/*.h",
				"SDL2/src/haptic/windows/*.c",
				"SDL2/src/joystick/windows/*.h",
				"SDL2/src/joystick/windows/*.c",
				"SDL2/src/loadso/windows/*.h",
				"SDL2/src/loadso/windows/*.c",
				"SDL2/src/locale/windows/*.h",
				"SDL2/src/locale/windows/*.c",
				"SDL2/src/misc/windows/*.h",
				"SDL2/src/misc/windows/*.c",
				"SDL2/src/power/windows/*.h",
				"SDL2/src/power/windows/*.c",
				"SDL2/src/render/direct3d/*.h",
				"SDL2/src/render/direct3d/*.c",
				"SDL2/src/render/direct3d11/*.h",
				"SDL2/src/render/direct3d11/*.c",
				"SDL2/src/render/opengl/*.h",
				"SDL2/src/render/opengl/*.c",
				"SDL2/src/render/opengles2/*.h",
				"SDL2/src/render/opengles2/*.c",
				"SDL2/src/sensor/windows/*.h",
				"SDL2/src/sensor/windows/*.c",
				"SDL2/src/thread/windows/*.h",
				"SDL2/src/thread/windows/*.c",
				"SDL2/src/timer/windows/*.h",
				"SDL2/src/timer/windows/*.c",
				"SDL2/src/video/khronos/vulkan/*.h",
				"SDL2/src/video/windows/*.h",
				"SDL2/src/video/windows/*.c",
				"SDL2/src/video/yuv2rgb/*.h",
				"SDL2/src/video/yuv2rgb/*.c",
			}
		}
		vpaths(windows_files)
		files(_flatten(windows_files))
		
		removedefines { "WIN32_LEAN_AND_MEAN" }
		
	-- linux settings
	filter "system:Linux"
		links {
			-- sdl2 dependencies
		}
	
	filter {}
