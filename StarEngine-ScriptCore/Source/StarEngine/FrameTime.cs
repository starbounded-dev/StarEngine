﻿﻿﻿﻿namespace StarEngine
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
