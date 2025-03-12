#include <StarEngine.h>

#include <StarEngine/Core/Entrypoint.h>

#include "EditorLayer.h"

namespace StarEngine {

	class StarEditor : public Application
	{
	public:
		StarEditor(ApplicationCommandLineArgs args)
			: Application("Hazelnut", args)
		{
			PushLayer(new EditorLayer());
		}
		~StarEditor()
		{

		}
	};

	Application* CreateApplication(ApplicationCommandLineArgs args)
	{
		return new StarEditor(args);
	}
}
