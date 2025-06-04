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
		public static bool IsKeyDown(KeyCode keycode)
		{
			unsafe
			{
				return InternalCalls.Input_IsKeyDown(keycode);
			}
		}
		public static bool IsKeyUp(KeyCode keycode)
		{
			unsafe
			{
				return InternalCalls.Input_IsKeyUp(keycode);
			}
		}

		public static bool IsMouseButtonPressed(MouseButton button)
		{
			unsafe
			{
				return InternalCalls.Input_IsMouseButtonPressed(button);
			}
		}

		public static bool PressMouseButton(MouseButton button)
		{
			unsafe
			{
				return InternalCalls.Input_PressMouseButton(button);
			}
		}

		public static bool ReleaseMouseButton(MouseButton button)
		{
			unsafe
			{
				return InternalCalls.Input_ReleaseMouseButton(button);
			}
		}

		public static Vector2 GetMousePosition()
		{
			unsafe
			{
				float resultX = InternalCalls.Input_GetMousePositionX();
				float resultY = InternalCalls.Input_GetMousePositionY();

				return new Vector2(resultX, resultY);
			}
		}

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
