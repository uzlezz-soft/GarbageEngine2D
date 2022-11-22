project "GarbageEngine"
	language "C++"
	cppdialect "C++20"

	targetdir ("%{wks.location}/Bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/Intermediate/" .. outputdir .. "/%{prj.name}")

	flags { "NoPCH" }

	files
	{
		"Source/**.h",
		"Source/**.cpp",
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS",
		"GLFW_INCLUDE_NONE"
	}

	includedirs
	{
		"Source/Public"
	}

	links
	{
		
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
		}

		links
		{
			"%{Library.WinSock}",
			"%{Library.WinMM}",
			"%{Library.WinVersion}",
			"%{Library.BCrypt}",
		}

	filter "configurations:Debug"
		defines
        {
            "GARBAGE_DEBUG",
            "_DEBUG",
            "GARBAGE_BUILD_DLL",
            "GARBAGEENGINE_EXPORTS"
        }

		runtime "Debug"
		symbols "on"

        kind "SharedLib"
        staticruntime "off"

	filter "configurations:Release"
		defines 
        {
            "GARBAGE_RELEASE",
            "NDEBUG",
            "GARBAGE_BUILD_DLL",
            "GARBAGEENGINE_EXPORTS"
        }

		runtime "Release"
		optimize "Speed"

        kind "SharedLib"
        staticruntime "off"

	filter "configurations:Shipping"
		defines "GARBAGE_SHIPPING"
		runtime "Release"
		optimize "Speed"

        kind "StaticLib"
        staticruntime "on"