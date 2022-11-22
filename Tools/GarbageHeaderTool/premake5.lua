project "GarbageHeaderTool"
    kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"
    staticruntime "on"

	flags { "NoPCH" }

	targetdir ("%{wks.location}/Bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/Intermediate/" .. outputdir .. "/%{prj.name}")

	defines
	{
		"_CRT_SECURE_NO_WARNINGS"
	}

	files
	{
		"Source/**.h",
		"Source/**.cpp",
	}

    includedirs
	{
		"Source/Public"
	}

    filter "configurations:Debug"
		defines "_DEBUG"

		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "NDEBUG"

		runtime "Release"
		optimize "Speed"

	filter "configurations:Shipping"
        defines "NDEBUG"

		runtime "Release"
		optimize "Speed"