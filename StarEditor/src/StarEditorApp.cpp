#include <StarEngine.h>

#include <StarEngine/Core/Entrypoint.h>

#include "EditorLayer.h"

namespace StarEngine {

	class StarEditor : public Application
	{
	public:
		StarEditor()
		{
			PushLayer(new EditorLayer());
		}
		~StarEditor()
		{

		}
	};

	Application* CreateApplication()
	{
		return new StarEditor();
	}
}