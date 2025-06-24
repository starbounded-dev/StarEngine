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
		/// <summary>
		/// Changes the cursor to the specified image file with the given hotspot coordinates.
		/// </summary>
		/// <param name="filepath">The file path to the cursor image.</param>
		/// <param name="hotspotX">The X coordinate of the cursor hotspot.</param>
		/// <param name="hotspotY">The Y coordinate of the cursor hotspot.</param>

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

		/// <summary>
		/// Closes the application.
		/// </summary>
		public static void Quit()
		{
			unsafe
			{
				InternalCalls.Scene_CloseApplication();
			}
		}

		/// <summary>
		/// Loads a new scene using the specified asset handle if it is valid.
		/// </summary>
		/// <param name="assetHandle">The handle of the scene asset to load.</param>
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

		/// <summary>
		/// Returns the name of the currently loaded scene.
		/// </summary>
		/// <returns>The current scene's name as a string.</returns>
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

		/// <summary>
		/// Returns the ID of the entity currently under the cursor, or zero if no entity is hovered.
		/// </summary>
		/// <returns>The hovered entity's ID, or zero if none.</returns>
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

		/// <summary>
		/// Renders an outline around the entity currently hovered by the cursor using the specified RGBA color.
		/// </summary>
		/// <param name="outlineColor">The color of the outline, specified as a Vector4 (RGBA).</param>
		public static void RenderHoveredEntityOutline(Vector4 outlineColor)
		{
			unsafe
			{
				ulong entityID = InternalCalls.Scene_GetHoveredEntity();
				InternalCalls.Scene_RenderHoveredEntityOutline(entityID, outlineColor.X, outlineColor.Y, outlineColor.Z, outlineColor.W);
			}
		}

		/// <summary>
		/// Renders an outline around the currently selected entity using the specified RGBA color.
		/// </summary>
		/// <param name="outlineColor">The color of the outline as a Vector4 (RGBA).</param>
		public static void RenderSelectedEntityOutline(Vector4 outlineColor)
		{
			unsafe
			{
				ulong entityID = InternalCalls.Scene_GetSelectedEntity();
				InternalCalls.Scene_RenderSelectedEntityOutline(entityID, outlineColor.X, outlineColor.Y, outlineColor.Z, outlineColor.W);
			}
		}

		/// <summary>
		/// Returns an Entity instance for the specified entity ID if it is valid; otherwise, returns null.
		/// </summary>
		/// <param name="entityID">The unique identifier of the entity to retrieve.</param>
		/// <returns>The Entity corresponding to the given ID, or null if the entity is not valid.</returns>
		public static Entity GetEntity(ulong entityID)
		{
			unsafe
			{
				Entity entity = InternalCalls.Scene_IsEntityValid(entityID) ? new Entity(entityID) : null;
				return entity;
			}
		}

		/// <summary>
		/// Returns the ID of the currently selected entity in the scene.
		/// </summary>
		/// <returns>The unique identifier of the selected entity, or zero if no entity is selected.</returns>
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
