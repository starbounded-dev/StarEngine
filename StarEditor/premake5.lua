project "StarEditor"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"src/**.h",
		"src/**.cpp"
	}

	includedirs
	{
		"%{wks.location}/StarEngine/vendor/spdlog/include",
		"%{wks.location}/StarEngine/src",
		"%{wks.location}/StarEngine/vendor",
		"%{IncludeDir.glm}",
		"%{IncludeDir.entt}",
		"%{IncludeDir.ImGuizmo}"
	}

	links
	{
		"StarEngine"
	}

	filter "system:windows" 
		systemversion "latest"

	filter "configurations:Debug"
		defines "SE_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "SE_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "SE_DIST"
		runtime "Release"
		optimize "on"

		postbuildcommands {
			"{COPYDIR} %{wks.location}/StarEditor/assets %{wks.location}/bin/" .. outputdir .. "/StarEditor/assets"
		}

		postbuildcommands {
			"{COPYDIR} %{wks.location}/StarEditor/Resources %{wks.location}/bin/" .. outputdir .. "/StarEditor/Resources"
		}

		postbuildcommands {
			"{COPY} %{wks.location}/StarEditor/imgui.ini %{wks.location}/bin/" .. outputdir .. "/StarEditor/imgui.ini"
		}

	filter "action:vs2022"
    	buildoptions { "/utf-8" }
