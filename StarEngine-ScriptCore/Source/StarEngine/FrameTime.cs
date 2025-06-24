namespace StarEngine
{
	public struct FrameTime
	{
		static float FPSDeltaTime;
		static float MillisecondsDeltaTime;
		[ThreadStatic]
		static float FPSDeltaTime;
		[ThreadStatic]
		static float FPS;
		static float Milliseconds;

		/// <summary>
		/// Calculates and returns the current frames per second (FPS) based on the provided time step.
		/// </summary>
		/// <param name="ts">The elapsed time since the last frame, in seconds.</param>
		/// <returns>The current FPS value, updated approximately every 0.1 seconds.</returns>
		public static float GetFPS(float ts)
		{
			DeltaTime += ts;

			if (DeltaTime > 0.1f)
			{
				DeltaTime -= 0.1f;
				FPS = 1.0f / ts;
			}

			return FPS;
		}

		/// <summary>
		/// Calculates and returns the current frame time in milliseconds, updating the value approximately every 0.1 seconds based on the provided time step.
		/// </summary>
		/// <param name="ts">The elapsed time since the last frame, in seconds.</param>
		/// <returns>The most recently calculated frame time in milliseconds.</returns>
		public static float GetMilliseconds(float ts)
		{
			MillisecondsDeltaTime += ts;

			if (MillisecondsDeltaTime > 0.1f)
			{
				MillisecondsDeltaTime -= 0.1f;
				FPS = 1.0f / ts;
				Milliseconds = 1000.0f / FPS;
			}


			return Milliseconds;
		}
	}
}
