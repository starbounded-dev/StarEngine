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
		/// <summary>
        /// Logs a message with trace-level severity.
        /// </summary>
        /// <param name="message">The message to log.</param>
        public static void Trace(object message)
        {
            InternalCalls.ConsoleLog_Trace(message);
        }

        /// <summary>
        /// Logs an informational message to the console.
        /// </summary>
        /// <param name="message">The message to log.</param>
        public static void Info(object message)
        {
            InternalCalls.ConsoleLog_Info(message);
        }

        /// <summary>
        /// Logs a message with warning severity.
        /// </summary>
        /// <param name="message">The message to log.</param>
        public static void Warning(object message)
        {
            InternalCalls.ConsoleLog_Warning(message);
        }

        /// <summary>
        /// Logs a message with error severity.
        /// </summary>
        /// <param name="message">The message to log.</param>
        public static void Error(object message)
        {
            InternalCalls.ConsoleLog_Error(message);
        }

        /// <summary>
        /// Logs a message with critical severity.
        /// </summary>
        /// <param name="message">The message to log.</param>
        public static void Critical(object message)
        {
            InternalCalls.ConsoleLog_Critical(message);
        }
	}
}
