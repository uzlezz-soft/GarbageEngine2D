include "Dependencies.lua"

workspace "Garbage"
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
    include "GarbageEngine"
    include "GarbageEditor"
group ""

group "Tools"
    include "Tools/GarbageHeaderTool"
group ""