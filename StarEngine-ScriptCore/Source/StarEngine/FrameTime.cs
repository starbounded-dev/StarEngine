﻿﻿﻿﻿namespace StarEngine
{
	public struct FrameTime
	{
		static float DeltaTime;
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
			DeltaTime += ts;

			if (DeltaTime > 0.1f)
			{
				DeltaTime -= 0.1f;
				FPS = 1.0f / ts;
				Milliseconds = 1000.0f / FPS;
			}


			return Milliseconds;
		}
	}
}
