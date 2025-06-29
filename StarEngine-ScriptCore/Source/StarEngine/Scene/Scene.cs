using StarEngine;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;

namespace StarEngine
{
	[EditorAssignable]
	public static class Scene
	{
		//public static Entity CreateEntity(string tag = "Unnamed") => new Entity(InternalCalls.Scene_CreateEntity(tag));

		public static string Cursor
		{
			get
			{
				unsafe
				{
					return InternalCalls.Scene_GetCursor();
				}
			}
			set
			{
				unsafe
				{
					InternalCalls.Scene_SetCursor(value);
				}
			}
		}

		public static Vector2 CursorHotSpot
		{
			get
			{
				unsafe
				{
					float resultX = InternalCalls.Scene_GetMouseHotSpotX();
					float resultY = InternalCalls.Scene_GetMouseHotSpotY();

					return new Vector2(resultX, resultY);
				}
			}
			set
			{
				unsafe
				{
					Vector2 result = (Vector2)value;
					InternalCalls.Scene_SetMouseHotSpot(result.X, result.Y);
				}
			}
		}

		//public static void ChangeCursor(string filepath, Vector2 hotspot)
		//{
		//    unsafe
		//    {
		//        InternalCalls.Scene_ChangeCursor(filepath, hotspot);
		//    }
		//}

		public static void ChangeCursor(string filepath, float hotspotX, float hotspotY)
		{
			unsafe
			{
				InternalCalls.Scene_ChangeCursor(filepath, hotspotX, hotspotY);
			}
		}

		public static string Name
		{
			get
			{
				unsafe
				{
					return InternalCalls.Scene_GetName();
				}
			}
			set
			{
				unsafe
				{
					InternalCalls.Scene_SetName(value);
				}
			}
		}

		public static bool Pause
		{
			get
			{
				unsafe
				{
					bool result = InternalCalls.Scene_IsGamePaused();
					return result;
				}
			}
			set
			{
				unsafe
				{
					InternalCalls.Scene_SetPauseGame(value);
				}
			}
		}

		public static void Quit()
		{
			unsafe
			{
				InternalCalls.Scene_CloseApplication();
			}
		}

		public static void LoadScene(AssetHandle assetHandle)
		{
			unsafe
			{
				if (!assetHandle.IsValid() /*assetHandle == 0*/)
				{
					Console.WriteLine($"SceneManager: Tried to a load an invalid scene!");
					return;
				}


				InternalCalls.Scene_LoadScene(assetHandle);
			}
		}

		public static string GetCurrentFilename()
		{
			unsafe
			{
				return InternalCalls.Scene_GetName();
			}
		}

		public static Entity HoveredEntity
		{
			get
			{
				unsafe
				{
					ulong entityID = InternalCalls.Scene_GetHoveredEntity();
					Entity entity = InternalCalls.Scene_IsEntityValid(entityID) ? new Entity(entityID) : null;
					return entity;
				}
			}
		}

		public static ulong GetHoveredEntityID()
		{
			unsafe
			{
				ulong entityID = InternalCalls.Scene_GetHoveredEntity();
				return entityID;
			}
		}

		public static Entity SelectedEntity
		{
			get
			{
				unsafe
				{
					ulong entityID = InternalCalls.Scene_GetSelectedEntity();
					Entity entity = InternalCalls.Scene_IsEntityValid(entityID) ? new Entity(entityID) : null;
					return entity;
				}
			}
			set
			{
				unsafe
				{
					if (value == null)
					{
						InternalCalls.Scene_SetSelectedEntity(0UL); // Deselect if null
						return;
					}
					InternalCalls.Scene_SetSelectedEntity(value.ID);
				}
			}
		}

		public static void RenderHoveredEntityOutline(Vector4 outlineColor)
		{
			unsafe
			{
				ulong entityID = InternalCalls.Scene_GetHoveredEntity();
				InternalCalls.Scene_RenderHoveredEntityOutline(entityID, outlineColor.X, outlineColor.Y, outlineColor.Z, outlineColor.W);
			}
		}

		public static void RenderSelectedEntityOutline(Vector4 outlineColor)
		{
			unsafe
			{
				ulong entityID = InternalCalls.Scene_GetSelectedEntity();
				InternalCalls.Scene_RenderSelectedEntityOutline(entityID, outlineColor.X, outlineColor.Y, outlineColor.Z, outlineColor.W);
			}
		}

		public static Entity GetEntity(ulong entityID)
		{
			unsafe
			{
				Entity entity = InternalCalls.Scene_IsEntityValid(entityID) ? new Entity(entityID) : null;
				return entity;
			}
		}

		public static ulong GetSelectedEntityID()
		{
			unsafe
			{
				ulong entityID = InternalCalls.Scene_GetSelectedEntity();
				return entityID;
			}
		}

		//public static unsafe void GetEntityComponent(ulong id, void* component)
		//{
		//	unsafe
		//	{
		//		InternalCalls.Scene_GetEntityComponent(id, component);
		//	}
		//}
	}
}
