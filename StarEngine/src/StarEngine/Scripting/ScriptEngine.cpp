#include "sepch.h"
#include "ScriptEngine.h"
#include "ScriptGlue.h"

#include "StarEngine/Core/Application.h"
#include "StarEngine/Utils/Hash.h"
#include "StarEngine/Project/Project.h"
#include "StarEngine/Asset/AssetManager.h"

#include <Coral/HostInstance.hpp>
#include <Coral/StringHelper.hpp>
#include <Coral/Attribute.hpp>
#include <Coral/TypeCache.hpp>
#include <Coral/String.hpp>

#include <filesystem>

namespace StarEngine {

	static std::unordered_map<std::string, DataType> s_DataTypeLookup = {
		{ "System.SByte", DataType::SByte },
		{ "System.Byte", DataType::Byte },
		{ "System.Int16", DataType::Short },
		{ "System.UInt16", DataType::UShort },
		{ "System.Int32", DataType::Int },
		{ "System.UInt32", DataType::UInt },
		{ "System.Int64", DataType::Long },
		{ "System.UInt64", DataType::ULong },
		{ "System.Single", DataType::Float },
		{ "System.Double", DataType::Double },
		//{ "System.Boolean", DataType::Bool },
		{ "System.String", DataType::String },
		{ "Coral.Managed.Interop.Bool32", DataType::Bool32 },
		{ "StarEngine.AssetHandle", DataType::AssetHandle },
		{ "StarEngine.Vector2", DataType::Vector2 },
		{ "StarEngine.Vector3", DataType::Vector3 },
		{ "StarEngine.Vector4", DataType::Vector4 },
		{ "StarEngine.Entity", DataType::Entity },
		//{ "StarEngine.Prefab", DataType::Prefab },
		//{ "StarEngine.Mesh", DataType::Mesh },
		//{ "StarEngine.StaticMesh", DataType::StaticMesh },
		//{ "StarEngine.Material", DataType::Material },
		//{ "StarEngine.Texture2D", DataType::Texture2D },
		//{ "StarEngine.Scene", DataType::Scene },
	};

	/**
	 * @brief Logs a C# exception message as an error.
	 *
	 * @param message The exception message received from the C# runtime.
	 */
	void OnCSharpException(std::string_view message)
	{
		std::string msg = message.data();
		SE_CORE_ERROR("C# Exception: {}", msg);
	}

	/**
	 * @brief Loads the project's C# assembly and caches its types and metadata.
	 *
	 * Loads the project assembly from the script module file path using the current Coral assembly load context. If loading succeeds, builds and caches type and field metadata for use by the scripting system.
	 */
	void ScriptEngine::LoadProjectAssembly()
	{
		// Ensure m_AppAssemblyData is accessed through the current instance
		this->m_AppAssemblyData = std::make_unique<AssemblyData>();

		auto filepath = std::filesystem::absolute(Project::GetScriptModuleFilePath());

		m_AppAssemblyData->Assembly = &m_LoadContext->LoadAssembly(filepath.string());

		if (m_AppAssemblyData->Assembly->GetLoadStatus() != Coral::AssemblyLoadStatus::Success)
		{
			return;
		}

		BuildAssemblyCache(m_AppAssemblyData.get());
	}


	/**
	 * @brief Checks whether a script with the given UUID is valid and recognized by the engine.
	 *
	 * A script is considered valid if it exists in both the cached types of the project assembly and the script metadata map.
	 *
	 * @param scriptID The UUID of the script to check.
	 * @return true if the script is valid and present in both caches; false otherwise.
	 */
	bool ScriptEngine::IsValidScript(UUID scriptID) const
	{
		if (!m_AppAssemblyData)
			return false;

		return m_AppAssemblyData->CachedTypes.find(scriptID) != m_AppAssemblyData->CachedTypes.end() &&
			m_ScriptMetadata.find(scriptID) != m_ScriptMetadata.end();

	}

	/**
	 * @brief Handles and logs messages from the Coral runtime with appropriate severity.
	 *
	 * Routes messages from Coral to the engine's logging system, mapping Coral's message level to the corresponding log severity.
	 */
	void OnCoralMessage(std::string_view message, Coral::MessageLevel level)
	{
		std::string msg = message.data();
		switch (level)
		{
		case Coral::MessageLevel::Info:
			SE_CORE_INFO("Coral: {}", msg);
			break;
		case Coral::MessageLevel::Warning:
			SE_CORE_WARN("Coral: {}", msg);
			break;
		case Coral::MessageLevel::Error:
			SE_CORE_ERROR("Coral: {}", msg);
			break;
		}
	}

	/**
	 * @brief Retrieves the metadata for a script type by its unique identifier.
	 *
	 * @param scriptID The unique identifier of the script type.
	 * @return const ScriptMetadata& Reference to the metadata associated with the script.
	 *
	 * @note Asserts if the script metadata does not exist for the given ID.
	 */
	const ScriptMetadata& ScriptEngine::GetScriptMetadata(UUID scriptID) const
	{
		SE_CORE_VERIFY(m_ScriptMetadata.find(scriptID) != m_ScriptMetadata.end());
		return m_ScriptMetadata.at(scriptID);
	}

	/**
	 * @brief Retrieves a Coral type by its full name from cached assemblies.
	 *
	 * Searches both the core and application assembly type caches for a type matching the specified full name.
	 *
	 * @param name The full name of the type to search for.
	 * @return Pointer to the matching Coral::Type if found, otherwise nullptr.
	 */
	const Coral::Type* ScriptEngine::GetTypeByName(std::string_view name) const
	{
		for (const auto& [id, type] : m_CoreAssemblyData->CachedTypes)
		{
			Coral::String typeName = type->GetFullName();

			if (typeName == name)
			{
				Coral::String::Free(typeName);
				return type;
			}

			Coral::String::Free(typeName);
		}

		for (const auto& [id, type] : m_AppAssemblyData->CachedTypes)
		{
			Coral::String typeName = type->GetFullName();

			if (typeName == name)
			{
				Coral::String::Free(typeName);
				return type;
			}

			Coral::String::Free(typeName);
		}

		return nullptr;
	}

	/**
 * @brief Returns a const reference to the singleton ScriptEngine instance.
 *
 * Use this method to access the global ScriptEngine in a read-only context.
 *
 * @return const ScriptEngine& Reference to the singleton ScriptEngine.
 */
const ScriptEngine& ScriptEngine::GetInstance() { return GetMutable(); }

	/**
	 * @brief Initializes the Coral host instance for C# scripting integration.
	 *
	 * Sets up the Coral runtime environment with appropriate settings and callbacks for message and exception handling.
	 */
	void ScriptEngine::InitializeHost()
	{
		m_Host = std::make_unique<Coral::HostInstance>();

		Coral::HostSettings settings;
		settings.CoralDirectory = (std::filesystem::current_path() / "DotNet").string();
		settings.MessageCallback = OnCoralMessage;
		settings.ExceptionCallback = OnCSharpException;

		SE_CORE_VERIFY(m_Host->Initialize(settings) == Coral::CoralInitStatus::Success, "Failed to initialize Coral");
	}

	/**
	 * @brief Shuts down the Coral host and clears the Coral type cache.
	 *
	 * Releases all resources associated with the Coral runtime host and resets the host instance.
	 */
	void ScriptEngine::ShutdownHost()
	{
		Coral::TypeCache::Get().Clear();

		m_Host->Shutdown();
		m_Host.reset();
	}

	/**
	 * @brief Initializes the scripting engine for the specified project.
	 *
	 * Sets up the Coral assembly load context, loads the core script assembly, builds its type and metadata cache, registers engine glue bindings, and loads the project's script assembly.
	 *
	 * @param project Reference to the project for which the scripting environment is initialized.
	 */
	void ScriptEngine::Initialize(Ref<Project> project)
	{
		m_LoadContext = std::make_unique<Coral::AssemblyLoadContext>(std::move(m_Host->CreateAssemblyLoadContext("StarEngineLoadContext")));

		auto scriptCorePath = (std::filesystem::current_path() / "Resources" / "Scripts" / "net8.0" / "StarEngine-ScriptCore.dll").string();
		m_CoreAssemblyData = std::make_unique<AssemblyData>();

		m_CoreAssemblyData->Assembly = &m_LoadContext->LoadAssembly(scriptCorePath);
		BuildAssemblyCache(m_CoreAssemblyData.get());

		ScriptGlue::RegisterGlue(*m_CoreAssemblyData->Assembly);

		LoadProjectAssembly();
	}

	/**
	 * @brief Shuts down the script engine and releases all loaded script resources.
	 *
	 * Releases default values for all script fields, clears script metadata, resets assembly data, unloads the assembly load context, and clears the Coral type cache.
	 */
	void ScriptEngine::Shutdown()
	{
		for (auto& [scriptID, scriptMetadata] : m_ScriptMetadata)
		{
			for (auto& [fieldID, fieldMetadata] : scriptMetadata.Fields)
			{
				fieldMetadata.DefaultValue.Release();
			}
		}
		m_ScriptMetadata.clear();

		m_AppAssemblyData.reset();
		m_CoreAssemblyData.reset();
		m_Host->UnloadAssemblyLoadContext(*m_LoadContext);

		Coral::TypeCache::Get().Clear();
	}

	/**
	 * @brief Caches types and script metadata from a loaded assembly.
	 *
	 * Iterates through all types in the provided assembly, generating a unique script ID for each. If a script asset does not already exist for a type, attempts to import it from the project's source directory. For types that inherit from `StarEngine.Entity`, extracts public fields with supported data types, generates field metadata including default values, and stores this information for runtime access.
	 *
	 * @param assemblyData Pointer to the assembly data structure containing the loaded assembly and type cache.
	 */

	void ScriptEngine::BuildAssemblyCache(AssemblyData* assemblyData)
	{
		auto& types = assemblyData->Assembly->GetTypes();
		auto& entityType = assemblyData->Assembly->GetType("StarEngine.Entity");

		for (auto& type : types)
		{
			std::string fullName = type->GetFullName();
			auto scriptID = Hash::GenerateFNVHash64Bit(fullName);

			if (!Project::GetActive()->GetEditorAssetManager()->IsAssetHandleValid(scriptID))
			{
				std::string fileName = fullName;
				size_t pos = fileName.find('.');
				fileName.erase(0, pos + 1);
				std::string finalName = fileName + ".cs";

				auto scriptModuleFilePath = std::filesystem::absolute(Project::GetActiveAssetDirectory());
				std::string finalFilePath = scriptModuleFilePath.string() + "/Scripts/Source/" + finalName;
				//SE_CORE_WARN("FileName = {}", finalFilePath);

				if (std::filesystem::exists(finalFilePath))
					Project::GetActive()->GetEditorAssetManager()->ImportScriptAsset("Scripts/Source/" + finalName, scriptID);
			}

			assemblyData->CachedTypes[scriptID] = type;

			if (type->IsSubclassOf(entityType))
			{
				auto& metadata = m_ScriptMetadata[scriptID];
				metadata.FullName = fullName;

				auto temp = type->CreateInstance();

				for (auto& fieldInfo : type->GetFields())
				{
					Coral::ScopedString fieldName = fieldInfo.GetName();
					std::string fieldNameStr = fieldName;

					if (fieldNameStr.find("k__BackingField") != std::string::npos)
						continue;

					auto typeName = fieldInfo.GetType().GetFullName();

					if (s_DataTypeLookup.find(typeName) == s_DataTypeLookup.end())
						continue;

					if (fieldInfo.GetAccessibility() != Coral::TypeAccessibility::Public)
						continue;

					// NOTE(Peter): Entity.ID bleeds through to the inheriting scripts, annoying, but we can deal
					if (fieldName == "ID")
						continue;

					auto fullFieldName = fmt::format("{}.{}", fullName, fieldNameStr);
					uint32_t fieldID = Hash::GenerateFNVHash(fullFieldName);

					auto& fieldMetadata = metadata.Fields[fieldID];
					fieldMetadata.Name = fieldName;
					fieldMetadata.Type = s_DataTypeLookup.at(typeName);
					fieldMetadata.ManagedType = &fieldInfo.GetType();

					switch (fieldMetadata.Type)
					{
					case DataType::SByte:
						fieldMetadata.SetDefaultValue<int8_t>(temp);
						break;
					case DataType::Byte:
						fieldMetadata.SetDefaultValue<uint8_t>(temp);
						break;
					case DataType::Short:
						fieldMetadata.SetDefaultValue<int16_t>(temp);
						break;
					case DataType::UShort:
						fieldMetadata.SetDefaultValue<uint16_t>(temp);
						break;
					case DataType::Int:
						fieldMetadata.SetDefaultValue<int32_t>(temp);
						break;
					case DataType::UInt:
						fieldMetadata.SetDefaultValue<uint32_t>(temp);
						break;
					case DataType::Long:
						fieldMetadata.SetDefaultValue<int64_t>(temp);
						break;
					case DataType::ULong:
						fieldMetadata.SetDefaultValue<uint64_t>(temp);
						break;
					case DataType::Float:
						fieldMetadata.SetDefaultValue<float>(temp);
						break;
					case DataType::Double:
						fieldMetadata.SetDefaultValue<double>(temp);
						break;
					case DataType::Bool:
						fieldMetadata.SetDefaultValue<bool>(temp);
						break;
					case DataType::String:
						fieldMetadata.SetDefaultValue<std::string_view>(temp);
						break;
					case DataType::Bool32:
						fieldMetadata.SetDefaultValue<uint32_t>(temp);
						break;
					case DataType::AssetHandle:
						fieldMetadata.SetDefaultValue<uint64_t>(temp);
						break;
					case DataType::Vector2:
						fieldMetadata.SetDefaultValue<glm::vec2>(temp);
						break;
					case DataType::Vector3:
						fieldMetadata.SetDefaultValue<glm::vec3>(temp);
						break;
					case DataType::Vector4:
						fieldMetadata.SetDefaultValue<glm::vec4>(temp);
						break;
					case DataType::Entity:
						fieldMetadata.SetDefaultValue<uint64_t>(temp);
						//case DataType::Prefab:
						//case DataType::Mesh:
						//case DataType::StaticMesh:
						//case DataType::Material:
						//case DataType::Texture2D:
						//case DataType::Scene:
						//	fieldMetadata.DefaultValue.Allocate(sizeof(uint64_t));
						//	fieldMetadata.DefaultValue.ZeroInitialize();
						//	break;
					default:
						break;
					}
				}

				temp.Destroy();
			}
		}
	}

	/**
	 * @brief Returns a mutable reference to the singleton ScriptEngine instance.
	 *
	 * This method provides access to the single global instance of ScriptEngine, allowing modification of its state.
	 *
	 * @return ScriptEngine& Reference to the singleton ScriptEngine instance.
	 */
	ScriptEngine& ScriptEngine::GetMutable()
	{
		static ScriptEngine s_Instance;
		return s_Instance;
	}

}
