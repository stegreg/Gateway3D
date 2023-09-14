#include "../Gateway/Gateway3D.h"

using namespace Gateway;

int main()
{
	Ref<Device> device = Device::Create();

	Ref<RenderWindow> window = RenderWindow::Create("Hello Cube!", 1920, 1080);

	Ref<Keyboard> keyboard = Keyboard::Create(window);

	Ref<Context> context = Context::Create(window);

	// Make sure the needed assets are imported into the engine's optimized format
	Texture::Import("Textures/Wood/007/BaseColor.jpg");
	Texture::Import("Textures/Wood/007/Roughness.jpg");
	Model::Import("Models/Box.glb");

	Ref<World> world = World::Create();

	Ref<RenderPath> renderPath = RenderPath::Create();

	Ref<Camera> camera = Camera::Create(world, "MyMainCamera");
	camera->SetFOV(70.0);
	camera->SetPosition(0.0, 0.0, -5.0);
	camera->SetClearColor({ 0.1, 0.2, 0.3, 1.0 });
	camera->SetRenderTarget(window);

	Ref<DirectionalLight> sun = DirectionalLight::Create(world);
	sun->SetColor({ 1.0, 1.0, 1.0 });
	sun->SetIntensity(5.0f);

	Ref<Material>	boxMaterial = Material::Load("Materials/Box.material");
	Ref<Model>		boxModel	= Model::Load("Models/Box.model");

	Ref<ModelInstance> boxInstance = ModelInstance::Create(world);
	boxInstance->SetModel(boxModel);
	boxInstance->SetOverrideMaterial(boxMaterial);
	boxInstance->SetPosition(0.0, 0.0, 5.0);
	Quat boxYaw, boxPitch;
	boxPitch.FromRotation(-20.0, { 1.0, 0.0, 0.0 });
	boxInstance->SetRotation(boxPitch);

	bool run = true;
	while ( run )
	{
		window->PollEvents();

		if ( keyboard->IsKeyPressed(KEY_ESCAPE)||window->IsClosed() )
			run = false;

		boxYaw.FromRotation(1.0, { 0.0, 1.0, 0.0 });
		boxInstance->Rotate(boxYaw);

		world->Update();

		renderPath->Execute(world);

		window->Present(true);
	}
}