project "StarEngine-ScriptCore"
	kind "SharedLib"
	language "C#"
	dotnetframework "net8.0"
	clr "Unsafe"

	namespace "StarEngine"

	targetdir ("../StarEditor/Resources/Scripts")
	objdir ("../StarEditor/Resources/Scripts/Intermediates")

	files 
	{
		"Source/**.cs"
	}

	links
	{
		"Coral.Managed"
	}
	
	filter "configurations:Debug"
		optimize "Off"
		symbols "Default"

	filter "configurations:Release"
		optimize "On"
		symbols "Default"

	filter "configurations:Dist"
		optimize "Full"
		symbols "Off"
