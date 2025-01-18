#include <StarStudio.h>

class Sandbox : public StarStudio::Application {
public:
	Sandbox() {

	}
	~Sandbox() {

	}
};

StarStudio::Application* StarStudio::CreateApplication() {
	return new Sandbox();
}