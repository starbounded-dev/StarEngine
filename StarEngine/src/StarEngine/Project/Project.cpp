#include "sepch.h"
#include "Project.h"

#include "ProjectSerializer.h"
#include "StarEngine/Audio/AudioEngine.h"
#include "StarEngine/Scripting/ScriptEngine.h"

namespace StarEngine {

	/**
	 * @brief Returns the absolute path to an asset within the project's asset directory.
	 *
	 * Combines the project's asset directory with the given relative asset path.
	 *
	 * @param path Relative path to the asset within the asset directory.
	 * @return std::filesystem::path Absolute path to the specified asset.
	 */
	std::filesystem::path Project::GetAssetAbsolutePath(const std::filesystem::path& path)
	{
		return GetAssetDirectory() / path;
	}

	/**
	 * @brief Shuts down and reinitializes the scripting engine with the active project.
	 *
	 * Ensures the scripting engine is reset and associated with the current active project instance.
	 */
	void Project::ReloadScriptEngine()
	{
		auto& scriptEngine = ScriptEngine::GetMutable();
		scriptEngine.Shutdown();
		scriptEngine.Initialize(s_ActiveProject);
	}

	/**
	 * @brief Creates a new project instance and sets it as the active project.
	 *
	 * @return Reference to the newly created project.
	 */
	Ref<Project> Project::New()
	{
		s_ActiveProject = CreateRef<Project>();
		return s_ActiveProject;
	}

	/**
	 * @brief Loads a project from the specified file path.
	 *
	 * Shuts down the scripting engine if an active project exists, then attempts to load a new project from the given path. If loading succeeds, resets and reinitializes the audio engine, updates the project directory, assigns the new project as active, recreates and deserializes the editor asset manager, and initializes the scripting engine with the loaded project.
	 *
	 * @param path Path to the project file to load.
	 * @return Reference to the loaded project if successful, or nullptr if loading fails.
	 */
	Ref<Project> Project::Load(const std::filesystem::path& path)
	{
		if (s_ActiveProject)
			ScriptEngine::GetMutable().Shutdown();

		Ref<Project> project = CreateRef<Project>();

		ProjectSerializer serializer(project);
		if (serializer.Serialize(path))
		{
			if (AudioEngine::HasInitializedEngine())
			{
				AudioEngine::Shutdown();
				AudioEngine::SetInitalizedEngine(false);
			}

			project->m_ProjectDirectory = path.parent_path();
			s_ActiveProject = project;

			std::shared_ptr<EditorAssetManager> editorAssetManager = std::make_shared<EditorAssetManager>();
			s_ActiveProject->m_AssetManager = editorAssetManager;
			editorAssetManager->DeserializeAssetRegistry();

			if (!AudioEngine::HasInitializedEngine())
			{
				AudioEngine::Init();
				AudioEngine::SetInitalizedEngine(true);
			}

			if (s_ActiveProject)
				ScriptEngine::GetMutable().Initialize(project);

			return s_ActiveProject;
		}

		return nullptr;
	}

	/**
	 * @brief Saves the currently active project to the specified file path.
	 *
	 * Updates the project's directory to the parent of the save location upon successful serialization.
	 *
	 * @param path The file path where the active project should be saved.
	 * @return true if the project was saved successfully; false otherwise.
	 */
	bool Project::SaveActive(const std::filesystem::path& path)
	{
		ProjectSerializer serializer(s_ActiveProject);
		if (serializer.Deserialize(path))
		{
			s_ActiveProject->m_ProjectDirectory = path.parent_path();
			return true;
		}

		return false;
	}

	/**
	 * @brief Updates the editor asset manager for the active project and resets the audio engine.
	 *
	 * Shuts down and reinitializes the audio engine, creates a new editor asset manager, assigns it to the active project, and deserializes the asset registry.
	 */
	void Project::UpdateEditorAssetManager()
	{
		if (AudioEngine::HasInitializedEngine())
		{
			AudioEngine::Shutdown();
			AudioEngine::SetInitalizedEngine(false);
		}

		std::shared_ptr<EditorAssetManager> editorAssetManager = std::make_shared<EditorAssetManager>();
		s_ActiveProject->m_AssetManager = editorAssetManager;
		editorAssetManager->DeserializeAssetRegistry();

		if (!AudioEngine::HasInitializedEngine())
		{
			AudioEngine::Init();
			AudioEngine::SetInitalizedEngine(true);
		}
	}

}
