workspace "StarStudio"
	architecture "x64"
	startproject "Sandbox"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

	flags
	{
		"MultiProcessorCompile"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["GLFW"] = "StarStudio/vendor/GLFW/include"
IncludeDir["GLAD"] = "StarStudio/vendor/GLAD/include"
IncludeDir["imgui"] = "StarStudio/vendor/imgui"
IncludeDir["glm"] = "StarStudio/vendor/glm"
IncludeDir["stb_image"] = "StarStudio/vendor/stb_image"


group "Dependencies"
	include "StarStudio/vendor/GLFW"
	include "StarStudio/vendor/GLAD"
	include "StarStudio/vendor/imgui"
group ""

project "StarStudio"
	location "StarStudio"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "sspch.h"
	pchsource "StarStudio/src/sspch.cpp"

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/vendor/stb_image/**.h",
		"%{prj.name}/vendor/stb_image/**.cpp",
		"%{prj.name}/vendor/glm/glm/**.hpp",
		"%{prj.name}/vendor/glm/glm/**.inl"
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS"
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{prj.name}/vendor/spdlog/include",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.GLAD}",
		"%{IncludeDir.imgui}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.stb_image}"


	}

	links
	{
		"GLFW",
		"GLAD",
		"imgui",
		"opengl32.lib",
		"dwmapi.lib"
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"SS_BUILD_DLL",
			"GLFW_INCLUDE_NONE"
		}

	filter "configurations:Debug"
		defines "SS_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "SS_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "SS_DIST"
		runtime "Release"
		optimize "on"

	filter "action:vs2022"
      buildoptions { "/utf-8" }

project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"StarStudio/vendor/spdlog/include",
		"StarStudio/src",
		"StarStudio/vendor",
		"%{IncludeDir.glm}"
	}

	links
	{
		"StarStudio"
	}

	filter "configurations:Debug"
		defines "SS_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "SS_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "SS_DIST"
		runtime "Release"
		optimize "on"

	filter "action:vs2022"
      buildoptions { "/utf-8" }