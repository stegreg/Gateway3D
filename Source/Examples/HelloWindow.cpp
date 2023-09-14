#include "../Gateway/Gateway3D.h"

using namespace Gateway;

int main()
{
	const int width  = 1920;
	const int height = 1080;

	Ref<Device> device = Device::Create();

	Ref<RenderWindow> window = RenderWindow::Create("Hello Window!", width, height);

	Ref<Context> context = Context::Create(window);

	bool run = true;
	while ( run )
	{
		window->PollEvents();

		if ( window->IsClosed() )
			run = false;

		window->Present(true);
	}

}