#include "../Gateway/Gateway3D.h"

using namespace Gateway;

int main()
{
	const int width  = 1920;
	const int height = 1080;

	Ref<Device> device = Device::Create();

	{
		Ref<RenderWindow> splashWindow = RenderWindow::Create("Hello splash screen!", 800, 400);

		Ref<Context> splashContext = Context::Create(splashWindow);

		Ref<World> dummyWorld = World::Create();

		Ref<Camera> dummyCamera = Camera::Create(dummyWorld);
		dummyCamera->SetClearColor({ 0.1, 0.2, 0.3, 1.0 });
		dummyCamera->SetRenderTarget(splashWindow);

		Ref<RenderPath> splashRenderPath = RenderPath::Create();

		dummyWorld->Update();
		
		splashRenderPath->Execute(dummyWorld);

		splashWindow->Present();

		// Load suff...
		System::Sleep(2000);
	}

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