#pragma once

#include "CSharpObject.h"
#include "ScriptEntityStorage.h"

#include "StarEngine/Core/Base.h"
#include "StarEngine/Core/Base.h"
#include "StarEngine/Core/Buffer.h"
#include "StarEngine/Asset/Asset.h"
#include "StarEngine/Scene/Scene.h"

#include <Coral/Assembly.hpp>
#include <Coral/Type.hpp>
#include <Coral/StableVector.hpp>
#include <Coral/Attribute.hpp>
#include <Coral/Array.hpp>

namespace Coral {

	class HostInstance;
	class ManagedAssembly;
	class AssemblyLoadContext;

}

namespace StarEngine {

	class Scene;
	class Project;

	struct AssemblyData
	{
		Coral::ManagedAssembly* Assembly;
		std::unordered_map<UUID, Coral::Type*> CachedTypes;
	};

	struct FieldMetadata
	{
		std::string Name;
		DataType Type;
		Coral::Type* ManagedType;

		Buffer DefaultValue;

	private:
		template<typename T>
		void SetDefaultValue(Coral::ManagedObject& temp)
		{
			if (ManagedType->IsSZArray())
			{
				auto value = temp.GetFieldValue<Coral::Array<T>>(Name);
				DefaultValue = Buffer::Copy(Buffer(value.Data(), value.ByteLength()));
				Coral::Array<T>::Free(value);
			}
			else
			{
				DefaultValue.Allocate(sizeof(T));
				auto value = temp.GetFieldValue<T>(Name);
				DefaultValue.Write(&value, sizeof(T));
			}
		}

		friend class ScriptEngine;
	};

	struct ScriptMetadata
	{
		std::string FullName;
		std::unordered_map<uint32_t, FieldMetadata> Fields;
	};

	class ScriptEngine
	{
	public:
		Ref<Scene> GetCurrentScene() const { return m_CurrentScene; }
		void SetCurrentScene(Ref<Scene> scene) { m_CurrentScene = scene; }

		bool IsValidScript(UUID scriptID) const;

		const ScriptMetadata& GetScriptMetadata(UUID scriptID) const;
		const std::unordered_map<UUID, ScriptMetadata>& GetAllScripts() const { return m_ScriptMetadata; }

		const Coral::Type* GetTypeByName(std::string_view name) const;

	public:
		static const ScriptEngine& GetInstance();

	private:
		void InitializeHost();
		void ShutdownHost();

		void Initialize(Ref<Project> project);
		void Shutdown();

		void LoadProjectAssembly();

		void BuildAssemblyCache(AssemblyData* assemblyData);

		template<typename... TArgs>
		CSharpObject Instantiate(UUID entityID, ScriptStorage& storage, TArgs&&... args)
		{
			SE_CORE_VERIFY(storage.EntityStorage.contains(entityID));

			auto& entityStorage = storage.EntityStorage.at(entityID);

			if (!IsValidScript(entityStorage.ScriptID))
				return {};

			auto* type = m_AppAssemblyData->CachedTypes[entityStorage.ScriptID];
			auto instance = type->CreateInstance(std::forward<TArgs>(args)...);
			auto [index, handle] = m_ManagedObjects.Insert(std::move(instance));

			entityStorage.Instance = &handle;

			for (auto& [fieldID, fieldStorage] : entityStorage.Fields)
			{
				const auto& fieldMetadata = m_ScriptMetadata[entityStorage.ScriptID].Fields[fieldID];

				auto& editorAssignableAttribType = m_CoreAssemblyData->Assembly->GetType("Nutcrackz.EditorAssignableAttribute");
				if (fieldMetadata.ManagedType->HasAttribute(editorAssignableAttribType))
				{
					Coral::ManagedObject value = fieldMetadata.ManagedType->CreateInstance(fieldStorage.GetValue<uint64_t>());
					handle.SetFieldValue(fieldStorage.GetName(), value);
					value.Destroy();
				}
				else if (fieldMetadata.ManagedType->IsSZArray())
				{
					if (fieldMetadata.ManagedType->GetElementType().HasAttribute(editorAssignableAttribType))
					{
						Coral::Array<Coral::ManagedObject> arr = Coral::Array<Coral::ManagedObject>::New(fieldStorage.GetLength());

						for (int32_t i = 0; i < fieldStorage.GetLength(); i++)
						{
							arr[i] = fieldMetadata.ManagedType->GetElementType().CreateInstance(fieldStorage.GetValue<uint64_t>(i));
						}

						handle.SetFieldValue(fieldStorage.GetName(), arr);

						for (int32_t i = 0; i < fieldStorage.GetLength(); i++)
							arr[i].Destroy();

						Coral::Array<Coral::ManagedObject>::Free(arr);
					}
					else
					{
						struct ArrayContainer
						{
							void* Data;
							int32_t Length;
						} array;

						array.Data = fieldStorage.m_ValueBuffer.Data;
						array.Length = fieldStorage.GetLength();

						handle.SetFieldValueRaw(fieldStorage.GetName(), &array);
					}
				}
				else
				{
					handle.SetFieldValueRaw(fieldStorage.GetName(), fieldStorage.m_ValueBuffer.Data);
				}

				fieldStorage.m_Instance = &handle;
			}

			CSharpObject result;
			result.m_Handle = &handle;
			return result;
		}

		void DestroyInstance(UUID entityID, ScriptStorage& storage)
		{
			SE_CORE_VERIFY(storage.EntityStorage.find(entityID) != storage.EntityStorage.end());

			auto& entityStorage = storage.EntityStorage.at(entityID);

			SE_CORE_VERIFY(IsValidScript(entityStorage.ScriptID));

			for (auto& [fieldID, fieldStorage] : entityStorage.Fields)
				fieldStorage.m_Instance = nullptr;

			entityStorage.Instance->Destroy();
			entityStorage.Instance = nullptr;

			// TODO(Peter): Free-list
		}

	private:
		static ScriptEngine& GetMutable();

	private:
		ScriptEngine() = default;

		ScriptEngine(const ScriptEngine&) = delete;
		ScriptEngine(ScriptEngine&&) = delete;

		ScriptEngine& operator=(const ScriptEngine&) = delete;
		ScriptEngine& operator=(ScriptEngine&&) = delete;

	private:
		std::unique_ptr<Coral::HostInstance> m_Host;
		std::unique_ptr<Coral::AssemblyLoadContext> m_LoadContext;
		std::unique_ptr<AssemblyData> m_CoreAssemblyData = nullptr;
		std::unique_ptr<AssemblyData> m_AppAssemblyData = nullptr;

		std::unordered_map<UUID, ScriptMetadata> m_ScriptMetadata;

		Ref<Scene> m_CurrentScene = nullptr;
		Coral::StableVector<Coral::ManagedObject> m_ManagedObjects;

	private:
		friend class Application;
		friend class Project;
		friend class Scene;
		friend class SceneHierarchyPanel;
		friend class SceneSerializer;
		friend class SceneImporter;
	};

}
