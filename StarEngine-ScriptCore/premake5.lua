project "StarEngine-ScriptCore"
	kind "SharedLib"
	language "C#"
	dotnetframework "net9.0"
	clr "Unsafe"

	targetdir ("../StarEditor/Resources/Scripts")
	objdir ("../StarEditor/Resources/Scripts/Intermediates")

	files 
	{
		"Source/**.cs",
		"Properties/**.cs"
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
