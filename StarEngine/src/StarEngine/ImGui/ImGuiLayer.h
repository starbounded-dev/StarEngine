#pragma once

#include "StarEngine/Core/Layer.h"

#include "ImGuiRenderer.h"

namespace StarEngine {

	class ImGuiLayer : public Layer
	{
	public:
		static ImGuiLayer* Create() { return new ImGuiLayer(); }

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
		RefPtr<ImGuiRenderer> m_ImGuiRenderer;
	};

}
