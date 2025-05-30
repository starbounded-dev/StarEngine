using System;

namespace StarEngine
{
	[AttributeUsage(AttributeTargets.Class, AllowMultiple = false, Inherited = true)]
	internal class EditorAssignableAttribute : Attribute { }
}
