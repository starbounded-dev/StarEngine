#include <StarStudio.h>

class Sandbox : public StarStudio::Application {
public:
	Sandbox() {

	}
	~Sandbox() {

	}
};

int main() {
	Sandbox* sandbox = new Sandbox();
	sandbox->Run();
	delete sandbox;
}