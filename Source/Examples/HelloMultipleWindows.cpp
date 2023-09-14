#include "../Gateway/Gateway3D.h"

using namespace Gateway;

Ref<RenderWindow> g_Window1 = nullptr;
Ref<RenderWindow> g_Window2 = nullptr;

Ref<Context> g_Context1 = nullptr;
Ref<Context> g_Context2 = nullptr;

Ref<ModelInstance> g_BoxInstance1 = nullptr;
Ref<ModelInstance> g_BoxInstance2 = nullptr;

Ref<RenderPath> g_RenderPath1 = nullptr;
Ref<RenderPath> g_RenderPath2 = nullptr;

Ref<World> g_World1 = nullptr;
Ref<World> g_World2 = nullptr;

Ref<Keyboard> g_Keyboard1 = nullptr;
Ref<Keyboard> g_Keyboard2 = nullptr;

Ref<World> BuildWorld(Ref<ModelInstance>& boxInstance, Ref<RenderWindow> target)
{
	Ref<World> world = World::Create();

	Ref<Camera> camera = Camera::Create(world, "MyMainCamera");
	camera->SetFOV(70.0);
	camera->SetPosition(0.0, 0.0, -5.0);
	camera->SetClearColor({ 0.1, 0.2, 0.3, 1.0 });
	camera->SetRenderTarget(target);

	Ref<DirectionalLight> sun = DirectionalLight::Create(world);
	sun->SetColor({ 1.0, 1.0, 1.0 });
	sun->SetIntensity(5.0f);

	Ref<Material>	boxMaterial = Material::Load("Materials/Box.material");
	Ref<Model>		boxModel	= Model::Load("Models/Box.model");

	boxInstance = ModelInstance::Create(world);
	boxInstance->SetModel(boxModel);
	boxInstance->SetOverrideMaterial(boxMaterial);
	boxInstance->SetPosition(0.0, 0.0, 5.0);
	Quat boxYaw, boxPitch;
	boxPitch.FromRotation(-20.0, { 1.0, 0.0, 0.0 });
	boxInstance->SetRotation(boxPitch);

	Quat pitch;
	pitch.FromRotation(-20.0, { 1.0, 0.0, 0.0 });
	boxInstance->SetRotation(pitch);

	return world;
}

int main()
{
	Ref<Device> device = Device::Create();

	Texture::Import("Textures/Wood/007/BaseColor.jpg");
	Texture::Import("Textures/Wood/007/Roughness.jpg");
	Model::Import("Models/Box.glb");
	
	{
		g_Window1 = RenderWindow::Create("Hello Window 1!", 1220, 800, 20, 400, 0);
		g_Context1 = Context::Create(g_Window1);

		g_Keyboard1 = Keyboard::Create(g_Window1);

		g_RenderPath1 = RenderPath::Create();

		g_World1 = BuildWorld(g_BoxInstance1, g_Window1);
	}

	{
		g_Window2 = RenderWindow::Create("Hello Window 2!", 1220, 800, 1300, 400, 0);
		g_Context2 = Context::Create(g_Window2);

		g_Keyboard2 = Keyboard::Create(g_Window2);

		g_RenderPath2 = RenderPath::Create();

		g_World2 = BuildWorld(g_BoxInstance2, g_Window2);
	}

	bool window1Closed = false;
	bool window2Closed = false;
	while ( !window1Closed||!window2Closed )
	{
		g_Window1->PollEvents();
		g_Window2->PollEvents();

		if ( g_Window1->IsClosed()||g_Keyboard1->IsKeyPressed(KEY_ESCAPE) )
		{
			window1Closed = true;
			g_Window1->Hide();
		}

		if ( g_Window2->IsClosed()||g_Keyboard2->IsKeyPressed(KEY_ESCAPE) )
		{
			window2Closed = true;
			g_Window2->Hide();
		}

		Context::MakeCurrent(g_Context1);
		{
			Quat yaw;
			yaw.FromRotation(1.0, { 0.0, 1.0, 0.0 });
			g_BoxInstance1->Rotate(yaw);

			g_World1->Update();

			g_RenderPath1->Execute(g_World1);

			g_Window1->Present(false);
		}

		Context::MakeCurrent(g_Context2);
		{
			Quat yaw;
			yaw.FromRotation(-1.0, { 0.0, 1.0, 0.0 });
			g_BoxInstance2->Rotate(yaw);

			g_World2->Update();

			g_RenderPath2->Execute(g_World2);

			g_Window2->Present(true);
		}
	}
}