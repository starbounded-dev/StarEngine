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
	include "StarEngine/vendor/FMOD-Audio"
group ""

include "StarEngine"
include "Sandbox"
include "StarEditor"
