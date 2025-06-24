using StarEngine;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace StarEngine
{
	public enum MouseButton
	{
		Button0 = 0,
		Button1 = 1,
		Button2 = 2,
		Button3 = 3,
		Button4 = 4,
		Button5 = 5,
		Left = Button0,
		Right = Button1,
		Middle = Button2
	}

	public class Input
	{
		/// <summary>
		/// Determines whether the specified key is currently pressed down.
		/// </summary>
		/// <param name="keycode">The key to check.</param>
		/// <returns>True if the key is pressed; otherwise, false.</returns>
		public static bool IsKeyDown(KeyCode keycode)
		{
			unsafe
			{
				return InternalCalls.Input_IsKeyDown(keycode);
			}
		}
		/// <summary>
		/// Determines whether the specified key is currently released.
		/// </summary>
		/// <param name="keycode">The key to check.</param>
		/// <returns>True if the key is not pressed; otherwise, false.</returns>
		public static bool IsKeyUp(KeyCode keycode)
		{
			unsafe
			{
				return InternalCalls.Input_IsKeyUp(keycode);
			}
		}

		/// <summary>
		/// Determines whether the specified mouse button is currently pressed.
		/// </summary>
		/// <param name="button">The mouse button to check.</param>
		/// <returns>True if the specified mouse button is pressed; otherwise, false.</returns>
		public static bool IsMouseButtonPressed(MouseButton button)
		{
			unsafe
			{
				return InternalCalls.Input_IsMouseButtonPressed(button);
			}
		}

		/// <summary>
		/// Simulates pressing the specified mouse button.
		/// </summary>
		/// <param name="button">The mouse button to press.</param>
		/// <returns>True if the mouse button press was successfully simulated; otherwise, false.</returns>
		public static bool PressMouseButton(MouseButton button)
		{
			unsafe
			{
				return InternalCalls.Input_PressMouseButton(button);
			}
		}

		/// <summary>
		/// Attempts to simulate releasing the specified mouse button.
		/// </summary>
		/// <param name="button">The mouse button to release.</param>
		/// <returns>True if the mouse button was successfully released; otherwise, false.</returns>
		public static bool ReleaseMouseButton(MouseButton button)
		{
			unsafe
			{
				return InternalCalls.Input_ReleaseMouseButton(button);
			}
		}

		/// <summary>
		/// Retrieves the current mouse cursor position in screen coordinates.
		/// </summary>
		/// <returns>A <see cref="Vector2"/> representing the X and Y position of the mouse cursor on the screen.</returns>
		public static Vector2 GetMousePosition()
		{
			unsafe
			{
				float resultX = InternalCalls.Input_GetMousePositionX();
				float resultY = InternalCalls.Input_GetMousePositionY();

				return new Vector2(resultX, resultY);
			}
		}

		/// <summary>
		/// Retrieves the current mouse cursor position in world coordinates.
		/// </summary>
		/// <returns>A <see cref="Vector2"/> representing the mouse position in world space.</returns>
		public static Vector2 GetMouseWorldPosition()
		{
			unsafe
			{
				float resultX = InternalCalls.Input_GetMouseWorldPositionX();
				float resultY = InternalCalls.Input_GetMouseWorldPositionY();

				return new Vector2(resultX, resultY);
			}
		}
	}
}
