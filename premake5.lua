include "./vendor/premake/premake_customization/solution_items.lua"

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

-- Include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["GLFW"] = "%{wks.location}/StarEngine/vendor/GLFW/include"
IncludeDir["GLAD"] = "%{wks.location}/StarEngine/vendor/GLAD/include"
IncludeDir["imgui"] = "%{wks.location}/StarEngine/vendor/imgui"
IncludeDir["glm"] = "%{wks.location}/StarEngine/vendor/glm"
IncludeDir["stb_image"] = "%{wks.location}/StarEngine/vendor/stb_image"
IncludeDir["entt"] = "%{wks.location}/StarEngine/vendor/entt/include"


group "Dependencies"
	include "vendor/premake"
	include "StarEngine/vendor/GLFW"
	include "StarEngine/vendor/GLAD"
	include "StarEngine/vendor/imgui"
group ""

include "StarEngine"
include "Sandbox"
include "StarEditor"