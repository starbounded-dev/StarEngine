-- StarEngine Dependencies

VULKAN_SDK = os.getenv("VULKAN_SDK")

IncludeDir = {}
IncludeDir["stb_image"] = "%{wks.location}/StarEngine/vendor/stb_image"
IncludeDir["yaml_cpp"] = "%{wks.location}/StarEngine/vendor/yaml-cpp/include"
IncludeDir["Box2D"] = "%{wks.location}/StarEngine/vendor/Box2D/include"
IncludeDir["GLFW"] = "%{wks.location}/StarEngine/vendor/GLFW/include"
IncludeDir["GLAD"] = "%{wks.location}/StarEngine/vendor/GLAD/include"
IncludeDir["ImGui"] = "%{wks.location}/StarEngine/vendor/imgui"
IncludeDir["ImGuizmo"] = "%{wks.location}/StarEngine/vendor/imguizmo"
IncludeDir["glm"] = "%{wks.location}/StarEngine/vendor/glm"
IncludeDir["filewatch"] = "%{wks.location}/StarEngine/vendor/filewatch"
IncludeDir["entt"] = "%{wks.location}/StarEngine/vendor/entt/include"
IncludeDir["mono"] = "%{wks.location}/StarEngine/vendor/mono/include"
IncludeDir["shaderc"] = "%{wks.location}/StarEngine/vendor/shaderc/include"
IncludeDir["SPIRV_Cross"] = "%{wks.location}/StarEngine/vendor/SPIRV-Cross"
IncludeDir["VulkanSDK"] = "%{VULKAN_SDK}/Include"
IncludeDir["msdfgen"] = "%{wks.location}/StarEngine/vendor/msdf-atlas-gen/msdfgen"
IncludeDir["msdf_atlas_gen"] = "%{wks.location}/StarEngine/vendor/msdf-atlas-gen/msdf-atlas-gen"

LibraryDir = {}

LibraryDir["VulkanSDK"] = "%{VULKAN_SDK}/Lib"
LibraryDir["Mono"] = "%{wks.location}/StarEngine/vendor/mono/lib/%{cfg.buildcfg}"

Library = {}
Library["mono"] = "%{LibraryDir.Mono}/libmono-static-sgen.lib"
Library["Vulkan"] = "%{LibraryDir.VulkanSDK}/vulkan-1.lib"
Library["VulkanUtils"] = "%{LibraryDir.VulkanSDK}/VkLayer_utils.lib"

Library["ShaderC_Debug"] = "%{LibraryDir.VulkanSDK}/shaderc_sharedd.lib"
Library["SPIRV_Cross_Debug"] = "%{LibraryDir.VulkanSDK}/spirv-cross-cored.lib"
Library["SPIRV_Cross_GLSL_Debug"] = "%{LibraryDir.VulkanSDK}/spirv-cross-glsld.lib"
Library["SPIRV_Tools_Debug"] = "%{LibraryDir.VulkanSDK}/SPIRV-Toolsd.lib"

Library["ShaderC_Release"] = "%{LibraryDir.VulkanSDK}/shaderc_shared.lib"
Library["SPIRV_Cross_Release"] = "%{LibraryDir.VulkanSDK}/spirv-cross-core.lib"
Library["SPIRV_Cross_GLSL_Release"] = "%{LibraryDir.VulkanSDK}/spirv-cross-glsl.lib"


-- Windows
Library["WinSock"] = "Ws2_32.lib"
Library["WinMM"] = "Winmm.lib"
Library["WinVersion"] = "Version.lib"
Library["BCrypt"] = "Bcrypt.lib"
