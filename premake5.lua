include "Dependencies.lua"

workspace "Garbage2D"
    architecture "x86_64"
    startproject "Garbage2DEditor"

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
    include "Garbage2DEditor"
group ""

group "Tools"
    include "Tools/GarbageHeaderTool"
group ""