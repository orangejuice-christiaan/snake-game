project "App"
   kind "ConsoleApp"
   language "C++"
   cppdialect "C++20"
   targetdir "Binaries/%{cfg.buildcfg}"
   staticruntime "off"

   files { "Source/**.h", "Source/**.cpp", "resources/**" }

   includedirs
   {
      "Source",
	  "../Vendor/raylib/src",
      "../vendor/raygui/"
   }
   
   links 
   {
      "raylib"
   }

   targetdir ("../Binaries/" .. OutputDir .. "/%{prj.name}")
   objdir ("../Binaries/Intermediates/" .. OutputDir .. "/%{prj.name}")

    filter "system:windows"
        systemversion "latest"
        defines { "WINDOWS" }
        links { "opengl32", "winmm", "gdi32", "kernel32" }

        postbuildcommands {
            '{COPY} "%{wks.location}App/Source/Resources" "%{cfg.targetdir}/resources"'
        }

    filter "system:linux"
        links { "GL", "pthread", "m", "dl", "rt", "X11" }

        postbuildcommands {
            'cp -r "%{wks.location}App/Source/Resources" "%{cfg.targetdir}/resources"'
        }

    filter "system:macosx"
        links { "Cocoa.framework", "OpenGL.framework", "IOKit.framework", "CoreVideo.framework" }

        postbuildcommands {
            'cp -r "%{wks.location}App/Source/Resources" "%{cfg.targetdir}/resources"'
        }

    filter "configurations:Debug"
        defines { "DEBUG" }
        runtime "Debug"
        symbols "On"
        libdirs { "../Vendor/raylib/build/raylib/Debug" }

    filter "configurations:Release"
        defines { "RELEASE" }
        runtime "Release"
        optimize "On"
        symbols "On"
        libdirs { "../Vendor/raylib/build/raylib/Release" }

    filter "configurations:Dist"
        defines { "DIST" }
        runtime "Release"
        optimize "On"
        symbols "Off"
        libdirs { "../Vendor/raylib/build/raylib/Release" }