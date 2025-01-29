#pragma once

#include "StarStudio/Core.h"
#include "StarStudio/Core/Timestep.h"
#include "StarStudio/Events/Event.h"

namespace StarStudio
{
	class STARSTUDIO_API Layer
	{
	public:
		Layer(const std::string& name = "Layer");
		virtual ~Layer();

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate(Timestep ts) {}
		virtual void OnImGuiRender() {}
		virtual void OnEvent(Event& event) {}

		inline const std::string& GetName() const { return m_DebugName; }
	protected:
		std::string m_DebugName;
	};
}
