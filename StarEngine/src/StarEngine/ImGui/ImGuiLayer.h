#pragma once

#include "StarEngine/Core/Layer.h"

#include "StarEngine/Events/ApplicationEvent.h"
#include "StarEngine/Events/KeyEvent.h"
#include "StarEngine/Events/MouseEvent.h"

namespace StarEngine {

	class ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnEvent(Event& e) override;

		void Begin();
		void End();

		void BlockEvents(bool block) { m_BlockEvents = block; }

		void SetDarkThemeColors();
	private:
		bool m_BlockEvents = true;
	};

}
