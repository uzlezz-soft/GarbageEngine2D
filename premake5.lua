include "Dependencies.lua"

workspace "Garbage2D"
    architecture "x86_64"
    startproject "GarbageEditor"

    flags { "NoPCH" }
	
    configurations
    {
        "Debug",
        "Release",
        "Shipping"
    }

    files
    {
        ".editorconfig"
    }

    flags
	{
		"MultiProcessorCompile"
	}

    outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

group "Dependencies"
	include "ThirdParty/Premake"
group ""

group "Core"
    include "GarbageEngine2D"
    include "GarbageEditor"
group ""

group "ThirdParty"
    include "GarbageEngine2D/ThirdParty/glad"
    include "GarbageEngine2D/ThirdParty/spdlog"
    include "GarbageEngine2D/ThirdParty/zlib"
group ""

group "Tools"
    include "Tools/GarbageHeaderTool"
group ""