#include "sepch.h"
#include "ScriptEntityStorage.h"
#include "ScriptEngine.h"

namespace StarEngine {

	/**
	 * @brief Initializes script storage for an entity with the specified script.
	 *
	 * Sets up storage for all fields defined in the script's metadata for the given entity. Verifies that the script is valid and the entity does not already have storage initialized.
	 *
	 * @param scriptID The unique identifier of the script to associate with the entity.
	 * @param entityID The unique identifier of the entity to initialize storage for.
	 */
	void ScriptStorage::InitializeEntityStorage(UUID scriptID, UUID entityID)
	{
		const auto& scriptEngine = ScriptEngine::GetInstance();

		SE_CORE_VERIFY(scriptEngine.IsValidScript(scriptID));
		SE_CORE_VERIFY(EntityStorage.find(entityID) == EntityStorage.end());


		const auto& scriptMetadata = scriptEngine.GetScriptMetadata(scriptID);

		auto& entityStorage = EntityStorage[entityID];		
		entityStorage.ScriptID = scriptID;

		for (const auto& [fieldID, fieldMetadata] : scriptMetadata.Fields)
		{
			InitializeFieldStorage(entityStorage, fieldID, fieldMetadata);
		}
	}

	/**
	 * @brief Shuts down and removes script storage for a specific entity.
	 *
	 * Releases all field value buffers associated with the entity and erases the entity's storage entry.
	 * Verifies that the script is valid and the entity storage exists before proceeding.
	 */
	void ScriptStorage::ShutdownEntityStorage(UUID scriptID, UUID entityID)
	{
		const auto& scriptEngine = ScriptEngine::GetInstance();

		SE_CORE_VERIFY(scriptEngine.IsValidScript(scriptID));
		SE_CORE_VERIFY(EntityStorage.find(entityID) != EntityStorage.end());

		for (auto& [fieldID, fieldStorage] : EntityStorage[entityID].Fields)
			fieldStorage.m_ValueBuffer.Release();

		EntityStorage.erase(entityID);
	}

	/**
	 * @brief Synchronizes all entity script storage with the latest script metadata.
	 *
	 * Updates each entity's stored fields to match the current script definition, updating field types and initializing storage for any new fields that have been added to the script.
	 */
	void ScriptStorage::SynchronizeStorage()
	{
		const auto& scriptEngine = ScriptEngine::GetInstance();

		for (auto& [entityID, entityStorage] : EntityStorage)
		{
			const auto& scriptMetadata = scriptEngine.GetScriptMetadata(entityStorage.ScriptID);

			for (const auto& [fieldID, fieldMetadata] : scriptMetadata.Fields)
			{
				if (entityStorage.Fields.find(fieldID) != entityStorage.Fields.end())
				{
					entityStorage.Fields[fieldID].m_Type = fieldMetadata.ManagedType;
					continue;
				}


				InitializeFieldStorage(entityStorage, fieldID, fieldMetadata);
			}
		}
	}

	/**
	 * @brief Copies script storage data from one entity to another within a target ScriptStorage instance.
	 *
	 * Copies all script field data and metadata from the source entity to the target entity, provided both entities are associated with the same script and the target entity's storage has been initialized. Fields that no longer exist in the script are skipped.
	 *
	 * @param entityID The ID of the source entity whose script storage will be copied.
	 * @param targetEntityID The ID of the target entity to receive the copied script storage.
	 * @param targetStorage The ScriptStorage instance where the target entity's storage resides.
	 */
	void ScriptStorage::CopyEntityStorage(UUID entityID, UUID targetEntityID, ScriptStorage& targetStorage) const
	{
		if (targetStorage.EntityStorage.find(targetEntityID) == targetStorage.EntityStorage.end())
		{
			SE_CORE_ERROR("Cannot copy script storage to entity {} because InitializeScriptStorage hasn't been called for it.", targetEntityID);
			return;
		}

		const auto& scriptEngine = ScriptEngine::GetInstance();
		const auto& srcStorage = EntityStorage.at(entityID);

		if (!scriptEngine.IsValidScript(srcStorage.ScriptID))
		{
			SE_CORE_ERROR("Cannot copy script data for script ID {}. The script is no longer valid", srcStorage.ScriptID);
			return;
		}

		auto& dstStorage = targetStorage.EntityStorage.at(targetEntityID);

		if (dstStorage.ScriptID != srcStorage.ScriptID)
		{
			SE_CORE_ERROR("Cannot copy script storage from entity {} to entity {} because they have different scripts!", entityID, targetEntityID);
			return;
		}

		const auto& scriptMetadata = scriptEngine.GetScriptMetadata(srcStorage.ScriptID);

		dstStorage.InstanceIndex = EntityScriptStorage::InvalidInstanceIndex;

		for (const auto& [fieldID, fieldStorage] : srcStorage.Fields)
		{
			if (scriptMetadata.Fields.find(fieldID) == scriptMetadata.Fields.end())
			{
				SE_CORE_ERROR("Cannot copy script data for field {}. The field is no longer contained in the script.", fieldStorage.GetName());
				continue;
			}

			auto& otherFieldStorage = dstStorage.Fields[fieldID];
			otherFieldStorage.m_Name = fieldStorage.m_Name;
			otherFieldStorage.m_Type = fieldStorage.m_Type;
			otherFieldStorage.m_DataType = fieldStorage.m_DataType;
			otherFieldStorage.m_ValueBuffer = Buffer::Copy(fieldStorage.m_ValueBuffer);
			otherFieldStorage.m_InstanceIndex = FieldStorage::InvalidInstanceIndex;
		}
	}

	/**
	 * @brief Releases all field value buffers and clears all entity script storage.
	 *
	 * Frees memory associated with all script field values for every entity and removes all entities from storage.
	 */
	void ScriptStorage::Clear()
	{
		for (auto& [entityID, entityStorage] : EntityStorage)
		{
			for (auto& [fieldID, fieldStorage] : entityStorage.Fields)
				fieldStorage.m_ValueBuffer.Release();
		}

		EntityStorage.clear();
	}

	/**
	 * @brief Initializes storage for a single script field within an entity.
	 *
	 * Sets up the field's name, managed type, data type, and allocates or copies the value buffer based on the field's default value. The field's instance index is set to an invalid state.
	 */
	void ScriptStorage::InitializeFieldStorage(EntityScriptStorage& storage, uint32_t fieldID, const FieldMetadata& fieldMetadata)
	{
		auto& fieldStorage = storage.Fields[fieldID];
		fieldStorage.m_Name = fieldMetadata.Name;
		fieldStorage.m_Type = fieldMetadata.ManagedType;
		fieldStorage.m_DataType = fieldMetadata.Type;

		if (fieldMetadata.DefaultValue.Data == nullptr || fieldMetadata.DefaultValue.Size == 0)
		{
			fieldStorage.m_ValueBuffer.Allocate(DataTypeSize(fieldMetadata.Type));
		}
		else
		{
			fieldStorage.m_ValueBuffer = Buffer::Copy(fieldMetadata.DefaultValue);
		}

		fieldStorage.m_InstanceIndex = FieldStorage::InvalidInstanceIndex;
	}

}
