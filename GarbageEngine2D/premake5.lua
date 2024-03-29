project "GarbageEngine2D"
	language "C++"
	cppdialect "C++17"

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
		"Source/Public",
		"Source/Intermediate",
		"%{Include.glad}",
		"%{Include.spdlog}",
		"%{Include.GLFW}",
		"%{Include.stb}"
	}

	links
	{
		"GarbageHeaderTool",
		"glad",
		"spdlog",
		"GLFW",
		"stb"
	}

	disablewarnings { "4251", "4005" }

	prebuildcommands
	{
		"%{wks.location}Bin/" .. outputdir .. "/GarbageHeaderTool/GarbageHeaderTool -p%{prj.name} -aGARBAGE_API -s%{prj.location}Source/Public -s%{prj.location}Source/Private -o%{prj.location}Source/Intermediate"
	}
	
	postbuildcommands
	{
		"{COPY} %{wks.location}Bin/" .. outputdir .. "/GarbageEngine2D/*.dll %{wks.location}Bin/" .. outputdir .. "/GarbageEditor",
		"{COPY} %{wks.location}Bin/" .. outputdir .. "/GarbageEngine2D/*.so %{wks.location}Bin/" .. outputdir .. "/GarbageEditor",
		"{COPY} %{wks.location}Bin/" .. outputdir .. "/GarbageEngine2D/*.pdb %{wks.location}Bin/" .. outputdir .. "/GarbageEditor"
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
            "GARBAGE_ENGINE_DLL",
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
            "GARBAGE_ENGINE_DLL",
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