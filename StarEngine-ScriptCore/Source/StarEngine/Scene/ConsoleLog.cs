using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.CompilerServices;

namespace StarEngine
{
	public class ConsoleLog
	{
		public static void Trace(object message)
        {
            InternalCalls.ConsoleLog_Trace(message);
        }

        public static void Info(object message)
        {
            InternalCalls.ConsoleLog_Info(message);
        }

        public static void Warning(object message)
        {
            InternalCalls.ConsoleLog_Warning(message);
        }

        public static void Error(object message)
        {
            InternalCalls.ConsoleLog_Error(message);
        }

        public static void Critical(object message)
        {
            InternalCalls.ConsoleLog_Critical(message);
        }
	}
}
