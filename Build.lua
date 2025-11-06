-- premake5.lua
workspace "SnakeGame"
   architecture "x64"
   configurations { "Debug", "Release", "Dist" }
   startproject "App"

   -- Workspace-wide build options for MSVC
   filter "system:windows"
      buildoptions { "/EHsc", "/Zc:preprocessor", "/Zc:__cplusplus", "/02", "/Ot" }

OutputDir = "%{cfg.system}-%{cfg.architecture}/%{cfg.buildcfg}"

local raylibVersion = "5.5"
local raylibDir = path.getabsolute("vendor/raylib")
local raylibBuildDir = path.join(raylibDir, "build")

if not os.isdir(raylibDir) then
	print("[Premake] Raylib not found. Cloning v" .. raylibVersion .. "...")
	os.execute("git clone --branch " .. raylibVersion .. " --depth 1 https://github.com/raysan5/raylib.git " .. raylibDir)
else
	print("[Premake] Raylib already exists.")
end

if not os.isdir(raylibBuildDir) then
	print("[Premake] Building raylib...")
	os.mkdir(raylibBuildDir)

	-- Build Debug (/MDd)
	local cmakeDebug = string.format(
		'cmake -S "%s" -B "%s" -DCMAKE_BUILD_TYPE=Debug -DBUILD_SHARED_LIBS=OFF -DCMAKE_MSVC_RUNTIME_LIBRARY=MultiThreadedDebugDLL',
		raylibDir, raylibBuildDir
	)
	os.execute(cmakeDebug)
	os.execute(string.format('cmake --build "%s" --config Debug', raylibBuildDir))

	-- Build Release (/MD)
	local cmakeRelease = string.format(
		'cmake -S "%s" -B "%s" -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=OFF -DCMAKE_MSVC_RUNTIME_LIBRARY=MultiThreadedDLL',
		raylibDir, raylibBuildDir
	)
	os.execute(cmakeRelease)
	os.execute(string.format('cmake --build "%s" --config Release', raylibBuildDir))
else
	print("[Premake] Raylib already built.")
end

local libVersion = "4.0"
local libDir = path.getabsolute("vendor/raygui")
local libFileName = "raygui.h"
local libFileUrl = "https://raw.githubusercontent.com/raysan5/raygui/" .. libVersion .. "/src/raygui.h"

if not os.isdir(libDir) then
	print("[Premake] RayGui not found. Downloading " .. libVersion .. "...")
	if package.config:sub(1,1) == '\\' then
		os.execute('mkdir "' .. libDir .. '"')
	else
		os.execute('mkdir -p "' .. libDir .. '"')
	end

	local out = libDir .. "/" .. libFileName
	os.execute('curl -L -o "' .. out .. '" "' .. libFileUrl .. '"')
else
	print("[Premake] RayGui already exists.")
end

include "App/Build-App.lua"