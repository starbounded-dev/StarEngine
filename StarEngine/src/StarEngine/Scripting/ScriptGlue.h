#pragma once

#include "StarEngine/Asset/Asset.h"
#include "StarEngine/Scene/Components.h"
#include "StarEngine/Core/Input.h"

#include <Coral/Assembly.hpp>
#include <Coral/String.hpp>
#include <Coral/Array.hpp>

#include <glm/glm.hpp>

/**
 * Registers scripting glue code and manages entity selection and cursor state for scripting integration.
 *
 * The ScriptGlue class provides static methods to register scripting glue with a managed assembly, as well as to get and set the currently hovered and selected entities within the engine. It also maintains static variables for cursor state and position, enabling scripting environments to interact with engine internals.
 */
namespace StarEngine {

	// Forward declarations
	class Entity;

	class ScriptGlue
	{
	public:
		static void RegisterGlue(Coral::ManagedAssembly& coreAssembly);

		static Entity GetHoveredEntity();
		static void SetHoveredEntity(Entity entity);

		static Entity GetSelectedEntity();
		static void SetSelectedEntity(Entity entity);

	private:
		static void RegisterComponentTypes(Coral::ManagedAssembly& coreAssembly);
		static void RegisterInternalCalls(Coral::ManagedAssembly& coreAssembly);

	public:
		static bool s_CalledSetCursor;
		static bool s_ChangedCursor;
		static glm::vec2 s_CursorHotSpot;

		static std::string s_SetCursorPath;

		inline static bool s_IsCursorInViewport = false;
	};

	namespace InternalCalls
	{
		template<typename T>
		struct Param
		{
			std::byte Data[sizeof(T)];

			operator T() const
			{
				T result;
				std::memcpy(&result, Data, sizeof(T));
				return result;
			}
		};

#pragma region AssetHandle

		bool AssetHandle_IsValid(AssetHandle assetHandle);

#pragma endregion

#pragma region Input

		bool Input_IsKeyDown(KeyCode keycode);
		//bool Input_IsKeyUp(KeyCode keycode);
		//bool Input_IsMouseButtonPressed(MouseCode button);
		//bool Input_PressMouseButton(MouseCode button);
		//bool Input_ReleaseMouseButton(MouseCode button);
		//float Input_GetMousePositionX();
		//float Input_GetMousePositionY();
		//float Input_GetMouseWorldPositionX();
		//float Input_GetMouseWorldPositionY();

#pragma endregion

		/*
#pragma region Application

		float Application_GetFPS();
		float Application_GetFrameTime();
		float Application_GetMinFrameTime();
		float Application_GetMaxFrameTime();

#pragma endregion
		*/
		/*
#pragma region Scene

		void Scene_LoadScene(AssetHandle assetHandle);
		Coral::String Scene_GetCursor();
		void Scene_SetCursor(Coral::String filepath);
		float Scene_GetMouseHotSpotX();
		float Scene_GetMouseHotSpotY();
		void Scene_SetMouseHotSpot(float hotSpotX, float hotSpotY);
		void Scene_ChangeCursor(Coral::String filepath, float hotSpotX, float hotspotY);
		//static void Scene_CloseApplication();
		Coral::String Scene_GetCurrentFilename();
		Coral::String Scene_GetName();
		void Scene_SetName(Coral::String path);
		bool Scene_IsGamePaused();
		void Scene_SetPauseGame(bool shouldPause);
		uint64_t Scene_CreateEntity(Coral::String tag);
		bool Scene_IsEntityValid(uint64_t entityID);
		uint64_t Scene_GetHoveredEntity();
		uint64_t Scene_GetSelectedEntity();
		void Scene_SetSelectedEntity(uint64_t entityID);
		void Scene_RenderHoveredEntityOutline(uint64_t entityID, float colorX, float colorY, float colorZ, float colorW);
		void Scene_RenderSelectedEntityOutline(uint64_t entityID, float colorX, float colorY, float colorZ, float colorW);
		//void Scene_GetEntityComponent(uint64_t entityID, void* component);

#pragma endregion
		*/
#pragma region Entity

		//void Entity_CreateComponent(uint64_t entityID, Coral::ReflectionType componentType);
		bool Entity_HasComponent(uint64_t entityID, Coral::ReflectionType componentType);
		//bool Entity_RemoveComponent(uint64_t entityID, Coral::ReflectionType componentType);
		//void Entity_DestroyEntity(uint64_t entityID);
		uint64_t Entity_FindEntityByName(Coral::String name);
		uint64_t Entity_FindEntityByTag(Coral::String tag);

#pragma endregion
		/*
#pragma region TagComponent

		Coral::String TagComponent_GetTag(uint64_t entityID);
		void TagComponent_SetTag(uint64_t entityID, Coral::String tag);

#pragma endregion
	*/
#pragma region TransformComponent

		//bool TransformComponent_GetIsEnabled(uint64_t entityID);
		//void TransformComponent_SetIsEnabled(uint64_t entityID, bool isEnabled);
		void TransformComponent_GetTransform(uint64_t entityID, TransformComponent* outTransform);
		void TransformComponent_SetTransform(uint64_t entityID, TransformComponent* inTransform);
		//float TransformComponent_GetTranslationX(uint64_t entityID);
		//float TransformComponent_GetTranslationY(uint64_t entityID);
		//float TransformComponent_GetTranslationZ(uint64_t entityID);
		//void TransformComponent_SetTranslation(uint64_t entityID, float translationX, float translationY, float translationZ);
		//float TransformComponent_GetRotationX(uint64_t entityID);
		//float TransformComponent_GetRotationY(uint64_t entityID);
		//float TransformComponent_GetRotationZ(uint64_t entityID);
		//void TransformComponent_SetRotation(uint64_t entityID, float rotationX, float rotationY, float rotationZ);
		//float TransformComponent_GetScaleX(uint64_t entityID);
		//float TransformComponent_GetScaleY(uint64_t entityID);
		//float TransformComponent_GetScaleZ(uint64_t entityID);
		//void TransformComponent_SetScale(uint64_t entityID, float scaleX, float scaleY, float scaleZ);

#pragma endregion
		/*
#pragma region CameraComponent

		bool CameraComponent_GetIsPrimary(uint64_t entityID);
		void CameraComponent_SetPrimary(uint64_t entityID, bool primary);
		bool CameraComponent_GetFixedAspectRatio(uint64_t entityID);
		void CameraComponent_SetFixedAspectRatio(uint64_t entityID, bool fixedAspectRatio);

#pragma endregion
		*/
		/*
#pragma region SpriteRendererComponent

		float SpriteRendererComponent_GetOffsetX(uint64_t entityID);
		float SpriteRendererComponent_GetOffsetY(uint64_t entityID);
		float SpriteRendererComponent_GetOffsetZ(uint64_t entityID);
		void SpriteRendererComponent_SetOffset(uint64_t entityID, float offsetX, float offsetY, float offsetZ);
		float SpriteRendererComponent_GetColorX(uint64_t entityID);
		float SpriteRendererComponent_GetColorY(uint64_t entityID);
		float SpriteRendererComponent_GetColorZ(uint64_t entityID);
		float SpriteRendererComponent_GetColorW(uint64_t entityID);
		void SpriteRendererComponent_SetColor(uint64_t entityID, float colorX, float colorY, float colorZ, float colorW);
		float SpriteRendererComponent_GetUVX(uint64_t entityID);
		float SpriteRendererComponent_GetUVY(uint64_t entityID);
		void SpriteRendererComponent_SetUV(uint64_t entityID, float uvX, float uvY);
		bool SpriteRendererComponent_GetUseParallax(uint64_t entityID);
		void SpriteRendererComponent_SetUseParallax(uint64_t entityID, bool useParallax);
		float SpriteRendererComponent_GetParallaxSpeedX(uint64_t entityID);
		float SpriteRendererComponent_GetParallaxSpeedY(uint64_t entityID);
		void SpriteRendererComponent_SetParallaxSpeed(uint64_t entityID, float speedX, float speedY);
		float SpriteRendererComponent_GetParallaxDivision(uint64_t entityID);
		void SpriteRendererComponent_SetParallaxDivision(uint64_t entityID, float division);
		bool SpriteRendererComponent_GetUseTextureAtlasAnimation(uint64_t entityID);
		void SpriteRendererComponent_SetUseTextureAtlasAnimation(uint64_t entityID, bool useAnimation);
		float SpriteRendererComponent_GetAnimationSpeed(uint64_t entityID);
		void SpriteRendererComponent_SetAnimationSpeed(uint64_t entityID, float animSpeed);
		int SpriteRendererComponent_GetNumTiles(uint64_t entityID);
		void SpriteRendererComponent_SetNumTiles(uint64_t entityID, int numTiles);
		int SpriteRendererComponent_GetStartIndexX(uint64_t entityID);
		void SpriteRendererComponent_SetStartIndexX(uint64_t entityID, int startIndex);
		int SpriteRendererComponent_GetStartIndexY(uint64_t entityID);
		void SpriteRendererComponent_SetStartIndexY(uint64_t entityID, int startIndex);
		int SpriteRendererComponent_GetColumn(uint64_t entityID);
		void SpriteRendererComponent_SetColumn(uint64_t entityID, int column);
		int SpriteRendererComponent_GetRow(uint64_t entityID);
		void SpriteRendererComponent_SetRow(uint64_t entityID, int row);
		float SpriteRendererComponent_GetSaturation(uint64_t entityID);
		void SpriteRendererComponent_SetSaturation(uint64_t entityID, float saturation);
		AssetHandle SpriteRendererComponent_GetTextureAssetHandle(uint64_t entityID);
		void SpriteRendererComponent_SetTextureAssetHandle(uint64_t entityID, AssetHandle textureHandle);
		uint64_t SpriteRendererComponent_GetTextureAssetID(uint64_t entityID);
		void SpriteRendererComponent_SetTextureAssetID(uint64_t entityID, uint64_t textureHandle);

#pragma endregion
		*/
		/*
#pragma region CircleRendererComponent

		float CircleRendererComponent_GetColorX(uint64_t entityID);
		float CircleRendererComponent_GetColorY(uint64_t entityID);
		float CircleRendererComponent_GetColorZ(uint64_t entityID);
		float CircleRendererComponent_GetColorW(uint64_t entityID);
		void CircleRendererComponent_SetColor(uint64_t entityID, float colorX, float colorY, float colorZ, float colorW);
		float CircleRendererComponent_GetUVX(uint64_t entityID);
		float CircleRendererComponent_GetUVY(uint64_t entityID);
		void CircleRendererComponent_SetUV(uint64_t entityID, float uvX, float uvY);
		bool CircleRendererComponent_GetUseParallax(uint64_t entityID);
		void CircleRendererComponent_SetUseParallax(uint64_t entityID, bool useParallax);
		float CircleRendererComponent_GetParallaxSpeedX(uint64_t entityID);
		float CircleRendererComponent_GetParallaxSpeedY(uint64_t entityID);
		void CircleRendererComponent_SetParallaxSpeed(uint64_t entityID, float speedX, float speedY);
		float CircleRendererComponent_GetParallaxDivision(uint64_t entityID);
		void CircleRendererComponent_SetParallaxDivision(uint64_t entityID, float division);
		bool CircleRendererComponent_GetUseTextureAtlasAnimation(uint64_t entityID);
		void CircleRendererComponent_SetUseTextureAtlasAnimation(uint64_t entityID, bool useAnimation);
		float CircleRendererComponent_GetAnimationSpeed(uint64_t entityID);
		void CircleRendererComponent_SetAnimationSpeed(uint64_t entityID, float animSpeed);
		int CircleRendererComponent_GetNumTiles(uint64_t entityID);
		void CircleRendererComponent_SetNumTiles(uint64_t entityID, int numTiles);
		int CircleRendererComponent_GetStartIndexX(uint64_t entityID);
		void CircleRendererComponent_SetStartIndexX(uint64_t entityID, int startIndex);
		int CircleRendererComponent_GetStartIndexY(uint64_t entityID);
		void CircleRendererComponent_SetStartIndexY(uint64_t entityID, int startIndex);
		int CircleRendererComponent_GetColumn(uint64_t entityID);
		void CircleRendererComponent_SetColumn(uint64_t entityID, int column);
		int CircleRendererComponent_GetRow(uint64_t entityID);
		void CircleRendererComponent_SetRow(uint64_t entityID, int row);

#pragma endregion
		*/
		/*
#pragma region LineRendererComponent

		float LineRendererComponent_GetLineThickness(uint64_t entityID);
		void LineRendererComponent_SetLineThickness(uint64_t entityID, float lineThickness);

#pragma endregion
		*/
#pragma region TextComponent

		Coral::String TextComponent_GetText(uint64_t entityID);
		void TextComponent_SetText(uint64_t entityID, Coral::String textString);
		float TextComponent_GetColorX(uint64_t entityID);
		float TextComponent_GetColorY(uint64_t entityID);
		float TextComponent_GetColorZ(uint64_t entityID);
		float TextComponent_GetColorW(uint64_t entityID);
		void TextComponent_SetColor(uint64_t entityID, float colorX, float colorY, float colorZ, float colorW);
		float TextComponent_GetKerning(uint64_t entityID);
		void TextComponent_SetKerning(uint64_t entityID, float kerning);
		float TextComponent_GetLineSpacing(uint64_t entityID);
		void TextComponent_SetLineSpacing(uint64_t entityID, float lineSpacing);

#pragma endregion

#pragma region RigidBody2DComponent

		void RigidBody2DComponent_ApplyLinearImpulse(uint64_t entityID, float impulseX, float impulseY, float offsetX, float offsetY, bool wake);
		void RigidBody2DComponent_ApplyLinearImpulseToCenter(uint64_t entityID, float impulseX, float impulseY, bool wake);
		float RigidBody2DComponent_GetLinearVelocityX(uint64_t entityID);
		float RigidBody2DComponent_GetLinearVelocityY(uint64_t entityID);
		//void RigidBody2DComponent_SetLinearVelocity(uint64_t entityID, float velocityX, float velocityY);
		RigidBody2DComponent::BodyType Rigidbody2DComponent_GetType(uint64_t entityID);
		void RigidBody2DComponent_SetType(uint64_t entityID, RigidBody2DComponent::BodyType bodyType);
		//float RigidBody2DComponent_GetGravityX(uint64_t entityID);
		//float RigidBody2DComponent_GetGravityY(uint64_t entityID);
		//void RigidBody2DComponent_SetGravity(uint64_t entityID, float gravityX, float gravityY);
		//bool RigidBody2DComponent_GetEnabled(uint64_t entityID);
		//void RigidBody2DComponent_SetEnabled(uint64_t entityID, bool setEnabled);

#pragma endregion

		/*
#pragma region BoxCollider2DComponent

		void BoxCollider2DComponent_GetOffset(uint64_t entityID, glm::vec2* outOffset);
		float BoxCollider2DComponent_GetOffsetX(uint64_t entityID);
		float BoxCollider2DComponent_GetOffsetY(uint64_t entityID);
		void BoxCollider2DComponent_SetOffset(uint64_t entityID, float offsetX, float offsetY);
		float BoxCollider2DComponent_GetSizeX(uint64_t entityID);
		float BoxCollider2DComponent_GetSizeY(uint64_t entityID);
		void BoxCollider2DComponent_SetSize(uint64_t entityID, float sizeX, float sizeY);
		float BoxCollider2DComponent_GetDensity(uint64_t entityID);
		void BoxCollider2DComponent_SetDensity(uint64_t entityID, float density);
		float BoxCollider2DComponent_GetFriction(uint64_t entityID);
		void BoxCollider2DComponent_SetFriction(uint64_t entityID, float friction);
		float BoxCollider2DComponent_GetRestitution(uint64_t entityID);
		void BoxCollider2DComponent_SetRestitution(uint64_t entityID, float restitution);
		float BoxCollider2DComponent_GetRestitutionThreshold(uint64_t entityID);
		void BoxCollider2DComponent_SetRestitutionThreshold(uint64_t entityID, float restitutionThreshold);
		float BoxCollider2DComponent_GetCollisionRayX(uint64_t entityID);
		float BoxCollider2DComponent_GetCollisionRayY(uint64_t entityID);
		void BoxCollider2DComponent_SetCollisionRay(uint64_t entityID, float rayX, float rayY);
		bool BoxCollider2DComponent_GetAwake(uint64_t entityID);
		void BoxCollider2DComponent_SetAwake(uint64_t entityID, bool setAwake);

#pragma endregion
		*/
		/*
#pragma region CircleCollider2DComponent

		float CircleCollider2DComponent_GetOffsetX(uint64_t entityID);
		float CircleCollider2DComponent_GetOffsetY(uint64_t entityID);
		void CircleCollider2DComponent_SetOffset(uint64_t entityID, float offsetX, float offsetY);
		float CircleCollider2DComponent_GetRadius(uint64_t entityID);
		void CircleCollider2DComponent_SetRadius(uint64_t entityID, float radius);
		float CircleCollider2DComponent_GetDensity(uint64_t entityID);
		void CircleCollider2DComponent_SetDensity(uint64_t entityID, float density);
		float CircleCollider2DComponent_GetFriction(uint64_t entityID);
		void CircleCollider2DComponent_SetFriction(uint64_t entityID, float friction);
		float CircleCollider2DComponent_GetRestitution(uint64_t entityID);
		void CircleCollider2DComponent_SetRestitution(uint64_t entityID, float restitution);
		float CircleCollider2DComponent_GetRestitutionThreshold(uint64_t entityID);
		void CircleCollider2DComponent_SetRestitutionThreshold(uint64_t entityID, float restitutionThreshold);
		float CircleCollider2DComponent_GetCollisionRayX(uint64_t entityID);
		float CircleCollider2DComponent_GetCollisionRayY(uint64_t entityID);
		void CircleCollider2DComponent_SetCollisionRay(uint64_t entityID, float rayX, float rayY);
		bool CircleCollider2DComponent_GetAwake(uint64_t entityID);
		void CircleCollider2DComponent_SetAwake(uint64_t entityID, bool setAwake);

#pragma endregion
		*/
		/*
#pragma region AudioListenerComponent

		bool AudioListenerComponent_GetActive(uint64_t entityID);
		void AudioListenerComponent_SetActive(uint64_t entityID, bool active);

#pragma endregion

#pragma region AudioSourceComponent

		AssetHandle AudioSourceComponent_GetAssetHandle(uint64_t entityID);
		void AudioSourceComponent_SetAssetHandle(uint64_t entityID, AssetHandle handle);
		float AudioSourceComponent_GetVolume(uint64_t entityID);
		void AudioSourceComponent_SetVolume(uint64_t entityID, float volume);
		float AudioSourceComponent_GetPitch(uint64_t entityID);
		void AudioSourceComponent_SetPitch(uint64_t entityID, float pitch);
		bool AudioSourceComponent_GetPlayOnAwake(uint64_t entityID);
		void AudioSourceComponent_SetPlayOnAwake(uint64_t entityID, bool playOnAwake);
		bool AudioSourceComponent_GetLooping(uint64_t entityID);
		void AudioSourceComponent_SetLooping(uint64_t entityID, bool looping);
		bool AudioSourceComponent_GetSpatialization(uint64_t entityID);
		void AudioSourceComponent_SetSpatialization(uint64_t entityID, bool spatialization);
		int AudioSourceComponent_GetAttenuationModel(uint64_t entityID);
		void AudioSourceComponent_SetAttenuationModel(uint64_t entityID, int attenuationModel);
		float AudioSourceComponent_GetRollOff(uint64_t entityID);
		void AudioSourceComponent_SetRollOff(uint64_t entityID, float rollOff);
		float AudioSourceComponent_GetMinGain(uint64_t entityID);
		void AudioSourceComponent_SetMinGain(uint64_t entityID, float minGain);
		float AudioSourceComponent_GetMaxGain(uint64_t entityID);
		void AudioSourceComponent_SetMaxGain(uint64_t entityID, float maxGain);
		float AudioSourceComponent_GetMinDistance(uint64_t entityID);
		void AudioSourceComponent_SetMinDistance(uint64_t entityID, float minDistance);
		float AudioSourceComponent_GetMaxDistance(uint64_t entityID);
		void AudioSourceComponent_SetMaxDistance(uint64_t entityID, float maxDistance);
		float AudioSourceComponent_GetConeInnerAngle(uint64_t entityID);
		void AudioSourceComponent_SetConeInnerAngle(uint64_t entityID, float coneInnerAngle);
		float AudioSourceComponent_GetConeOuterAngle(uint64_t entityID);
		void AudioSourceComponent_SetConeOuterAngle(uint64_t entityID, float coneOuterAngle);
		float AudioSourceComponent_GetConeOuterGain(uint64_t entityID);
		void AudioSourceComponent_SetConeOuterGain(uint64_t entityID, float coneOuterGain);
		void AudioSourceComponent_SetCone(uint64_t entityID, float coneInnerAngle, float coneOuterAngle, float coneOuterGain);
		float AudioSourceComponent_GetDopplerFactor(uint64_t entityID);
		void AudioSourceComponent_SetDopplerFactor(uint64_t entityID, float dopplerFactor);
		bool AudioSourceComponent_IsPlaying(uint64_t entityID);
		void AudioSourceComponent_Play(uint64_t entityID);
		void AudioSourceComponent_Pause(uint64_t entityID);
		void AudioSourceComponent_UnPause(uint64_t entityID);
		void AudioSourceComponent_Stop(uint64_t entityID);

#pragma endregion
		*/
	}
}
