using System;
using System.Runtime.CompilerServices;
using Coral.Managed.Interop;

namespace StarEngine
{
	internal static unsafe class InternalCalls
	{
		#region AssetHandle

		internal static delegate*<AssetHandle, bool> AssetHandle_IsValid;

		#endregion
		#region Scene

		internal static delegate*<NativeString, bool> Scene_IsSceneValid;
		internal static delegate*<AssetHandle, void> Scene_LoadScene;
		internal static delegate*<NativeString> Scene_GetCursor;
		internal static delegate*<NativeString, void> Scene_SetCursor;
		internal static delegate*<float> Scene_GetMouseHotSpotX;
		internal static delegate*<float> Scene_GetMouseHotSpotY;
		internal static delegate*<float, float, void> Scene_SetMouseHotSpot;
		//internal static delegate*<string, Vector2, void> Scene_ChangeCursor;
		internal static delegate*<NativeString, float, float, void> Scene_ChangeCursor;
		internal static delegate*<NativeString> Scene_GetName;
		internal static delegate*<NativeString, void> Scene_SetName;
		internal static delegate*<bool> Scene_IsGamePaused;
		internal static delegate*<bool, void> Scene_SetPauseGame;
		internal static delegate*<void> Scene_CloseApplication;
		internal static delegate*<NativeString, ulong> Scene_CreateEntity;
		internal static delegate*<ulong, bool> Scene_IsEntityValid;
		internal static delegate*<ulong> Scene_GetHoveredEntity;
		internal static delegate*<ulong> Scene_SetHoveredEntity;
		internal static delegate*<ulong> Scene_GetSelectedEntity;
		internal static delegate*<ulong, void> Scene_SetSelectedEntity;
		internal static delegate*<ulong, float, float, float, float, void> Scene_RenderHoveredEntityOutline;
		internal static delegate*<ulong, float, float, float, float, void> Scene_RenderSelectedEntityOutline;
		internal static delegate*<ulong, void*, void> Scene_GetEntityComponent;

		#endregion
		#region Entity

		internal static delegate* unmanaged<ulong, ReflectionType, void> Entity_CreateComponent;
		internal static delegate* unmanaged<ulong, ReflectionType, bool> Entity_HasComponent;
		internal static delegate* unmanaged<ulong, ReflectionType, bool> Entity_RemoveComponent;
		internal static delegate*<ulong, void> Entity_DestroyEntity;
		internal static delegate*<NativeString, ulong> Entity_FindEntityByTag;
		internal static delegate*<NativeString, ulong> Entity_FindEntityByName;
		//internal static delegate*<ulong, object> GetScriptInstance;

		#endregion
		#region ConsoleLog

		internal static delegate*<object, void> ConsoleLog_Trace;
		internal static delegate*<object, void> ConsoleLog_Info;
		internal static delegate*<object, void> ConsoleLog_Warning;
		internal static delegate*<object, void> ConsoleLog_Error;
		internal static delegate*<object, void> ConsoleLog_Critical;

		#endregion
		#region Input

		internal static delegate*<KeyCode, bool> Input_IsKeyDown;
		internal static delegate*<KeyCode, bool> Input_IsKeyUp;
		internal static delegate*<MouseButton, bool> Input_IsMouseButtonPressed;
		internal static delegate*<MouseButton, bool> Input_PressMouseButton;
		internal static delegate*<MouseButton, bool> Input_ReleaseMouseButton;
		internal static delegate*<float> Input_GetMousePositionX;
		internal static delegate*<float> Input_GetMousePositionY;
		internal static delegate*<float> Input_GetMouseWorldPositionX;
		internal static delegate*<float> Input_GetMouseWorldPositionY;

		#endregion
		#region Application

		internal static delegate*<float> Application_GetFPS;
		internal static delegate*<float> Application_GetFrameTime;
		internal static delegate*<float> Application_GetMinFrameTime;
		internal static delegate*<float> Application_GetMaxFrameTime;

		#endregion
		#region TagComponent

		internal static delegate*<ulong, NativeString> TagComponent_GetTag;
		internal static delegate*<ulong, NativeString, void> TagComponent_SetTag;

		#endregion
		#region TransformComponent

		internal static delegate*<ulong, bool> TransformComponent_GetIsEnabled;
		internal static delegate*<ulong, bool, void> TransformComponent_SetIsEnabled;
		internal static delegate*<ulong, Transform> TransformComponent_GetTransform;
		internal static delegate*<ulong, Transform, void> TransformComponent_SetTransform;
		internal static delegate*<ulong, float> TransformComponent_GetTranslationX;
		internal static delegate*<ulong, float> TransformComponent_GetTranslationY;
		internal static delegate*<ulong, float> TransformComponent_GetTranslationZ;
		internal static delegate*<ulong, float, float, float, void> TransformComponent_SetTranslation;
		internal static delegate*<ulong, float> TransformComponent_GetRotationX;
		internal static delegate*<ulong, float> TransformComponent_GetRotationY;
		internal static delegate*<ulong, float> TransformComponent_GetRotationZ;
		internal static delegate*<ulong, float, float, float, void> TransformComponent_SetRotation;
		internal static delegate*<ulong, float> TransformComponent_GetScaleX;
		internal static delegate*<ulong, float> TransformComponent_GetScaleY;
		internal static delegate*<ulong, float> TransformComponent_GetScaleZ;
		internal static delegate*<ulong, float, float, float, void> TransformComponent_SetScale;

		#endregion
		#region CameraComponent

		internal static delegate*<ulong, bool> CameraComponent_GetIsPrimary;
		internal static delegate*<ulong, bool, void> CameraComponent_SetPrimary;
		internal static delegate*<ulong, bool> CameraComponent_GetFixedAspectRatio;
		internal static delegate*<ulong, bool, void> CameraComponent_SetFixedAspectRatio;

		#endregion
		#region SpriteRendererComponent

		internal static delegate*<ulong, float> SpriteRendererComponent_GetOffsetX;
		internal static delegate*<ulong, float> SpriteRendererComponent_GetOffsetY;
		internal static delegate*<ulong, float> SpriteRendererComponent_GetOffsetZ;
		internal static delegate*<ulong, float, float, float, void> SpriteRendererComponent_SetOffset;
		internal static delegate*<ulong, float> SpriteRendererComponent_GetColorX;
		internal static delegate*<ulong, float> SpriteRendererComponent_GetColorY;
		internal static delegate*<ulong, float> SpriteRendererComponent_GetColorZ;
		internal static delegate*<ulong, float> SpriteRendererComponent_GetColorW;
		internal static delegate*<ulong, float, float, float, float, void> SpriteRendererComponent_SetColor;
		internal static delegate*<ulong, float> SpriteRendererComponent_GetUVX;
		internal static delegate*<ulong, float> SpriteRendererComponent_GetUVY;
		internal static delegate*<ulong, float, float, void> SpriteRendererComponent_SetUV;
		internal static delegate*<ulong, NativeString> SpriteRendererComponent_GetPath;
		internal static delegate*<ulong, NativeString, void> SpriteRendererComponent_SetPath;
		internal static delegate*<ulong, float> SpriteRendererComponent_GetParallaxSpeedX;
		internal static delegate*<ulong, float> SpriteRendererComponent_GetParallaxSpeedY;
		internal static delegate*<ulong, float, float, void> SpriteRendererComponent_SetParallaxSpeed;
		internal static delegate*<ulong, bool> SpriteRendererComponent_GetUseParallax;
		internal static delegate*<ulong, bool, void> SpriteRendererComponent_SetUseParallax;
		internal static delegate*<ulong, float> SpriteRendererComponent_GetParallaxDivision;
		internal static delegate*<ulong, float, void> SpriteRendererComponent_SetParallaxDivision;
		internal static delegate*<ulong, bool> SpriteRendererComponent_GetUseTextureAtlasAnimation;
		internal static delegate*<ulong, bool, void> SpriteRendererComponent_SetUseTextureAtlasAnimation;
		internal static delegate*<ulong, float> SpriteRendererComponent_GetAnimationSpeed;
		internal static delegate*<ulong, float, void> SpriteRendererComponent_SetAnimationSpeed;
		internal static delegate*<ulong, int> SpriteRendererComponent_GetNumTiles;
		internal static delegate*<ulong, int, void> SpriteRendererComponent_SetNumTiles;
		internal static delegate*<ulong, int> SpriteRendererComponent_GetStartIndexX;
		internal static delegate*<ulong, int, void> SpriteRendererComponent_SetStartIndexX;
		internal static delegate*<ulong, int> SpriteRendererComponent_GetStartIndexY;
		internal static delegate*<ulong, int, void> SpriteRendererComponent_SetStartIndexY;
		internal static delegate*<ulong, int> SpriteRendererComponent_GetCurrentIndexX;
		internal static delegate*<ulong, int, void> SpriteRendererComponent_SetCurrentIndexX;
		internal static delegate*<ulong, int> SpriteRendererComponent_GetCurrentIndexY;
		internal static delegate*<ulong, int, void> SpriteRendererComponent_SetCurrentIndexY;
		internal static delegate*<ulong, int> SpriteRendererComponent_GetColumn;
		internal static delegate*<ulong, int, void> SpriteRendererComponent_SetColumn;
		internal static delegate*<ulong, int> SpriteRendererComponent_GetRow;
		internal static delegate*<ulong, int, void> SpriteRendererComponent_SetRow;
		internal static delegate*<ulong, float> SpriteRendererComponent_GetSaturation;
		internal static delegate*<ulong, float, void> SpriteRendererComponent_SetSaturation;
		internal static delegate*<ulong, AssetHandle> SpriteRendererComponent_GetTextureAssetHandle;
		internal static delegate*<ulong, AssetHandle, void> SpriteRendererComponent_SetTextureAssetHandle;
		internal static delegate*<ulong, ulong> SpriteRendererComponent_GetTextureAssetID;
		internal static delegate*<ulong, ulong, void> SpriteRendererComponent_SetTextureAssetID;

		#endregion
		#region CircleRendererComponent

		internal static delegate*<ulong, float> CircleRendererComponent_GetColorX;
		internal static delegate*<ulong, float> CircleRendererComponent_GetColorY;
		internal static delegate*<ulong, float> CircleRendererComponent_GetColorZ;
		internal static delegate*<ulong, float> CircleRendererComponent_GetColorW;
		internal static delegate*<ulong, float, float, float, float, void> CircleRendererComponent_SetColor;
		internal static delegate*<ulong, float> CircleRendererComponent_GetUVX;
		internal static delegate*<ulong, float> CircleRendererComponent_GetUVY;
		internal static delegate*<ulong, float, float, void> CircleRendererComponent_SetUV;
		internal static delegate*<ulong, NativeString> CircleRendererComponent_GetPath;
		internal static delegate*<ulong, NativeString, void> CircleRendererComponent_SetPath;
		internal static delegate*<ulong, float> CircleRendererComponent_GetParallaxSpeedX;
		internal static delegate*<ulong, float> CircleRendererComponent_GetParallaxSpeedY;
		internal static delegate*<ulong, float, float, void> CircleRendererComponent_SetParallaxSpeed;
		internal static delegate*<ulong, bool> CircleRendererComponent_GetUseParallax;
		internal static delegate*<ulong, bool, void> CircleRendererComponent_SetUseParallax;
		internal static delegate*<ulong, float> CircleRendererComponent_GetParallaxDivision;
		internal static delegate*<ulong, float, void> CircleRendererComponent_SetParallaxDivision;
		internal static delegate*<ulong, bool> CircleRendererComponent_GetUseTextureAtlasAnimation;
		internal static delegate*<ulong, bool, void> CircleRendererComponent_SetUseTextureAtlasAnimation;
		internal static delegate*<ulong, float> CircleRendererComponent_GetAnimationSpeed;
		internal static delegate*<ulong, float, void> CircleRendererComponent_SetAnimationSpeed;
		internal static delegate*<ulong, int> CircleRendererComponent_GetNumTiles;
		internal static delegate*<ulong, int, void> CircleRendererComponent_SetNumTiles;
		internal static delegate*<ulong, int> CircleRendererComponent_GetStartIndexX;
		internal static delegate*<ulong, int, void> CircleRendererComponent_SetStartIndexX;
		internal static delegate*<ulong, int> CircleRendererComponent_GetStartIndexY;
		internal static delegate*<ulong, int, void> CircleRendererComponent_SetStartIndexY;
		internal static delegate*<ulong, int> CircleRendererComponent_GetCurrentIndexX;
		internal static delegate*<ulong, int, void> CircleRendererComponent_SetCurrentIndexX;
		internal static delegate*<ulong, int> CircleRendererComponent_GetCurrentIndexY;
		internal static delegate*<ulong, int, void> CircleRendererComponent_SetCurrentIndexY;
		internal static delegate*<ulong, int> CircleRendererComponent_GetColumn;
		internal static delegate*<ulong, int, void> CircleRendererComponent_SetColumn;
		internal static delegate*<ulong, int> CircleRendererComponent_GetRow;
		internal static delegate*<ulong, int, void> CircleRendererComponent_SetRow;

		#endregion
		#region LineRendererComponent

		internal static delegate*<ulong, float> LineRendererComponent_GetLineThickness;
		internal static delegate*<ulong, float, void> LineRendererComponent_SetLineThickness;

		#endregion
		#region TextComponent

		internal static delegate*<ulong, NativeString> TextComponent_GetText;
		internal static delegate*<ulong, NativeString, void> TextComponent_SetText;
		internal static delegate*<ulong, float> TextComponent_GetColorX;
		internal static delegate*<ulong, float> TextComponent_GetColorY;
		internal static delegate*<ulong, float> TextComponent_GetColorZ;
		internal static delegate*<ulong, float> TextComponent_GetColorW;
		internal static delegate*<ulong, float, float, float, float, void> TextComponent_SetColor;
		internal static delegate*<ulong, float> TextComponent_GetKerning;
		internal static delegate*<ulong, float, void> TextComponent_SetKerning;
		internal static delegate*<ulong, float> TextComponent_GetLineSpacing;
		internal static delegate*<ulong, float, void> TextComponent_SetLineSpacing;

		#endregion
		#region RigidBody2DComponent

		internal static delegate*<ulong, float, float, float, float, bool, void> RigidBody2DComponent_ApplyLinearImpulse;
		internal static delegate*<ulong, float, float, bool, void> RigidBody2DComponent_ApplyLinearImpulseToCenter;
		internal static delegate*<ulong, float> RigidBody2DComponent_GetLinearVelocityX;
		internal static delegate*<ulong, float> RigidBody2DComponent_GetLinearVelocityY;
		internal static delegate*<ulong, float, float, void> RigidBody2DComponent_SetLinearVelocity;
		//internal static delegate*<ulong, Vector2> RigidBody2DComponent_GetGravity;
		internal static delegate*<ulong, float> RigidBody2DComponent_GetGravityX;
		internal static delegate*<ulong, float> RigidBody2DComponent_GetGravityY;
		internal static delegate*<ulong, float, float, void> RigidBody2DComponent_SetGravity;
		internal static delegate*<ulong, bool> RigidBody2DComponent_GetEnabled;
		internal static delegate*<ulong, bool, void> RigidBody2DComponent_SetEnabled;
		internal static delegate*<ulong, RigidBody2DComponent.BodyType> RigidBody2DComponent_GetType;
		internal static delegate*<ulong, RigidBody2DComponent.BodyType, void> RigidBody2DComponent_SetType;

		#endregion
		#region BoxCollider2DComponent

		//internal static delegate*<ulong, Vector2> BoxCollider2DComponent_GetOffset;
		internal static delegate*<ulong, float> BoxCollider2DComponent_GetOffsetX;
		internal static delegate*<ulong, float> BoxCollider2DComponent_GetOffsetY;
		internal static delegate*<ulong, float, float, void> BoxCollider2DComponent_SetOffset;
		internal static delegate*<ulong, float> BoxCollider2DComponent_GetSizeX;
		internal static delegate*<ulong, float> BoxCollider2DComponent_GetSizeY;
		internal static delegate*<ulong, float, float, void> BoxCollider2DComponent_SetSize;
		internal static delegate*<ulong, float> BoxCollider2DComponent_GetDensity;
		internal static delegate*<ulong, float, void> BoxCollider2DComponent_SetDensity;
		internal static delegate*<ulong, float> BoxCollider2DComponent_GetFriction;
		internal static delegate*<ulong, float, void> BoxCollider2DComponent_SetFriction;
		internal static delegate*<ulong, float> BoxCollider2DComponent_GetRestitution;
		internal static delegate*<ulong, float, void> BoxCollider2DComponent_SetRestitution;
		internal static delegate*<ulong, float> BoxCollider2DComponent_GetRestitutionThreshold;
		internal static delegate*<ulong, float, void> BoxCollider2DComponent_SetRestitutionThreshold;
		internal static delegate*<ulong, float> BoxCollider2DComponent_GetCollisionRayX;
		internal static delegate*<ulong, float> BoxCollider2DComponent_GetCollisionRayY;
		internal static delegate*<ulong, float, float, void> BoxCollider2DComponent_SetCollisionRay;
		internal static delegate*<ulong, bool> BoxCollider2DComponent_GetAwake;
		internal static delegate*<ulong, bool, void> BoxCollider2DComponent_SetAwake;

		#endregion
		#region CircleCollider2DComponent

		internal static delegate*<ulong, float> CircleCollider2DComponent_GetOffsetX;
		internal static delegate*<ulong, float> CircleCollider2DComponent_GetOffsetY;
		internal static delegate*<ulong, float, float, void> CircleCollider2DComponent_SetOffset;
		internal static delegate*<ulong, float> CircleCollider2DComponent_GetRadius;
		internal static delegate*<ulong, float, void> CircleCollider2DComponent_SetRadius;
		internal static delegate*<ulong, float> CircleCollider2DComponent_GetDensity;
		internal static delegate*<ulong, float, void> CircleCollider2DComponent_SetDensity;
		internal static delegate*<ulong, float> CircleCollider2DComponent_GetFriction;
		internal static delegate*<ulong, float, void> CircleCollider2DComponent_SetFriction;
		internal static delegate*<ulong, float> CircleCollider2DComponent_GetRestitution;
		internal static delegate*<ulong, float, void> CircleCollider2DComponent_SetRestitution;
		internal static delegate*<ulong, float> CircleCollider2DComponent_GetRestitutionThreshold;
		internal static delegate*<ulong, float, void> CircleCollider2DComponent_SetRestitutionThreshold;
		internal static delegate*<ulong, float> CircleCollider2DComponent_GetCollisionRayX;
		internal static delegate*<ulong, float> CircleCollider2DComponent_GetCollisionRayY;
		internal static delegate*<ulong, float, float, void> CircleCollider2DComponent_SetCollisionRay;
		internal static delegate*<ulong, bool> CircleCollider2DComponent_GetAwake;
		internal static delegate*<ulong, bool, void> CircleCollider2DComponent_SetAwake;

		#endregion
		#region AudioListenerComponent

		internal static delegate*<ulong, bool> AudioListenerComponent_GetActive;
		internal static delegate*<ulong, bool, void> AudioListenerComponent_SetActive;

		#endregion
		#region AudioSourceComponent

		internal static delegate*<ulong, AssetHandle> AudioSourceComponent_GetAssetHandle;
		internal static delegate*<ulong, AssetHandle, void> AudioSourceComponent_SetAssetHandle;
		internal static delegate*<ulong, float> AudioSourceComponent_GetVolume;
		internal static delegate*<ulong, float, void> AudioSourceComponent_SetVolume;
		internal static delegate*<ulong, float> AudioSourceComponent_GetPitch;
		internal static delegate*<ulong, float, void> AudioSourceComponent_SetPitch;
		internal static delegate*<ulong, bool> AudioSourceComponent_GetPlayOnAwake;
		internal static delegate*<ulong, bool, void> AudioSourceComponent_SetPlayOnAwake;
		internal static delegate*<ulong, bool> AudioSourceComponent_GetLooping;
		internal static delegate*<ulong, bool, void> AudioSourceComponent_SetLooping;
		internal static delegate*<ulong, bool> AudioSourceComponent_GetSpatialization;
		internal static delegate*<ulong, bool, void> AudioSourceComponent_SetSpatialization;
		internal static delegate*<ulong, int> AudioSourceComponent_GetAttenuationModel;
		internal static delegate*<ulong, int, void> AudioSourceComponent_SetAttenuationModel;
		internal static delegate*<ulong, float> AudioSourceComponent_GetRollOff;
		internal static delegate*<ulong, float, void> AudioSourceComponent_SetRollOff;
		internal static delegate*<ulong, float> AudioSourceComponent_GetMinGain;
		internal static delegate*<ulong, float, void> AudioSourceComponent_SetMinGain;
		internal static delegate*<ulong, float> AudioSourceComponent_GetMaxGain;
		internal static delegate*<ulong, float, void> AudioSourceComponent_SetMaxGain;
		internal static delegate*<ulong, float> AudioSourceComponent_GetMinDistance;
		internal static delegate*<ulong, float, void> AudioSourceComponent_SetMinDistance;
		internal static delegate*<ulong, float> AudioSourceComponent_GetMaxDistance;
		internal static delegate*<ulong, float, void> AudioSourceComponent_SetMaxDistance;
		internal static delegate*<ulong, float> AudioSourceComponent_GetConeInnerAngle;
		internal static delegate*<ulong, float, void> AudioSourceComponent_SetConeInnerAngle;
		internal static delegate*<ulong, float> AudioSourceComponent_GetConeOuterAngle;
		internal static delegate*<ulong, float, void> AudioSourceComponent_SetConeOuterAngle;
		internal static delegate*<ulong, float> AudioSourceComponent_GetConeOuterGain;
		internal static delegate*<ulong, float, void> AudioSourceComponent_SetConeOuterGain;
		internal static delegate*<ulong, float, float, float, void> AudioSourceComponent_SetCone;
		internal static delegate*<ulong, float> AudioSourceComponent_GetDopplerFactor;
		internal static delegate*<ulong, float, void> AudioSourceComponent_SetDopplerFactor;
		internal static delegate*<ulong, bool> AudioSourceComponent_IsPlaying;
		internal static delegate*<ulong, void> AudioSourceComponent_Play;
		internal static delegate*<ulong, void> AudioSourceComponent_Pause;
		internal static delegate*<ulong, void> AudioSourceComponent_UnPause;
		internal static delegate*<ulong, void> AudioSourceComponent_Stop;

		#endregion
	}
}
