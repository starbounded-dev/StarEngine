#pragma once

#include "StarEngine/Core/Layer.h"

#include "ImGuiRenderer.h"

namespace StarEngine {

	class ImGuiLayer : public Layer
	{
	public:
		static RefPtr<ImGuiLayer> Create() { return RefPtr<ImGuiLayer>::Create(); }

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		void Begin();
		void End();

		void SetDarkThemeColors();

		void AllowInputsEvents(bool allowEvents);
	public:
		ImGuiLayer() = default;
		virtual ~ImGuiLayer() = default;
	private:
		void InitPlatformInterface();
	private:
		ImGuiRenderer m_ImGuiRenderer;
	};

}
