#include "sepch.h"
#include "ScriptGlue.h"

#include "StarEngine/Asset/AssetManager.h"

#include "StarEngine/Core/Application.h"
#include "StarEngine/Utils/Hash.h"
#include "StarEngine/Scene/Entity.h"
#include "StarEngine/Scene/Components.h"
#include "StarEngine/Physics/ContactListener2D.h"

#include "StarEngine/Scripting/ScriptEngine.h"

#include "StarEngine/Utils/TypeInfo.h"

#include "StarEngine/Core/KeyCodes.h"

//#include <Coral/ManagedObject.hpp>
//#include <Coral/HostInstance.hpp>

#include <functional>

namespace StarEngine {

#ifdef SE_PLATFORM_WINDOWS
#define SE_FUNCTION_NAME __func__
#else
#define SE_FUNCTION_NAME __FUNCTION__
#endif

#define SE_ADD_INTERNAL_CALL(icall) coreAssembly.AddInternalCall("StarEngine.InternalCalls", #icall, (void*)InternalCalls::icall)

#ifdef SE_DIST
#define SE_ICALL_VALIDATE_PARAM(param) SE_CORE_VERIFY(param, "{} called with an invalid value ({}) for parameter '{}'", SE_FUNCTION_NAME, param, #param)
#define SE_ICALL_VALIDATE_PARAM_V(param, value) SE_CORE_VERIFY(param, "{} called with an invalid value ({}) for parameter '{}'.\nStack Trace: {}", SE_FUNCTION_NAME, value, #param, ScriptUtils::GetCurrentStackTrace())
#else
#define SE_ICALL_VALIDATE_PARAM(param) { if (!(param)) { SE_CONSOLE_LOG_ERROR("{} called with an invalid value ({}) for parameter '{}'", SE_FUNCTION_NAME, param, #param); } }
#define SE_ICALL_VALIDATE_PARAM_V(param, value) { if (!(param)) { SE_CONSOLE_LOG_ERROR("{} called with an invalid value ({}) for parameter '{}'.", SE_FUNCTION_NAME, value, #param); } }
#endif

	bool ScriptGlue::s_CalledSetCursor = false;
	bool ScriptGlue::s_ChangedCursor = false;
	glm::vec2 ScriptGlue::s_CursorHotSpot = glm::vec2(0.0f, 0.0f);
	std::string ScriptGlue::s_SetCursorPath = "";

	Entity s_HoveredEntity;
	Entity s_SelectedEntity;

	std::unordered_map<Coral::TypeId, std::function<void(Entity&)>> s_CreateComponentFuncs;
	std::unordered_map<Coral::TypeId, std::function<bool(Entity&)>> s_HasComponentFuncs;
	std::unordered_map<Coral::TypeId, std::function<void(Entity&)>> s_RemoveComponentFuncs;

	template<typename TComponent>
	static void RegisterManagedComponent(Coral::ManagedAssembly& coreAssembly)
	{
		// NOTE(Peter): Get the demangled type name of TComponent
		const TypeNameString& componentTypeName = TypeInfo<TComponent, true>().Name();
		std::string componentName = fmt::format("StarEngine.{}", componentTypeName);

		auto& type = coreAssembly.GetType(componentName);

		if (type)
		{
			s_CreateComponentFuncs[type.GetTypeId()] = [](Entity& entity) { entity.AddComponent<TComponent>(); };
			s_HasComponentFuncs[type.GetTypeId()] = [](Entity& entity) { return entity.HasComponent<TComponent>(); };
			s_RemoveComponentFuncs[type.GetTypeId()] = [](Entity& entity) { entity.RemoveComponent<TComponent>(); };
		}
		else
		{
			SE_CORE_ERROR("No C# component class found for {}!", componentName);
			SE_CORE_VERIFY(false, "No C# component class found!");
		}
	}

	template<typename TComponent>
	static void RegisterManagedComponent(std::function<void(Entity&)>&& addFunction, Coral::ManagedAssembly& coreAssembly)
	{
		// NOTE(Peter): Get the demangled type name of TComponent
		const TypeNameString& componentTypeName = TypeInfo<TComponent, true>().Name();
		std::string componentName = fmt::format("StarEngine.{}", componentTypeName);

		auto& type = coreAssembly.GetType(componentName);

		if (type)
		{
			s_CreateComponentFuncs[type.GetTypeId()] = std::move(addFunction);
			s_HasComponentFuncs[type.GetTypeId()] = [](Entity& entity) { return entity.HasComponent<TComponent>(); };
			s_RemoveComponentFuncs[type.GetTypeId()] = [](Entity& entity) { entity.RemoveComponent<TComponent>(); };
		}
		else
		{
			SE_CORE_ERROR("No C# component class found for {}!", componentName);
			SE_CORE_VERIFY(false, "No C# component class found!");
		}
	}

	template<typename... TArgs>
	static void WarnWithTrace(const std::string& inFormat, TArgs&&... inArgs)
	{
		/*auto stackTrace = ScriptUtils::GetCurrentStackTrace();
		std::string formattedMessage = fmt::format(inFormat, std::forward<TArgs>(inArgs)...);
		Log::GetEditorConsoleLogger()->warn("{}\nStack Trace: {}", formattedMessage, stackTrace);*/
	}

	template<typename... TArgs>
	static void ErrorWithTrace(const std::string& inFormat, TArgs&&... inArgs)
	{
		/*auto stackTrace = ScriptUtils::GetCurrentStackTrace();
		std::string formattedMessage = fmt::format(inFormat, std::forward<TArgs>(inArgs)...);
		Log::GetEditorConsoleLogger()->error("{}\nStack Trace: {}", formattedMessage, stackTrace);*/
	}

	void ScriptGlue::RegisterGlue(Coral::ManagedAssembly& coreAssembly)
	{
		if (!s_CreateComponentFuncs.empty())
		{
			s_CreateComponentFuncs.clear();
			s_HasComponentFuncs.clear();
			s_RemoveComponentFuncs.clear();
		}

		RegisterComponentTypes(coreAssembly);
		RegisterInternalCalls(coreAssembly);

		coreAssembly.UploadInternalCalls();
	}

	void ScriptGlue::RegisterComponentTypes(Coral::ManagedAssembly& coreAssembly)
	{
		RegisterManagedComponent<TagComponent>(coreAssembly);
		RegisterManagedComponent<TransformComponent>(coreAssembly);
		RegisterManagedComponent<SpriteRendererComponent>(coreAssembly);
		RegisterManagedComponent<CircleRendererComponent>(coreAssembly);
		RegisterManagedComponent<TextComponent>(coreAssembly);
		RegisterManagedComponent<CameraComponent>(coreAssembly);
		RegisterManagedComponent<RigidBody2DComponent>(coreAssembly);
		RegisterManagedComponent<BoxCollider2DComponent>(coreAssembly);
		RegisterManagedComponent<CircleCollider2DComponent>(coreAssembly);
		RegisterManagedComponent<ScriptComponent>(coreAssembly);
		RegisterManagedComponent<AudioListenerComponent>(coreAssembly);
		RegisterManagedComponent<AudioSourceComponent>(coreAssembly);
	}

	void ScriptGlue::RegisterInternalCalls(Coral::ManagedAssembly& coreAssembly)
	{
		SE_ADD_INTERNAL_CALL(NativeLog);
		SE_ADD_INTERNAL_CALL(NativeLog_Vector);
		SE_ADD_INTERNAL_CALL(NativeLog_VectorDot);

		SE_ADD_INTERNAL_CALL(GetScriptInstance);

		SE_ADD_INTERNAL_CALL(Entity_HasComponent);
		SE_ADD_INTERNAL_CALL(Entity_FindEntityByName);

		SE_ADD_INTERNAL_CALL(TransformComponent_GetTranslation);
		SE_ADD_INTERNAL_CALL(TransformComponent_SetTranslation);

		SE_ADD_INTERNAL_CALL(RigidBody2DComponent_ApplyLinearImpulse);
		SE_ADD_INTERNAL_CALL(RigidBody2DComponent_ApplyLinearImpulseToCenter);
		SE_ADD_INTERNAL_CALL(RigidBody2DComponent_GetLinearVelocity);
		SE_ADD_INTERNAL_CALL(RigidBody2DComponent_GetType);
		SE_ADD_INTERNAL_CALL(RigidBody2DComponent_SetType);

		SE_ADD_INTERNAL_CALL(TextComponent_GetText);
		SE_ADD_INTERNAL_CALL(TextComponent_SetText);
		SE_ADD_INTERNAL_CALL(TextComponent_GetColor);
		SE_ADD_INTERNAL_CALL(TextComponent_SetColor);
		SE_ADD_INTERNAL_CALL(TextComponent_GetKerning);
		SE_ADD_INTERNAL_CALL(TextComponent_SetKerning);
		SE_ADD_INTERNAL_CALL(TextComponent_GetLineSpacing);
		SE_ADD_INTERNAL_CALL(TextComponent_SetLineSpacing);

		SE_ADD_INTERNAL_CALL(Input_IsKeyDown);

		// AudioListener
		SE_ADD_INTERNAL_CALL(AudioListenerComponent_GetActive);
		SE_ADD_INTERNAL_CALL(AudioListenerComponent_SetActive);

		// AudioSource
		SE_ADD_INTERNAL_CALL(AudioSourceComponent_GetAssetHandle);
		SE_ADD_INTERNAL_CALL(AudioSourceComponent_SetAssetHandle);
		SE_ADD_INTERNAL_CALL(AudioSourceComponent_GetVolume);
		SE_ADD_INTERNAL_CALL(AudioSourceComponent_SetVolume);
		SE_ADD_INTERNAL_CALL(AudioSourceComponent_GetPitch);
		SE_ADD_INTERNAL_CALL(AudioSourceComponent_SetPitch);
		SE_ADD_INTERNAL_CALL(AudioSourceComponent_GetPlayOnAwake);
		SE_ADD_INTERNAL_CALL(AudioSourceComponent_SetPlayOnAwake);
		SE_ADD_INTERNAL_CALL(AudioSourceComponent_GetLooping);
		SE_ADD_INTERNAL_CALL(AudioSourceComponent_SetLooping);
		SE_ADD_INTERNAL_CALL(AudioSourceComponent_GetSpatialization);
		SE_ADD_INTERNAL_CALL(AudioSourceComponent_SetSpatialization);
		SE_ADD_INTERNAL_CALL(AudioSourceComponent_GetAttenuationModel);
		SE_ADD_INTERNAL_CALL(AudioSourceComponent_SetAttenuationModel);
		SE_ADD_INTERNAL_CALL(AudioSourceComponent_GetRollOff);
		SE_ADD_INTERNAL_CALL(AudioSourceComponent_SetRollOff);
		SE_ADD_INTERNAL_CALL(AudioSourceComponent_GetMinGain);
		SE_ADD_INTERNAL_CALL(AudioSourceComponent_SetMinGain);
		SE_ADD_INTERNAL_CALL(AudioSourceComponent_GetMaxGain);
		SE_ADD_INTERNAL_CALL(AudioSourceComponent_SetMaxGain);
		SE_ADD_INTERNAL_CALL(AudioSourceComponent_GetMinDistance);
		SE_ADD_INTERNAL_CALL(AudioSourceComponent_SetMinDistance);
		SE_ADD_INTERNAL_CALL(AudioSourceComponent_GetMaxDistance);
		SE_ADD_INTERNAL_CALL(AudioSourceComponent_SetMaxDistance);
		SE_ADD_INTERNAL_CALL(AudioSourceComponent_GetConeInnerAngle);
		SE_ADD_INTERNAL_CALL(AudioSourceComponent_SetConeInnerAngle);
		SE_ADD_INTERNAL_CALL(AudioSourceComponent_GetConeOuterAngle);
		SE_ADD_INTERNAL_CALL(AudioSourceComponent_SetConeOuterAngle);
		SE_ADD_INTERNAL_CALL(AudioSourceComponent_GetConeOuterGain);
		SE_ADD_INTERNAL_CALL(AudioSourceComponent_SetConeOuterGain);
		SE_ADD_INTERNAL_CALL(AudioSourceComponent_SetCone);
		SE_ADD_INTERNAL_CALL(AudioSourceComponent_GetDopplerFactor);
		SE_ADD_INTERNAL_CALL(AudioSourceComponent_SetDopplerFactor);
		SE_ADD_INTERNAL_CALL(AudioSourceComponent_IsPlaying);
		SE_ADD_INTERNAL_CALL(AudioSourceComponent_Play);
		SE_ADD_INTERNAL_CALL(AudioSourceComponent_Pause);
		SE_ADD_INTERNAL_CALL(AudioSourceComponent_UnPause);
		SE_ADD_INTERNAL_CALL(AudioSourceComponent_Stop);
	}

	Entity ScriptGlue::GetHoveredEntity()
	{
		return s_HoveredEntity;
	}

	void ScriptGlue::SetHoveredEntity(Entity entity)
	{
		s_HoveredEntity = entity;
	}

	Entity ScriptGlue::GetSelectedEntity()
	{
		return s_SelectedEntity;
	}

	void ScriptGlue::SetSelectedEntity(Entity entity)
	{
		s_SelectedEntity = entity;
	}

	namespace InternalCalls
	{
		template<std::default_initializable T>
		struct Param
		{
			std::byte Data [sizeof(T)];

			operator T() const
			{
				T result;
				std::memcpy(&result, Data, sizeof(T));
				return result;
			}
		};

		bool Input_IsKeyDown(KeyCode keycode);

	}
}
