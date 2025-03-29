#include "sepch.h"
#include "ScriptGlue.h"

#include "mono/metadata/object.h"

namespace StarEngine {

#define SE_ADD_INTERNAL_CALL(Name) mono_add_internal_call("StarEngine.InternalCalls::" #Name, Name)

	static void NativeLog(MonoString* string, int parameter)
	{
		char* cStr = mono_string_to_utf8(string);
		std::string str(cStr);
		mono_free(cStr);
		std::cout << str << ", " << parameter << std::endl;
	}

	static void NativeLog_Vector(glm::vec3* parameter, glm::vec3* outResult)
	{
		SE_CORE_WARN("Value: {0}", *parameter);
		*outResult = glm::normalize(*parameter);
	}

	static float NativeLog_VectorDot(glm::vec3* parameter)
	{
		SE_CORE_WARN("Value: {0}", *parameter);
		return glm::dot(*parameter, *parameter);
	}

	void ScriptGlue::RegisterFunctions()
	{
		SE_ADD_INTERNAL_CALL(NativeLog);
		SE_ADD_INTERNAL_CALL(NativeLog_Vector);
		SE_ADD_INTERNAL_CALL(NativeLog_VectorDot);
	}

}
