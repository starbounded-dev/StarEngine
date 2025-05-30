#pragma once

#include "StarEngine/Asset/Asset.h"
#include "StarEngine/Scene/Components.h"
#include "StarEngine/Core/Input.h"

#include <Coral/Assembly.hpp>
#include <Coral/String.hpp>
#include <Coral/Array.hpp>

//#include <glm/glm.hpp>

namespace StarEngine {

	// Forward declarations
	class Entity;

	class ScriptGlue
	{
	public:
		static void RegisterGlue(Coral::ManagedAssembly& coreAssembly);

		static Entity GetHoveredEntity();
		static void SetHoveredEntity(Entity entity);

		static Entity GetSelectedEntity();
		static void SetSelectedEntity(Entity entity);

	private:
		static void RegisterComponentTypes(Coral::ManagedAssembly& coreAssembly);
		static void RegisterInternalCalls(Coral::ManagedAssembly& coreAssembly);

	public:
		static bool s_CalledSetCursor;
		static bool s_ChangedCursor;
		static glm::vec2 s_CursorHotSpot;

		static std::string s_SetCursorPath;

		inline static bool s_IsCursorInViewport = false;
	};
}
