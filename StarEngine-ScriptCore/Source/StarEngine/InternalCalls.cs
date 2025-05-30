using System;
using System.Runtime.CompilerServices;
using Coral.Managed.Interop;

namespace StarEngine
{
	internal static unsafe class InternalCalls
	{
		#region Scene

		internal static delegate*<NativeString, ulong> Scene_CreateEntity;
		internal static delegate*<ulong, void> Scene_SetSelectedEntity;

		#endregion
		#region Entity

		internal static delegate* unmanaged<ulong, ReflectionType, bool> Entity_HasComponent;
		internal static delegate*<NativeString, ulong> Entity_FindEntityByName;

		#endregion
		#region ConsoleLog

		//internal static delegate*<object, void> ConsoleLog_Trace;
		//internal static delegate*<object, void> ConsoleLog_Info;
		//internal static delegate*<object, void> ConsoleLog_Warning;
		//internal static delegate*<object, void> ConsoleLog_Error;
		//internal static delegate*<object, void> ConsoleLog_Critical;

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
		#region TransformComponent

		internal static delegate*<ulong, float, float, float, void> TransformComponent_SetTranslation;

		#endregion
		#region TextComponent

		internal static delegate*<ulong, NativeString> TextComponent_GetText;
		internal static delegate*<ulong, NativeString, void> TextComponent_SetText;
		internal static delegate*<ulong, float, float, float, float, void> TextComponent_SetColor;
		internal static delegate*<ulong, float> TextComponent_GetKerning;
		internal static delegate*<ulong, float, void> TextComponent_SetKerning;
		internal static delegate*<ulong, float> TextComponent_GetLineSpacing;
		internal static delegate*<ulong, float, void> TextComponent_SetLineSpacing;

		#endregion
		#region RigidBody2DComponent

		internal static delegate*<ulong, float, float, float, float, bool, void> RigidBody2DComponent_ApplyLinearImpulse;
		internal static delegate*<ulong, float, float, bool, void> RigidBody2DComponent_ApplyLinearImpulseToCenter;
		internal static delegate*<ulong, RigidBody2DComponent.BodyType> RigidBody2DComponent_GetType;
		internal static delegate*<ulong, RigidBody2DComponent.BodyType, void> RigidBody2DComponent_SetType;

		#endregion
	}
}
