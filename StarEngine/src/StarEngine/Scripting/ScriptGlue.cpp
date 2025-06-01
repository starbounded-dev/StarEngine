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
			#if defined(_MSC_VER)
						__debugbreak();
			#else
						__builtin_trap();
			#endif
			return;
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
			#if defined(_MSC_VER)
						__debugbreak();
			#else
						__builtin_trap();
			#endif
			return;
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
		SE_ADD_INTERNAL_CALL(AssetHandle_IsValid);

		// ConsoleLog
		SE_ADD_INTERNAL_CALL(ConsoleLog_Trace);
		SE_ADD_INTERNAL_CALL(ConsoleLog_Info);
		SE_ADD_INTERNAL_CALL(ConsoleLog_Warning);
		SE_ADD_INTERNAL_CALL(ConsoleLog_Error);
		SE_ADD_INTERNAL_CALL(ConsoleLog_Critical);

		// Input
		SE_ADD_INTERNAL_CALL(Input_IsKeyDown);
		SE_ADD_INTERNAL_CALL(Input_IsKeyUp);
		SE_ADD_INTERNAL_CALL(Input_IsMouseButtonPressed);
		SE_ADD_INTERNAL_CALL(Input_PressMouseButton);
		SE_ADD_INTERNAL_CALL(Input_ReleaseMouseButton);
		SE_ADD_INTERNAL_CALL(Input_GetMousePositionX);
		SE_ADD_INTERNAL_CALL(Input_GetMousePositionY);
		SE_ADD_INTERNAL_CALL(Input_GetMouseWorldPositionX);
		SE_ADD_INTERNAL_CALL(Input_GetMouseWorldPositionY);

		// Application
		SE_ADD_INTERNAL_CALL(Application_GetFPS);
		SE_ADD_INTERNAL_CALL(Application_GetFrameTime);
		SE_ADD_INTERNAL_CALL(Application_GetMinFrameTime);
		SE_ADD_INTERNAL_CALL(Application_GetMaxFrameTime);

		// Scene
		SE_ADD_INTERNAL_CALL(Scene_LoadScene);
		SE_ADD_INTERNAL_CALL(Scene_GetCursor);
		SE_ADD_INTERNAL_CALL(Scene_SetCursor);
		SE_ADD_INTERNAL_CALL(Scene_GetMouseHotSpotX);
		SE_ADD_INTERNAL_CALL(Scene_GetMouseHotSpotY);
		SE_ADD_INTERNAL_CALL(Scene_SetMouseHotSpot);
		SE_ADD_INTERNAL_CALL(Scene_ChangeCursor);
		SE_ADD_INTERNAL_CALL(Scene_CloseApplication);
		SE_ADD_INTERNAL_CALL(Scene_GetName);
		SE_ADD_INTERNAL_CALL(Scene_SetName);
		SE_ADD_INTERNAL_CALL(Scene_IsGamePaused);
		SE_ADD_INTERNAL_CALL(Scene_SetPauseGame);
		SE_ADD_INTERNAL_CALL(Scene_CreateEntity);
		SE_ADD_INTERNAL_CALL(Scene_IsEntityValid);
		SE_ADD_INTERNAL_CALL(Scene_GetHoveredEntity);
		SE_ADD_INTERNAL_CALL(Scene_GetSelectedEntity);
		SE_ADD_INTERNAL_CALL(Scene_SetSelectedEntity);
		SE_ADD_INTERNAL_CALL(Scene_RenderHoveredEntityOutline);
		SE_ADD_INTERNAL_CALL(Scene_RenderSelectedEntityOutline);
		SE_ADD_INTERNAL_CALL(Scene_GetEntityComponent);

		// Entity
		SE_ADD_INTERNAL_CALL(Entity_CreateComponent);
		SE_ADD_INTERNAL_CALL(Entity_HasComponent);
		SE_ADD_INTERNAL_CALL(Entity_RemoveComponent);
		SE_ADD_INTERNAL_CALL(Entity_RemoveComponent);
		SE_ADD_INTERNAL_CALL(Entity_DestroyEntity);
		SE_ADD_INTERNAL_CALL(Entity_FindEntityByTag);
		SE_ADD_INTERNAL_CALL(Entity_FindEntityByName);

		// Tag
		SE_ADD_INTERNAL_CALL(TagComponent_GetTag);
		SE_ADD_INTERNAL_CALL(TagComponent_SetTag);

		// Transform
		SE_ADD_INTERNAL_CALL(TransformComponent_GetIsEnabled);
		SE_ADD_INTERNAL_CALL(TransformComponent_SetIsEnabled);
		SE_ADD_INTERNAL_CALL(TransformComponent_GetTransform);
		SE_ADD_INTERNAL_CALL(TransformComponent_SetTransform);
		SE_ADD_INTERNAL_CALL(TransformComponent_GetTranslationX);
		SE_ADD_INTERNAL_CALL(TransformComponent_GetTranslationY);
		SE_ADD_INTERNAL_CALL(TransformComponent_GetTranslationZ);
		SE_ADD_INTERNAL_CALL(TransformComponent_SetTranslation);
		SE_ADD_INTERNAL_CALL(TransformComponent_GetRotationX);
		SE_ADD_INTERNAL_CALL(TransformComponent_GetRotationY);
		SE_ADD_INTERNAL_CALL(TransformComponent_GetRotationZ);
		SE_ADD_INTERNAL_CALL(TransformComponent_SetRotation);
		SE_ADD_INTERNAL_CALL(TransformComponent_GetScaleX);
		SE_ADD_INTERNAL_CALL(TransformComponent_GetScaleY);
		SE_ADD_INTERNAL_CALL(TransformComponent_GetScaleZ);
		SE_ADD_INTERNAL_CALL(TransformComponent_SetScale);

		// CameraComponent
		SE_ADD_INTERNAL_CALL(CameraComponent_GetIsPrimary);
		SE_ADD_INTERNAL_CALL(CameraComponent_SetPrimary);
		SE_ADD_INTERNAL_CALL(CameraComponent_GetFixedAspectRatio);
		SE_ADD_INTERNAL_CALL(CameraComponent_SetFixedAspectRatio);

		// SpriteRenderer
		SE_ADD_INTERNAL_CALL(SpriteRendererComponent_GetOffsetX);
		SE_ADD_INTERNAL_CALL(SpriteRendererComponent_GetOffsetY);
		SE_ADD_INTERNAL_CALL(SpriteRendererComponent_GetOffsetZ);
		SE_ADD_INTERNAL_CALL(SpriteRendererComponent_SetOffset);
		SE_ADD_INTERNAL_CALL(SpriteRendererComponent_GetColorX);
		SE_ADD_INTERNAL_CALL(SpriteRendererComponent_GetColorY);
		SE_ADD_INTERNAL_CALL(SpriteRendererComponent_GetColorZ);
		SE_ADD_INTERNAL_CALL(SpriteRendererComponent_GetColorW);
		SE_ADD_INTERNAL_CALL(SpriteRendererComponent_SetColor);
		SE_ADD_INTERNAL_CALL(SpriteRendererComponent_GetUVX);
		SE_ADD_INTERNAL_CALL(SpriteRendererComponent_GetUVY);
		SE_ADD_INTERNAL_CALL(SpriteRendererComponent_SetUV);
		SE_ADD_INTERNAL_CALL(SpriteRendererComponent_GetUseParallax);
		SE_ADD_INTERNAL_CALL(SpriteRendererComponent_SetUseParallax);
		SE_ADD_INTERNAL_CALL(SpriteRendererComponent_GetParallaxSpeedX);
		SE_ADD_INTERNAL_CALL(SpriteRendererComponent_GetParallaxSpeedY);
		SE_ADD_INTERNAL_CALL(SpriteRendererComponent_SetParallaxSpeed);
		SE_ADD_INTERNAL_CALL(SpriteRendererComponent_GetParallaxDivision);
		SE_ADD_INTERNAL_CALL(SpriteRendererComponent_SetParallaxDivision);
		SE_ADD_INTERNAL_CALL(SpriteRendererComponent_GetUseTextureAtlasAnimation);
		SE_ADD_INTERNAL_CALL(SpriteRendererComponent_SetUseTextureAtlasAnimation);
		SE_ADD_INTERNAL_CALL(SpriteRendererComponent_GetAnimationSpeed);
		SE_ADD_INTERNAL_CALL(SpriteRendererComponent_SetAnimationSpeed);
		SE_ADD_INTERNAL_CALL(SpriteRendererComponent_GetNumTiles);
		SE_ADD_INTERNAL_CALL(SpriteRendererComponent_SetNumTiles);
		SE_ADD_INTERNAL_CALL(SpriteRendererComponent_GetStartIndexX);
		SE_ADD_INTERNAL_CALL(SpriteRendererComponent_SetStartIndexX);
		SE_ADD_INTERNAL_CALL(SpriteRendererComponent_GetStartIndexY);
		SE_ADD_INTERNAL_CALL(SpriteRendererComponent_SetStartIndexY);
		SE_ADD_INTERNAL_CALL(SpriteRendererComponent_GetColumn);
		SE_ADD_INTERNAL_CALL(SpriteRendererComponent_SetColumn);
		SE_ADD_INTERNAL_CALL(SpriteRendererComponent_GetRow);
		SE_ADD_INTERNAL_CALL(SpriteRendererComponent_SetRow);
		SE_ADD_INTERNAL_CALL(SpriteRendererComponent_GetSaturation);
		SE_ADD_INTERNAL_CALL(SpriteRendererComponent_SetSaturation);
		SE_ADD_INTERNAL_CALL(SpriteRendererComponent_GetTextureAssetHandle);
		SE_ADD_INTERNAL_CALL(SpriteRendererComponent_SetTextureAssetHandle);
		SE_ADD_INTERNAL_CALL(SpriteRendererComponent_GetTextureAssetID);
		SE_ADD_INTERNAL_CALL(SpriteRendererComponent_SetTextureAssetID);

		// CircleRenderer
		SE_ADD_INTERNAL_CALL(CircleRendererComponent_GetColorX);
		SE_ADD_INTERNAL_CALL(CircleRendererComponent_GetColorY);
		SE_ADD_INTERNAL_CALL(CircleRendererComponent_GetColorZ);
		SE_ADD_INTERNAL_CALL(CircleRendererComponent_GetColorW);
		SE_ADD_INTERNAL_CALL(CircleRendererComponent_SetColor);
		SE_ADD_INTERNAL_CALL(CircleRendererComponent_GetUVX);
		SE_ADD_INTERNAL_CALL(CircleRendererComponent_GetUVY);
		SE_ADD_INTERNAL_CALL(CircleRendererComponent_SetUV);
		SE_ADD_INTERNAL_CALL(CircleRendererComponent_GetUseParallax);
		SE_ADD_INTERNAL_CALL(CircleRendererComponent_SetUseParallax);
		SE_ADD_INTERNAL_CALL(CircleRendererComponent_GetParallaxSpeedX);
		SE_ADD_INTERNAL_CALL(CircleRendererComponent_GetParallaxSpeedY);
		SE_ADD_INTERNAL_CALL(CircleRendererComponent_SetParallaxSpeed);
		SE_ADD_INTERNAL_CALL(CircleRendererComponent_GetParallaxDivision);
		SE_ADD_INTERNAL_CALL(CircleRendererComponent_SetParallaxDivision);
		SE_ADD_INTERNAL_CALL(CircleRendererComponent_GetUseTextureAtlasAnimation);
		SE_ADD_INTERNAL_CALL(CircleRendererComponent_SetUseTextureAtlasAnimation);
		SE_ADD_INTERNAL_CALL(CircleRendererComponent_GetAnimationSpeed);
		SE_ADD_INTERNAL_CALL(CircleRendererComponent_SetAnimationSpeed);
		SE_ADD_INTERNAL_CALL(CircleRendererComponent_GetNumTiles);
		SE_ADD_INTERNAL_CALL(CircleRendererComponent_SetNumTiles);
		SE_ADD_INTERNAL_CALL(CircleRendererComponent_GetStartIndexX);
		SE_ADD_INTERNAL_CALL(CircleRendererComponent_SetStartIndexX);
		SE_ADD_INTERNAL_CALL(CircleRendererComponent_GetStartIndexY);
		SE_ADD_INTERNAL_CALL(CircleRendererComponent_SetStartIndexY);
		SE_ADD_INTERNAL_CALL(CircleRendererComponent_GetColumn);
		SE_ADD_INTERNAL_CALL(CircleRendererComponent_SetColumn);
		SE_ADD_INTERNAL_CALL(CircleRendererComponent_GetRow);
		SE_ADD_INTERNAL_CALL(CircleRendererComponent_SetRow);

		// LineRenderer
		SE_ADD_INTERNAL_CALL(LineRendererComponent_GetLineThickness);
		SE_ADD_INTERNAL_CALL(LineRendererComponent_SetLineThickness);

		// TextRenderer
		SE_ADD_INTERNAL_CALL(TextComponent_GetText);
		SE_ADD_INTERNAL_CALL(TextComponent_SetText);
		SE_ADD_INTERNAL_CALL(TextComponent_GetColorX);
		SE_ADD_INTERNAL_CALL(TextComponent_GetColorY);
		SE_ADD_INTERNAL_CALL(TextComponent_GetColorZ);
		SE_ADD_INTERNAL_CALL(TextComponent_GetColorW);
		SE_ADD_INTERNAL_CALL(TextComponent_SetColor);
		SE_ADD_INTERNAL_CALL(TextComponent_GetKerning);
		SE_ADD_INTERNAL_CALL(TextComponent_SetKerning);
		SE_ADD_INTERNAL_CALL(TextComponent_GetLineSpacing);
		SE_ADD_INTERNAL_CALL(TextComponent_SetLineSpacing);

		// RigidBody2D
		SE_ADD_INTERNAL_CALL(RigidBody2DComponent_ApplyLinearImpulse);
		SE_ADD_INTERNAL_CALL(RigidBody2DComponent_ApplyLinearImpulseToCenter);
		SE_ADD_INTERNAL_CALL(RigidBody2DComponent_GetLinearVelocityX);
		SE_ADD_INTERNAL_CALL(RigidBody2DComponent_GetLinearVelocityY);
		SE_ADD_INTERNAL_CALL(RigidBody2DComponent_SetLinearVelocity);
		SE_ADD_INTERNAL_CALL(RigidBody2DComponent_GetType);
		SE_ADD_INTERNAL_CALL(RigidBody2DComponent_SetType);
		SE_ADD_INTERNAL_CALL(RigidBody2DComponent_GetGravity);
		SE_ADD_INTERNAL_CALL(RigidBody2DComponent_GetGravityX);
		SE_ADD_INTERNAL_CALL(RigidBody2DComponent_GetGravityY);
		SE_ADD_INTERNAL_CALL(RigidBody2DComponent_SetGravity);
		SE_ADD_INTERNAL_CALL(RigidBody2DComponent_GetEnabled);
		SE_ADD_INTERNAL_CALL(RigidBody2DComponent_SetEnabled);

		// BoxCollider2D
		SE_ADD_INTERNAL_CALL(BoxCollider2DComponent_GetOffset);
		SE_ADD_INTERNAL_CALL(BoxCollider2DComponent_GetOffsetX);
		SE_ADD_INTERNAL_CALL(BoxCollider2DComponent_GetOffsetY);
		SE_ADD_INTERNAL_CALL(BoxCollider2DComponent_SetOffset);
		SE_ADD_INTERNAL_CALL(BoxCollider2DComponent_GetSizeX);
		SE_ADD_INTERNAL_CALL(BoxCollider2DComponent_GetSizeY);
		SE_ADD_INTERNAL_CALL(BoxCollider2DComponent_SetSize);
		SE_ADD_INTERNAL_CALL(BoxCollider2DComponent_GetDensity);
		SE_ADD_INTERNAL_CALL(BoxCollider2DComponent_SetDensity);
		SE_ADD_INTERNAL_CALL(BoxCollider2DComponent_GetFriction);
		SE_ADD_INTERNAL_CALL(BoxCollider2DComponent_SetFriction);
		SE_ADD_INTERNAL_CALL(BoxCollider2DComponent_GetRestitution);
		SE_ADD_INTERNAL_CALL(BoxCollider2DComponent_SetRestitution);
		SE_ADD_INTERNAL_CALL(BoxCollider2DComponent_GetRestitutionThreshold);
		SE_ADD_INTERNAL_CALL(BoxCollider2DComponent_SetRestitutionThreshold);
		SE_ADD_INTERNAL_CALL(BoxCollider2DComponent_GetCollisionRayX);
		SE_ADD_INTERNAL_CALL(BoxCollider2DComponent_GetCollisionRayY);
		SE_ADD_INTERNAL_CALL(BoxCollider2DComponent_SetCollisionRay);
		SE_ADD_INTERNAL_CALL(BoxCollider2DComponent_GetAwake);
		SE_ADD_INTERNAL_CALL(BoxCollider2DComponent_SetAwake);

		// CircleCollider2D
		SE_ADD_INTERNAL_CALL(CircleCollider2DComponent_GetOffsetX);
		SE_ADD_INTERNAL_CALL(CircleCollider2DComponent_GetOffsetY);
		SE_ADD_INTERNAL_CALL(CircleCollider2DComponent_SetOffset);
		SE_ADD_INTERNAL_CALL(CircleCollider2DComponent_GetRadius);
		SE_ADD_INTERNAL_CALL(CircleCollider2DComponent_SetRadius);
		SE_ADD_INTERNAL_CALL(CircleCollider2DComponent_GetDensity);
		SE_ADD_INTERNAL_CALL(CircleCollider2DComponent_SetDensity);
		SE_ADD_INTERNAL_CALL(CircleCollider2DComponent_GetFriction);
		SE_ADD_INTERNAL_CALL(CircleCollider2DComponent_SetFriction);
		SE_ADD_INTERNAL_CALL(CircleCollider2DComponent_GetRestitution);
		SE_ADD_INTERNAL_CALL(CircleCollider2DComponent_SetRestitution);
		SE_ADD_INTERNAL_CALL(CircleCollider2DComponent_GetRestitutionThreshold);
		SE_ADD_INTERNAL_CALL(CircleCollider2DComponent_SetRestitutionThreshold);
		SE_ADD_INTERNAL_CALL(CircleCollider2DComponent_GetCollisionRayX);
		SE_ADD_INTERNAL_CALL(CircleCollider2DComponent_GetCollisionRayY);
		SE_ADD_INTERNAL_CALL(CircleCollider2DComponent_SetCollisionRay);
		SE_ADD_INTERNAL_CALL(CircleCollider2DComponent_GetAwake);
		SE_ADD_INTERNAL_CALL(CircleCollider2DComponent_SetAwake);

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

#pragma region AssetHandle

		bool AssetHandle_IsValid(AssetHandle assetHandle)
		{
			return AssetManager::IsAssetHandleValid(assetHandle);
		}

#pragma endregion

#pragma region Input

		bool Input_IsKeyDown(KeyCode keycode)
		{
			return Input::IsKeyPressed(keycode);
		}

		bool Input_IsKeyUp(KeyCode keycode)
		{
			return Input::IsKeyReleased(keycode);
		}

		bool Input_IsMouseButtonPressed(MouseCode button)
		{
			if (ScriptGlue::s_IsCursorInViewport)
				return Input::IsMouseButtonPressed(button);

			return false;
		}

		bool Input_PressMouseButton(MouseCode button)
		{
			if (ScriptGlue::s_IsCursorInViewport)
				return Input::PressMouseButton(button);

			return false;
		}

		bool Input_ReleaseMouseButton(MouseCode button)
		{
			if (ScriptGlue::s_IsCursorInViewport)
				return Input::ReleaseMouseButton(button);

			return false;
		}

		float Input_GetMousePositionX()
		{
			float mousePositionX = Input::GetMousePosition().x;
			return mousePositionX;
		}

		float Input_GetMousePositionY()
		{
			float mousePositionY = Input::GetMousePosition().y;
			return mousePositionY;
		}

		float Input_GetMouseWorldPositionX()
		{
			glm::vec2 mousePosition = Input::GetViewportMousePosition();
			return mousePosition.x;
		}

		float Input_GetMouseWorldPositionY()
		{
			glm::vec2 mousePosition = Input::GetViewportMousePosition();
			return mousePosition.y;
		}

#pragma endregion

#pragma region Application

		float Application_GetFPS()
		{
			return Application::Get().GetFPS();
		}
		
		float Application_GetFrameTime()
		{
			return Application::Get().GetFrameTime();
		}
		
		float Application_GetMinFrameTime()
		{
			return Application::Get().GetMinFrameTime();
		}
		
		float Application_GetMaxFrameTime()
		{
			return Application::Get().GetMaxFrameTime();
		}

#pragma endregion

#pragma region Scene

		void Scene_LoadScene(AssetHandle assetHandle)
		{
			Ref<Scene> activeScene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(activeScene, "No active scene!");
			//SE_ICALL_VALIDATE_PARAM_V(assetHandle, "nullptr");
			//SE_ICALL_VALIDATE_PARAM(AssetManager::IsAssetHandleValid(assetHandle));
			if (AssetManager::IsAssetHandleValid(assetHandle))
				activeScene->OnSceneTransition(assetHandle);
		}

		Coral::String Scene_GetCursor()
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene, "No active scene!");

			return Coral::String::New(ScriptGlue::s_SetCursorPath);
		}

		void Scene_SetCursor(Coral::String filepath)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene, "No active scene!");

			ScriptGlue::s_SetCursorPath = filepath;
			ScriptGlue::s_ChangedCursor = true;
		}

		float Scene_GetMouseHotSpotX()
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene, "No active scene!");

			glm::vec2 mouseHotSpot = ScriptGlue::s_CursorHotSpot;
			return mouseHotSpot.x;
		}

		float Scene_GetMouseHotSpotY()
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene, "No active scene!");

			glm::vec2 mouseHotSpot = ScriptGlue::s_CursorHotSpot;
			return mouseHotSpot.y;
		}

		void Scene_SetMouseHotSpot(float hotSpotX, float hotSpotY)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene, "No active scene!");

			ScriptGlue::s_CursorHotSpot = glm::vec2(hotSpotX, hotSpotY);
		}

		void Scene_ChangeCursor(Coral::NativeString filepath, glm::vec2* hotSpot)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene, "No active scene!");
		
			std::string path = ConvertCoralStringToCppString(filepath);
		
			if (ScriptGlue::s_SetCursorPath != path)
				ScriptGlue::s_SetCursorPath = path;
		
			if (ScriptGlue::s_CursorHotSpot.x != (int)hotSpot->x && ScriptGlue::s_CursorHotSpot.y != (int)hotSpot->y)
				ScriptGlue::s_CursorHotSpot = glm::ivec2((int)hotSpot->x, (int)hotSpot->y);
		
			ScriptGlue::s_ChangedCursor = true;
		}

		void Scene_ChangeCursor(Coral::String filepath, float hotSpotX, float hotspotY)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene, "No active scene!");

			if (ScriptGlue::s_SetCursorPath != filepath)
				ScriptGlue::s_SetCursorPath = filepath;

			if (ScriptGlue::s_CursorHotSpot.x != hotSpotX || ScriptGlue::s_CursorHotSpot.y != hotspotY)
				ScriptGlue::s_CursorHotSpot = glm::vec2(hotSpotX, hotspotY);

			ScriptGlue::s_ChangedCursor = true;
		}

		static void Scene_CloseApplication()
		{
			if (ScriptEngine::s_IsRuntime == true)
			{
				Application::Get().Close();
			}
		}

		Coral::String Scene_GetCurrentFilename()
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene, "No active scene!");

			return Coral::String::New(scene->GetName());
		}

		Coral::String Scene_GetName()
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene, "No active scene!");

			return Coral::String::New(scene->GetName());
		}

		void Scene_SetName(Coral::String path)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene, "No active scene!");

			scene->SetName(path);
		}

		bool Scene_IsGamePaused()
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene, "No active scene!");

			return scene->IsGamePaused();
		}

		void Scene_SetPauseGame(bool shouldPause)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene, "No active scene!");

			scene->ShouldGameBePaused(shouldPause);
		}

		uint64_t Scene_CreateEntity(Coral::String tag)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene, "No active scene!");

			return scene->CreateEntity(tag).GetEntityHandle();
		}

		bool Scene_IsEntityValid(uint64_t entityID)
		{
			if (entityID == 0)
				return false;

			return (bool)(ScriptEngine::GetInstance().GetCurrentScene()->TryGetEntityWithID(entityID));
		}

		uint64_t Scene_GetHoveredEntity()
		{
			if (!s_HoveredEntity)
				return 0;

			return s_HoveredEntity.GetEntityHandle();
		}

		uint64_t Scene_GetSelectedEntity()
		{
			if (!s_SelectedEntity)
				return 0;

			return s_SelectedEntity.GetEntityHandle();
		}

		void Scene_SetSelectedEntity(uint64_t entityID)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByID(entityID);
			SE_CORE_ASSERT(entity);

			s_SelectedEntity = entity;
		}

		void Scene_RenderHoveredEntityOutline(uint64_t entityID, float colorX, float colorY, float colorZ, float colorW)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene);

			static uint64_t finalEntityID = 0;

			if (entityID > 500)
				finalEntityID = entityID;

			Entity entity = scene->GetEntityByID(finalEntityID);
			SE_CORE_ASSERT(entity);

			scene->RenderHoveredEntityOutline(entity, glm::vec4(colorX, colorY, colorZ, colorW));
		}

		void Scene_RenderSelectedEntityOutline(uint64_t entityID, float colorX, float colorY, float colorZ, float colorW)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene);

			static uint64_t finalEntityID = 0;

			if (entityID > 500)
				finalEntityID = entityID;

			Entity entity = scene->GetEntityByID(finalEntityID);
			SE_CORE_ASSERT(entity);

			scene->RenderSelectedEntityOutline(entity, glm::vec4(colorX, colorY, colorZ, colorW));
		}

		void Scene_GetEntityComponent(uint64_t entityID, void* component)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByID(entityID);
			SE_CORE_ASSERT(entity);
		
			Coral::ManagedType* managedType = reinterpret_cast<Coral::ManagedType*>(component);
			SE_CORE_ASSERT(s_GetComponentFuncs.find(managedType) != s_GetComponentFuncs.end());
			s_GetComponentFuncs.at(managedType)(entity);
		}

#pragma endregion

#pragma region Entity

		static inline Entity GetEntity(uint64_t entityID)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_VERIFY(scene, "No active scene!");
			return scene->TryGetEntityWithID(entityID);
		};

		void Entity_CreateComponent(uint64_t entityID, Coral::ReflectionType componentType)
		{
			auto entity = GetEntity(entityID);
			SE_ICALL_VALIDATE_PARAM_V(entity, entityID);

			if (!entity)
				return;

			Coral::Type& type = componentType;

			if (!type)
				return;

			Coral::ScopedString typeName = type.GetFullName();

			if (!s_CreateComponentFuncs.contains(type.GetTypeId()))
			{
				//ErrorWithTrace("Cannot check if entity '{}' has a component of type '{}'. That component hasn't been registered with the engine.", entity.Name(), std::string(typeName));
				return;
			}

			if (s_HasComponentFuncs.at(type.GetTypeId())(entity))
			{
				//WarnWithTrace("Attempting to add duplicate component '{}' to entity '{}', ignoring.", std::string(typeName), entity.Name());
				return;
			}

			return s_CreateComponentFuncs.at(type.GetTypeId())(entity);
		}

		bool Entity_HasComponent(uint64_t entityID, Coral::ReflectionType componentType)
		{
			auto entity = GetEntity(entityID);
			SE_ICALL_VALIDATE_PARAM_V(entity, entityID);

			if (!entity)
				return false;

			Coral::Type& type = componentType;

			if (!type)
				return false;

			Coral::ScopedString typeName = type.GetFullName();

			if (!s_HasComponentFuncs.contains(type.GetTypeId()))
			{
				//ErrorWithTrace("Cannot check if entity '{}' has a component of type '{}'. That component hasn't been registered with the engine.", entity.Name(), std::string(typeName));
				return false;
			}

			return s_HasComponentFuncs.at(type.GetTypeId())(entity);
		}

		bool Entity_RemoveComponent(uint64_t entityID, Coral::ReflectionType componentType)
		{
			auto entity = GetEntity(entityID);
			SE_ICALL_VALIDATE_PARAM_V(entity, entityID);

			if (!entity)
				return false;

			Coral::Type& type = componentType;

			if (!type)
				return false;

			Coral::ScopedString typeName = type.GetFullName();

			if (!s_RemoveComponentFuncs.contains(type.GetTypeId()))
			{
				//ErrorWithTrace("Cannot remove a component of type '{}' from entity '{}'. That component hasn't been registered with the engine.", std::string(typeName), entity.Name());
				return false;
			}

			if (!s_HasComponentFuncs.at(type.GetTypeId())(entity))
			{
				//WarnWithTrace("Tried to remove component '{}' from entity '{}' even though it doesn't have that component.", std::string(typeName), entity.Name());
				return false;
			}

			s_RemoveComponentFuncs.at(type.GetTypeId())(entity);
			return true;
		}

		void Entity_DestroyEntity(uint64_t entityID)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByID(entityID);
			SE_CORE_ASSERT(entity);

			entity.DestroyEntity();
		}

		uint64_t Entity_FindEntityByName(Coral::String name)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene);

			std::string nameCStr = name;
			SE_CORE_WARN("Entity name: {}", nameCStr);

			Entity entity = scene->FindEntityByName(nameCStr);

			if (entity)
				return entity.GetEntityHandle();

			return 0;
		}

		uint64_t Entity_FindEntityByTag(Coral::String tag)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene);

			std::string nameCStr = tag;
			SE_CORE_WARN("Entity name: {}", nameCStr);

			Entity entity = scene->FindEntityByTag(nameCStr);

			if (entity)
				return entity.GetComponent<IDComponent>().ID;

			return 0;
		}

#pragma endregion

#pragma region Tag

		Coral::String TagComponent_GetTag(uint64_t entityID)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByID(entityID);
			SE_CORE_ASSERT(entity);

			auto& tagComponent = entity.GetComponent<TagComponent>();

			return Coral::String::New(tagComponent.Tag.c_str());
		}

		void TagComponent_SetTag(uint64_t entityID, Coral::String tag)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByID(entityID);
			SE_CORE_ASSERT(entity);

			auto& tagComponent = entity.GetComponent<TagComponent>();

			tagComponent.Tag = tag;
		}

#pragma endregion

#pragma region Transform

		bool TransformComponent_GetIsEnabled(uint64_t entityID)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByID(entityID);
			SE_CORE_ASSERT(entity);

			return entity.GetComponent<TransformComponent>().Enabled;
		}

		void TransformComponent_SetIsEnabled(uint64_t entityID, bool isEnabled)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByID(entityID);
			SE_CORE_ASSERT(entity);

			entity.GetComponent<TransformComponent>().Enabled = isEnabled;
		}

		void TransformComponent_GetTransform(uint64_t entityID, TransformComponent* outTransform)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByID(entityID);
			SE_CORE_ASSERT(entity);

			*outTransform = entity.GetComponent<TransformComponent>();
		}

		void TransformComponent_SetTransform(uint64_t entityID, TransformComponent* inTransform)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByID(entityID);
			SE_CORE_ASSERT(entity);

			entity.GetComponent<TransformComponent>() = *inTransform;
		}

		float TransformComponent_GetTranslationX(uint64_t entityID)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByID(entityID);
			SE_CORE_ASSERT(entity);

			return entity.GetComponent<TransformComponent>().Translation.x;
		}

		float TransformComponent_GetTranslationY(uint64_t entityID)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByID(entityID);
			SE_CORE_ASSERT(entity);

			return entity.GetComponent<TransformComponent>().Translation.y;
		}

		float TransformComponent_GetTranslationZ(uint64_t entityID)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByID(entityID);
			SE_CORE_ASSERT(entity);

			return entity.GetComponent<TransformComponent>().Translation.z;
		}

		void TransformComponent_SetTranslation(uint64_t entityID, float translationX, float translationY, float translationZ)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByID(entityID);
			SE_CORE_ASSERT(entity);

			entity.GetComponent<TransformComponent>().Translation = glm::vec4(translationX, translationY, translationZ, 1.0f);
		}

		float TransformComponent_GetRotationX(uint64_t entityID)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByID(entityID);
			SE_CORE_ASSERT(entity);

			return entity.GetComponent<TransformComponent>().Rotation.x;
		}

		float TransformComponent_GetRotationY(uint64_t entityID)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByID(entityID);
			SE_CORE_ASSERT(entity);

			return entity.GetComponent<TransformComponent>().Rotation.y;
		}

		float TransformComponent_GetRotationZ(uint64_t entityID)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByID(entityID);
			SE_CORE_ASSERT(entity);

			return entity.GetComponent<TransformComponent>().Rotation.z;
		}

		void TransformComponent_SetRotation(uint64_t entityID, float rotationX, float rotationY, float rotationZ)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByID(entityID);
			SE_CORE_ASSERT(entity);

			entity.GetComponent<TransformComponent>().Rotation = glm::vec3(rotationX, rotationY, rotationZ);
		}

		float TransformComponent_GetScaleX(uint64_t entityID)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByID(entityID);
			SE_CORE_ASSERT(entity);

			return entity.GetComponent<TransformComponent>().Scale.x;
		}

		float TransformComponent_GetScaleY(uint64_t entityID)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByID(entityID);
			SE_CORE_ASSERT(entity);

			return entity.GetComponent<TransformComponent>().Scale.y;
		}

		float TransformComponent_GetScaleZ(uint64_t entityID)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByID(entityID);
			SE_CORE_ASSERT(entity);

			return entity.GetComponent<TransformComponent>().Scale.z;
		}

		void TransformComponent_SetScale(uint64_t entityID, float scaleX, float scaleY, float scaleZ)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByID(entityID);
			SE_CORE_ASSERT(entity);

			entity.GetComponent<TransformComponent>().Scale = glm::vec3(scaleX, scaleY, scaleZ);
		}

#pragma endregion

#pragma region CameraComponent

		bool CameraComponent_GetIsPrimary(uint64_t entityID)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByID(entityID);
			SE_CORE_ASSERT(entity);

			return entity.GetComponent<CameraComponent>().Primary;
		}

		void CameraComponent_SetPrimary(uint64_t entityID, bool primary)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByID(entityID);
			SE_CORE_ASSERT(entity);

			entity.GetComponent<CameraComponent>().Primary = primary;
		}

		bool CameraComponent_GetFixedAspectRatio(uint64_t entityID)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByID(entityID);
			SE_CORE_ASSERT(entity);

			return entity.GetComponent<CameraComponent>().FixedAspectRatio;
		}

		void CameraComponent_SetFixedAspectRatio(uint64_t entityID, bool fixedAspectRatio)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByID(entityID);
			SE_CORE_ASSERT(entity);

			entity.GetComponent<CameraComponent>().FixedAspectRatio = fixedAspectRatio;
		}

#pragma endregion

#pragma region SpriteRenderer

		float SpriteRendererComponent_GetOffsetX(uint64_t entityID)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByID(entityID);
			SE_CORE_ASSERT(entity);

			return entity.GetComponent<SpriteRendererComponent>().SpriteTranslation.X;
		}

		float SpriteRendererComponent_GetOffsetY(uint64_t entityID)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByID(entityID);
			SE_CORE_ASSERT(entity);

			return entity.GetComponent<SpriteRendererComponent>().SpriteTranslation.Y;
		}

		float SpriteRendererComponent_GetOffsetZ(uint64_t entityID)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByID(entityID);
			SE_CORE_ASSERT(entity);

			return entity.GetComponent<SpriteRendererComponent>().SpriteTranslation.Z;
		}

		void SpriteRendererComponent_SetOffset(uint64_t entityID, float offsetX, float offsetY, float offsetZ)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByID(entityID);
			SE_CORE_ASSERT(entity);

			entity.GetComponent<SpriteRendererComponent>().SpriteTranslation = glm::vec4(offsetX, offsetY, offsetZ, 1.0f);
		}

		float SpriteRendererComponent_GetColorX(uint64_t entityID)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByID(entityID);
			SE_CORE_ASSERT(entity);

			return entity.GetComponent<SpriteRendererComponent>().Color.x;
		}

		float SpriteRendererComponent_GetColorY(uint64_t entityID)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByID(entityID);
			SE_CORE_ASSERT(entity);

			return entity.GetComponent<SpriteRendererComponent>().Color.y;
		}

		float SpriteRendererComponent_GetColorZ(uint64_t entityID)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByID(entityID);
			SE_CORE_ASSERT(entity);

			return entity.GetComponent<SpriteRendererComponent>().Color.x;
		}

		float SpriteRendererComponent_GetColorW(uint64_t entityID)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByID(entityID);
			SE_CORE_ASSERT(entity);

			return entity.GetComponent<SpriteRendererComponent>().Color.w;
		}

		void SpriteRendererComponent_SetColor(uint64_t entityID, float colorX, float colorY, float colorZ, float colorW)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByID(entityID);
			SE_CORE_ASSERT(entity);

			entity.GetComponent<SpriteRendererComponent>().Color = glm::vec4(colorX, colorY, colorZ, colorW);
		}

		float SpriteRendererComponent_GetUVX(uint64_t entityID)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByID(entityID);
			SE_CORE_ASSERT(entity);

			return entity.GetComponent<SpriteRendererComponent>().UV.X;
		}

		float SpriteRendererComponent_GetUVY(uint64_t entityID)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByID(entityID);
			SE_CORE_ASSERT(entity);

			return entity.GetComponent<SpriteRendererComponent>().UV.Y;
		}

		void SpriteRendererComponent_SetUV(uint64_t entityID, float uvX, float uvY)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByID(entityID);
			SE_CORE_ASSERT(entity);

			entity.GetComponent<SpriteRendererComponent>().UV = glm::vec2(uvX, uvY);
		}

		float SpriteRendererComponent_GetSaturation(uint64_t entityID)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByID(entityID);
			SE_CORE_ASSERT(entity);

			return entity.GetComponent<SpriteRendererComponent>().Saturation;
		}

		void SpriteRendererComponent_SetSaturation(uint64_t entityID, float saturation)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByID(entityID);
			SE_CORE_ASSERT(entity);

			entity.GetComponent<SpriteRendererComponent>().Saturation = saturation;
		}

		AssetHandle SpriteRendererComponent_GetTextureAssetHandle(uint64_t entityID)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByID(entityID);
			SE_CORE_ASSERT(entity);

			return entity.GetComponent<SpriteRendererComponent>().TextureHandle;
		}

		void SpriteRendererComponent_SetTextureAssetHandle(uint64_t entityID, AssetHandle textureHandle)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByID(entityID);
			SE_CORE_ASSERT(entity);

			entity.GetComponent<SpriteRendererComponent>().TextureHandle = textureHandle;
		}

		uint64_t SpriteRendererComponent_GetTextureAssetID(uint64_t entityID)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByID(entityID);
			SE_CORE_ASSERT(entity);

			return (uint64_t)entity.GetComponent<SpriteRendererComponent>().TextureHandle;
		}

		void SpriteRendererComponent_SetTextureAssetID(uint64_t entityID, uint64_t textureHandle)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByID(entityID);
			SE_CORE_ASSERT(entity);

			entity.GetComponent<SpriteRendererComponent>().TextureHandle = (AssetHandle)textureHandle;
		}

#pragma endregion

#pragma region CircleRenderer

		float CircleRendererComponent_GetColorX(uint64_t entityID)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByID(entityID);
			SE_CORE_ASSERT(entity);

			return entity.GetComponent<CircleRendererComponent>().Color.x;
		}

		float CircleRendererComponent_GetColorY(uint64_t entityID)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByID(entityID);
			SE_CORE_ASSERT(entity);

			return entity.GetComponent<CircleRendererComponent>().Color.y;
		}

		float CircleRendererComponent_GetColorZ(uint64_t entityID)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByID(entityID);
			SE_CORE_ASSERT(entity);

			return entity.GetComponent<CircleRendererComponent>().Color.z;
		}

		float CircleRendererComponent_GetColorW(uint64_t entityID)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByID(entityID);
			SE_CORE_ASSERT(entity);

			return entity.GetComponent<CircleRendererComponent>().Color.w;
		}

		void CircleRendererComponent_SetColor(uint64_t entityID, float colorX, float colorY, float colorZ, float colorW)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByID(entityID);
			SE_CORE_ASSERT(entity);

			entity.GetComponent<CircleRendererComponent>().Color = glm::vec4(colorX, colorY, colorZ, colorW);
		}

		float CircleRendererComponent_GetUVX(uint64_t entityID)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByID(entityID);
			SE_CORE_ASSERT(entity);

			return entity.GetComponent<CircleRendererComponent>().UV.X;
		}

		float CircleRendererComponent_GetUVY(uint64_t entityID)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByID(entityID);
			SE_CORE_ASSERT(entity);

			return entity.GetComponent<CircleRendererComponent>().UV.Y;
		}

		void CircleRendererComponent_SetUV(uint64_t entityID, float uvX, float uvY)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByID(entityID);
			SE_CORE_ASSERT(entity);

			entity.GetComponent<CircleRendererComponent>().UV = glm::vec2(uvX, uvY);
		}

#pragma endregion

#pragma region LineRendererComponent

		float LineRendererComponent_GetLineThickness(uint64_t entityID)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByID(entityID);
			SE_CORE_ASSERT(entity);

			return entity.GetComponent<LineRendererComponent>().LineThickness;
		}

		void LineRendererComponent_SetLineThickness(uint64_t entityID, float lineThickness)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByID(entityID);
			SE_CORE_ASSERT(entity);

			entity.GetComponent<LineRendererComponent>().LineThickness = lineThickness;
		}

#pragma endregion

#pragma region TextComponent

		Coral::String TextComponent_GetText(uint64_t entityID)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByID(entityID);
			SE_CORE_ASSERT(entity);

			return Coral::String::New(entity.GetComponent<TextComponent>().TextString.c_str());
		}

		void TextComponent_SetText(uint64_t entityID, Coral::String textString)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByID(entityID);
			SE_CORE_ASSERT(entity);

			entity.GetComponent<TextComponent>().TextString = textString;
		}

		float TextComponent_GetColorX(uint64_t entityID)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByID(entityID);
			SE_CORE_ASSERT(entity);
			SE_CORE_ASSERT(entity.HasComponent<TextComponent>());

			auto& tc = entity.GetComponent<TextComponent>();
			return tc.Color.x;
		}

		float TextComponent_GetColorY(uint64_t entityID)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByID(entityID);
			SE_CORE_ASSERT(entity);
			SE_CORE_ASSERT(entity.HasComponent<TextComponent>());

			auto& tc = entity.GetComponent<TextComponent>();
			return tc.Color.y;
		}

		float TextComponent_GetColorZ(uint64_t entityID)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByID(entityID);
			SE_CORE_ASSERT(entity);
			SE_CORE_ASSERT(entity.HasComponent<TextComponent>());

			auto& tc = entity.GetComponent<TextComponent>();
			return tc.Color.z;
		}

		float TextComponent_GetColorW(uint64_t entityID)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByID(entityID);
			SE_CORE_ASSERT(entity);
			SE_CORE_ASSERT(entity.HasComponent<TextComponent>());

			auto& tc = entity.GetComponent<TextComponent>();
			return tc.Color.w;
		}

		void TextComponent_SetColor(uint64_t entityID, float colorX, float colorY, float colorZ, float colorW)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByID(entityID);
			SE_CORE_ASSERT(entity);
			SE_CORE_ASSERT(entity.HasComponent<TextComponent>());

			auto& tc = entity.GetComponent<TextComponent>();
			tc.Color = glm::vec4(colorX, colorY, colorZ, colorW);
		}

		float TextComponent_GetKerning(uint64_t entityID)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByID(entityID);
			SE_CORE_ASSERT(entity);
			SE_CORE_ASSERT(entity.HasComponent<TextComponent>());

			auto& tc = entity.GetComponent<TextComponent>();
			return tc.Kerning;
		}

		void TextComponent_SetKerning(uint64_t entityID, float kerning)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByID(entityID);
			SE_CORE_ASSERT(entity);
			SE_CORE_ASSERT(entity.HasComponent<TextComponent>());

			auto& tc = entity.GetComponent<TextComponent>();
			tc.Kerning = kerning;
		}

		float TextComponent_GetLineSpacing(uint64_t entityID)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByID(entityID);
			SE_CORE_ASSERT(entity);
			SE_CORE_ASSERT(entity.HasComponent<TextComponent>());

			auto& tc = entity.GetComponent<TextComponent>();
			return tc.LineSpacing;
		}

		void TextComponent_SetLineSpacing(uint64_t entityID, float lineSpacing)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByID(entityID);
			SE_CORE_ASSERT(entity);
			SE_CORE_ASSERT(entity.HasComponent<TextComponent>());

			auto& tc = entity.GetComponent<TextComponent>();
			tc.LineSpacing = lineSpacing;
		}

#pragma endregion

#pragma region RigidBody2D

		void RigidBody2DComponent_ApplyLinearImpulse(uint64_t entityID, float impulseX, float impulseY, float offsetX, float offsetY, bool wake)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByID(entityID);
			SE_CORE_ASSERT(entity);

			auto& component = entity.GetComponent<RigidBody2DComponent>();
			b2Body* body = (b2Body*)component.RuntimeBody;
			body->ApplyLinearImpulse(b2Vec2(impulseX, impulseY), body->GetWorldCenter() + b2Vec2(offsetX, offsetY), wake);
		}

		void RigidBody2DComponent_ApplyLinearImpulseToCenter(uint64_t entityID, float impulseX, float impulseY, bool wake)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByID(entityID);
			SE_CORE_ASSERT(entity);

			auto& rb2d = entity.GetComponent<RigidBody2DComponent>();
			b2Body* body = (b2Body*)rb2d.RuntimeBody;
			body->ApplyLinearImpulseToCenter(b2Vec2(impulseX, impulseY), wake);
		}

		float RigidBody2DComponent_GetLinearVelocityX(uint64_t entityID)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByID(entityID);
			SE_CORE_ASSERT(entity);

			auto& rb2d = entity.GetComponent<RigidBody2DComponent>();
			b2Body* body = (b2Body*)rb2d.RuntimeBody;
			const b2Vec2& linearVelocity = body->GetLinearVelocity();
			return linearVelocity.x;
		}

		float RigidBody2DComponent_GetLinearVelocityY(uint64_t entityID)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByID(entityID);
			SE_CORE_ASSERT(entity);

			auto& rb2d = entity.GetComponent<RigidBody2DComponent>();
			b2Body* body = (b2Body*)rb2d.RuntimeBody;
			const b2Vec2& linearVelocity = body->GetLinearVelocity();
			return linearVelocity.y;
		}

		void RigidBody2DComponent_SetLinearVelocity(uint64_t entityID, float velocityX, float velocityY)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByID(entityID);
			SE_CORE_ASSERT(entity);

			auto& component = entity.GetComponent<RigidBody2DComponent>();
			b2Body* body = (b2Body*)component.RuntimeBody;
			body->SetLinearVelocity({ velocityX, velocityY });
		}

		RigidBody2DComponent::BodyType RigidBody2DComponent_GetType(uint64_t entityID)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByID(entityID);
			SE_CORE_ASSERT(entity);

			auto& rb2d = entity.GetComponent<RigidBody2DComponent>();
			b2Body* body = (b2Body*)rb2d.RuntimeBody;
			return Utils::RigidBody2DTypeFromBox2DBody(body->GetType());
		}

		void RigidBody2DComponent_SetType(uint64_t entityID, RigidBody2DComponent::BodyType bodyType)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByID(entityID);
			SE_CORE_ASSERT(entity);

			auto& rb2d = entity.GetComponent<RigidBody2DComponent>();
			b2Body* body = (b2Body*)rb2d.RuntimeBody;
			body->SetType(Utils::RigidBody2DTypeToBox2DBody(bodyType));
		}

		float Rigidbody2DComponent_GetGravityX(uint64_t entityID)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByID(entityID);
			SE_CORE_ASSERT(entity);

			const auto& gravity = scene->GetPhysics2DGravity();
			return gravity.X;
		}

		float Rigidbody2DComponent_GetGravityY(uint64_t entityID)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByID(entityID);
			SE_CORE_ASSERT(entity);

			const auto& gravity = scene->GetPhysics2DGravity();
			return gravity.Y;
		}

		void Rigidbody2DComponent_SetGravity(uint64_t entityID, float gravityX, float gravityY)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByID(entityID);
			SE_CORE_ASSERT(entity);

			scene->SetPhysics2DGravity({ gravityX, gravityY });
		}

		bool Rigidbody2DComponent_GetEnabled(uint64_t entityID)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByID(entityID);
			SE_CORE_ASSERT(entity);

			return entity.GetComponent<RigidBody2DComponent>().SetEnabled;
		}

		void Rigidbody2DComponent_SetEnabled(uint64_t entityID, bool setEnabled)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByID(entityID);
			SE_CORE_ASSERT(entity);

			entity.GetComponent<RigidBody2DComponent>().SetEnabled = setEnabled;
		}

#pragma endregion

#pragma region BoxCollider2D

		float BoxCollider2DComponent_GetOffsetX(uint64_t entityID)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByID(entityID);
			SE_CORE_ASSERT(entity);

			if (entity.HasComponent<BoxCollider2DComponent>())
				return entity.GetComponent<BoxCollider2DComponent>().Offset.x;

			return 0.0f;
		}

		float BoxCollider2DComponent_GetOffsetY(uint64_t entityID)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByID(entityID);
			SE_CORE_ASSERT(entity);

			if (entity.HasComponent<BoxCollider2DComponent>())
				return entity.GetComponent<BoxCollider2DComponent>().Offset.y;

			return 0.0f;
		}

		void BoxCollider2DComponent_SetOffset(uint64_t entityID, float offsetX, float offsetY)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByID(entityID);
			SE_CORE_ASSERT(entity);

			if (entity.HasComponent<BoxCollider2DComponent>())
				entity.GetComponent<BoxCollider2DComponent>().Offset = glm::vec2(offsetX, offsetY);
		}

		float BoxCollider2DComponent_GetSizeX(uint64_t entityID)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByID(entityID);
			SE_CORE_ASSERT(entity);

			if (entity.HasComponent<BoxCollider2DComponent>())
				return entity.GetComponent<BoxCollider2DComponent>().Size.x;

			return 0.0f;
		}

		float BoxCollider2DComponent_GetSizeY(uint64_t entityID)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByID(entityID);
			SE_CORE_ASSERT(entity);

			if (entity.HasComponent<BoxCollider2DComponent>())
				return entity.GetComponent<BoxCollider2DComponent>().Size.y;

			return 0.0f;
		}

		void BoxCollider2DComponent_SetSize(uint64_t entityID, float sizeX, float sizeY)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByID(entityID);
			SE_CORE_ASSERT(entity);

			if (entity.HasComponent<BoxCollider2DComponent>())
				entity.GetComponent<BoxCollider2DComponent>().Size = glm::vec2(sizeX, sizeY);
		}

		float BoxCollider2DComponent_GetDensity(uint64_t entityID)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByID(entityID);
			SE_CORE_ASSERT(entity);

			if (entity.HasComponent<BoxCollider2DComponent>())
				return entity.GetComponent<BoxCollider2DComponent>().Density;

			return 0.0f;
		}

		void BoxCollider2DComponent_SetDensity(uint64_t entityID, float density)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByID(entityID);
			SE_CORE_ASSERT(entity);

			if (entity.HasComponent<BoxCollider2DComponent>())
				entity.GetComponent<BoxCollider2DComponent>().Density = density;
		}

		float BoxCollider2DComponent_GetFriction(uint64_t entityID)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByID(entityID);
			SE_CORE_ASSERT(entity);

			if (entity.HasComponent<BoxCollider2DComponent>())
				return entity.GetComponent<BoxCollider2DComponent>().Friction;

			return 0.0f;
		}

		void BoxCollider2DComponent_SetFriction(uint64_t entityID, float friction)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByID(entityID);
			SE_CORE_ASSERT(entity);

			if (entity.HasComponent<BoxCollider2DComponent>())
				entity.GetComponent<BoxCollider2DComponent>().Friction = friction;
		}

		float BoxCollider2DComponent_GetRestitution(uint64_t entityID)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByID(entityID);
			SE_CORE_ASSERT(entity);

			if (entity.HasComponent<BoxCollider2DComponent>())
				return entity.GetComponent<BoxCollider2DComponent>().Restitution;

			return 0.0f;
		}

		void BoxCollider2DComponent_SetRestitution(uint64_t entityID, float restitution)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByID(entityID);
			SE_CORE_ASSERT(entity);

			if (entity.HasComponent<BoxCollider2DComponent>())
				entity.GetComponent<BoxCollider2DComponent>().Restitution = restitution;
		}

		float BoxCollider2DComponent_GetRestitutionThreshold(uint64_t entityID)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByID(entityID);
			SE_CORE_ASSERT(entity);

			if (entity.HasComponent<BoxCollider2DComponent>())
				return entity.GetComponent<BoxCollider2DComponent>().RestitutionThreshold;

			return 0.0f;
		}

		void BoxCollider2DComponent_SetRestitutionThreshold(uint64_t entityID, float restitutionThreshold)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByID(entityID);
			SE_CORE_ASSERT(entity);

			if (entity.HasComponent<BoxCollider2DComponent>())
				entity.GetComponent<BoxCollider2DComponent>().RestitutionThreshold = restitutionThreshold;
		}

		float BoxCollider2DComponent_GetCollisionRayX(uint64_t entityID)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByID(entityID);
			SE_CORE_ASSERT(entity);

			if (entity.HasComponent<BoxCollider2DComponent>())
				return entity.GetComponent<BoxCollider2DComponent>().CollisionRay.X;

			return 0.0f;
		}

		float BoxCollider2DComponent_GetCollisionRayY(uint64_t entityID)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByID(entityID);
			SE_CORE_ASSERT(entity);

			if (entity.HasComponent<BoxCollider2DComponent>())
				return entity.GetComponent<BoxCollider2DComponent>().CollisionRay.Y;

			return 0.0f;
		}

		void BoxCollider2DComponent_SetCollisionRay(uint64_t entityID, float rayX, float rayY)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByID(entityID);
			SE_CORE_ASSERT(entity);

			if (entity.HasComponent<BoxCollider2DComponent>())
				entity.GetComponent<BoxCollider2DComponent>().CollisionRay = rtmcpp::Vec2(rayX, rayY);
		}

		bool BoxCollider2DComponent_GetAwake(uint64_t entityID)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByID(entityID);
			SE_CORE_ASSERT(entity);

			if (entity.HasComponent<BoxCollider2DComponent>())
				return entity.GetComponent<BoxCollider2DComponent>().Awake;

			return false;
		}

		void BoxCollider2DComponent_SetAwake(uint64_t entityID, bool setAwake)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByID(entityID);
			SE_CORE_ASSERT(entity);

			if (entity.HasComponent<BoxCollider2DComponent>())
				entity.GetComponent<BoxCollider2DComponent>().Awake = setAwake;
		}

#pragma endregion

#pragma region CircleCollider2D

		float CircleCollider2DComponent_GetOffsetX(uint64_t entityID)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByID(entityID);
			SE_CORE_ASSERT(entity);

			if (entity.HasComponent<CircleCollider2DComponent>())
				return entity.GetComponent<CircleCollider2DComponent>().Offset.X;

			return 0.0f;
		}

		float CircleCollider2DComponent_GetOffsetY(uint64_t entityID)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByID(entityID);
			SE_CORE_ASSERT(entity);

			if (entity.HasComponent<CircleCollider2DComponent>())
				return entity.GetComponent<CircleCollider2DComponent>().Offset.Y;

			return 0.0f;
		}

		void CircleCollider2DComponent_SetOffset(uint64_t entityID, float offsetX, float offsetY)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByID(entityID);
			SE_CORE_ASSERT(entity);

			if (entity.HasComponent<CircleCollider2DComponent>())
				entity.GetComponent<CircleCollider2DComponent>().Offset = rtmcpp::Vec2(offsetX, offsetY);
		}

		float CircleCollider2DComponent_GetRadius(uint64_t entityID)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByID(entityID);
			SE_CORE_ASSERT(entity);

			if (entity.HasComponent<CircleCollider2DComponent>())
				return entity.GetComponent<CircleCollider2DComponent>().Radius;

			return 0.0f;
		}

		void CircleCollider2DComponent_SetRadius(uint64_t entityID, float radius)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByID(entityID);
			SE_CORE_ASSERT(entity);

			if (entity.HasComponent<CircleCollider2DComponent>())
				entity.GetComponent<CircleCollider2DComponent>().Radius = radius;
		}

		float CircleCollider2DComponent_GetDensity(uint64_t entityID)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByID(entityID);
			SE_CORE_ASSERT(entity);

			if (entity.HasComponent<CircleCollider2DComponent>())
				return entity.GetComponent<CircleCollider2DComponent>().Density;

			return 0.0f;
		}

		void CircleCollider2DComponent_SetDensity(uint64_t entityID, float density)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByID(entityID);
			SE_CORE_ASSERT(entity);

			if (entity.HasComponent<CircleCollider2DComponent>())
				entity.GetComponent<CircleCollider2DComponent>().Density = density;
		}

		float CircleCollider2DComponent_GetFriction(uint64_t entityID)
		{
			RefPtr<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByID(entityID);
			SE_CORE_ASSERT(entity);

			if (entity.HasComponent<CircleCollider2DComponent>())
				return entity.GetComponent<CircleCollider2DComponent>().Friction;

			return 0.0f;
		}

		void CircleCollider2DComponent_SetFriction(uint64_t entityID, float friction)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByID(entityID);
			SE_CORE_ASSERT(entity);

			if (entity.HasComponent<CircleCollider2DComponent>())
				entity.GetComponent<CircleCollider2DComponent>().Friction = friction;
		}

		float CircleCollider2DComponent_GetRestitution(uint64_t entityID)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByID(entityID);
			SE_CORE_ASSERT(entity);

			if (entity.HasComponent<CircleCollider2DComponent>())
				return entity.GetComponent<CircleCollider2DComponent>().Restitution;

			return 0.0f;
		}

		void CircleCollider2DComponent_SetRestitution(uint64_t entityID, float restitution)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByID(entityID);
			SE_CORE_ASSERT(entity);

			if (entity.HasComponent<CircleCollider2DComponent>())
				entity.GetComponent<CircleCollider2DComponent>().Restitution = restitution;
		}

		float CircleCollider2DComponent_GetRestitutionThreshold(uint64_t entityID)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByID(entityID);
			SE_CORE_ASSERT(entity);

			if (entity.HasComponent<CircleCollider2DComponent>())
				return entity.GetComponent<CircleCollider2DComponent>().RestitutionThreshold;

			return 0.0f;
		}

		void CircleCollider2DComponent_SetRestitutionThreshold(uint64_t entityID, float restitutionThreshold)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByID(entityID);
			SE_CORE_ASSERT(entity);

			if (entity.HasComponent<CircleCollider2DComponent>())
				entity.GetComponent<CircleCollider2DComponent>().RestitutionThreshold = restitutionThreshold;
		}

		float CircleCollider2DComponent_GetCollisionRayX(uint64_t entityID)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByID(entityID);
			SE_CORE_ASSERT(entity);

			if (entity.HasComponent<CircleCollider2DComponent>())
				return entity.GetComponent<CircleCollider2DComponent>().CollisionRay.X;

			return 0.0f;
		}

		float CircleCollider2DComponent_GetCollisionRayY(uint64_t entityID)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByID(entityID);
			SE_CORE_ASSERT(entity);

			if (entity.HasComponent<CircleCollider2DComponent>())
				return entity.GetComponent<CircleCollider2DComponent>().CollisionRay.Y;

			return 0.0f;
		}

		void CircleCollider2DComponent_SetCollisionRay(uint64_t entityID, float rayX, float rayY)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByID(entityID);
			SE_CORE_ASSERT(entity);

			if (entity.HasComponent<CircleCollider2DComponent>())
				entity.GetComponent<CircleCollider2DComponent>().CollisionRay = rtmcpp::Vec2(rayX, rayY);
		}

		bool CircleCollider2DComponent_GetAwake(uint64_t entityID)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByID(entityID);
			SE_CORE_ASSERT(entity);

			if (entity.HasComponent<CircleCollider2DComponent>())
				return entity.GetComponent<CircleCollider2DComponent>().Awake;

			return false;
		}

		void CircleCollider2DComponent_SetAwake(uint64_t entityID, bool setAwake)
		{
			Ref<Scene> scene = ScriptEngine::GetInstance().GetCurrentScene();
			SE_CORE_ASSERT(scene);
			Entity entity = scene->GetEntityByID(entityID);
			SE_CORE_ASSERT(entity);

			if (entity.HasComponent<CircleCollider2DComponent>())
				entity.GetComponent<CircleCollider2DComponent>().Awake = setAwake;
		}

#pragma endregion
	}

}
