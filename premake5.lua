include "./vendor/premake/premake_customization/solution_items.lua"
include "Dependencies.lua"

workspace "StarEngine"
	architecture "x86_64"
	startproject "StarEditor"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

	solution_items
	{
		".editorconfig"
	}

	flags
	{
		"MultiProcessorCompile"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

group "Dependencies"
	include "vendor/premake"
	include "StarEngine/vendor/Box2D"
	include "StarEngine/vendor/GLFW"
	include "StarEngine/vendor/GLAD"
	include "StarEngine/vendor/imgui"
	include "StarEngine/vendor/yaml-cpp"
group ""

group "Dependencies - NVRHI"
		include "StarEngine/vendor/nvrhi"
group ""

group "Dependencies - Text"
		include "StarEngine/vendor/msdf-atlas-gen"
group ""

group "Core"
	include "StarEngine"
	include "StarEngine-ScriptCore"
group ""

group "Tools"
	include "StarEditor"
group ""

group "Misc"
	include "Sandbox"
group ""
