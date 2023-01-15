project "glad"
    kind "StaticLib"
	language "C++"
	cppdialect "C++17"

	flags { "NoPCH" }

	targetdir ("%{wks.location}/Bin/" .. outputdir .. "/ThirdParty/%{prj.name}")
	objdir ("%{wks.location}/Intermediate/" .. outputdir .. "/ThirdParty/%{prj.name}")

	defines
	{
		"_CRT_SECURE_NO_WARNINGS"
	}

	files
	{
		"Source/**.h",
		"Source/**.c",
	}

    includedirs
	{
		"Source/Public"
	}

    filter "configurations:Debug"
		defines "_DEBUG"

		runtime "Debug"
		symbols "on"

		staticruntime "off"

	filter "configurations:Release"
		defines "NDEBUG"

		runtime "Release"
		optimize "Speed"

		staticruntime "off"

	filter "configurations:Shipping"
        defines "NDEBUG"

		runtime "Release"
		optimize "Speed"

		staticruntime "on"