using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace StarEngine
{
	[EditorAssignable]
	public static class Application
	{
		public static float Framerate
        {
            get
            {
                unsafe
                {
                    return InternalCalls.Application_GetFPS();
                }
            }
        }

        public static float Frametime
        {
            get
            {
                unsafe
                {
                    return InternalCalls.Application_GetFrameTime();
                }
            }
        }

        public static float MinFrametime
        {
            get
            {
                unsafe
                {
                    return InternalCalls.Application_GetMinFrameTime();
                }
            }
        }
        public static float MaxFrametime
        {
            get
            {
                unsafe
                {
                    return InternalCalls.Application_GetMaxFrameTime();
                }
            }
        }

	}
}
