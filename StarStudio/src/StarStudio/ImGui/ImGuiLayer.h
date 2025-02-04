#pragma once

#include "StarStudio/Core/Layer.h"

#include "StarStudio/Events/ApplicationEvent.h"
#include "StarStudio/Events/KeyEvent.h"
#include "StarStudio/Events/MouseEvent.h"

namespace StarStudio {

	class ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnImGuiRender() override;

		void Begin();
		void End();
	private:
		float m_Time = 0.0f;
	};

}