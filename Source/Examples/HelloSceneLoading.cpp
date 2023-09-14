#include "../Gateway/Gateway3D.h"

#include <iostream>

using namespace Gateway;

const char* g_SceneName = "Worlds/MyWorld_01.world";

Ref<Model> g_PlaneModel = nullptr;
Ref<Model> g_BoxModel = nullptr;
Ref<Model> g_MonkeyModel = nullptr;

Ref<Material> g_BoxMaterial = nullptr;
Ref<Material> g_GroundMaterial = nullptr;
Ref<Material> g_WhiteMaterial = nullptr;

Ref<World> BuildAndSaveScene()
{
	Ref<World> world = World::Create();

	Ref<Camera> camera = Camera::Create(world, "MyMainCamera");
	camera->SetFOV(70.0);
	camera->SetPosition(0.0, 5.0, -10.0);
	camera->SetClearColor({ 0.1, 0.2, 0.3, 1.0 });

	Quat sunRot;
	sunRot.FromRotation(90.0f, { 1.0, 0.0, 0.0 });
	Ref<DirectionalLight> sun = DirectionalLight::Create(world);
	sun->SetRotation(sunRot);
	sun->SetColor({ 0.8, 0.7, 0.6 });
	sun->SetIntensity(0.3f);

	Ref<PointLight> pointLight1 = PointLight::Create(world);
	pointLight1->SetPosition(0.0, 10.0, 0.0);
	pointLight1->SetColor({ 1.0, 1.0, 1.0 });
	pointLight1->SetRange(100.0f);

	Ref<ModelInstance> ground = ModelInstance::Create(world);
	ground->SetModel(g_PlaneModel);
	ground->SetOverrideMaterial(g_GroundMaterial);
	ground->SetPosition(0.0, -2.0, 0.0);

	Ref<ModelInstance> monkeyInstance = ModelInstance::Create(world, "MyMonkeyHead");
	monkeyInstance->SetModel(g_MonkeyModel);
	monkeyInstance->SetOverrideMaterial(g_WhiteMaterial);
	monkeyInstance->SetPosition(0.0, 5.0, 10.0);

	for ( int x = -3; x<3; x++ )
	{
		for ( int z = -3; z<3; z++ )
		{
			Ref<ModelInstance> boxInstance = ModelInstance::Create(world);
			boxInstance->SetModel(g_BoxModel);
			boxInstance->SetOverrideMaterial(g_BoxMaterial);
			boxInstance->SetPosition(x*7.0, 0.0, z*7.0);
		}
	}

	World::Save(world, g_SceneName);

	return world;
}

Ref<World> LoadScene()
{
	Ref<World> world = World::Load(g_SceneName);

	return world;
}

int main()
{
	Ref<Device> device = Device::Create();

	Ref<RenderWindow> window = RenderWindow::Create("Hello Scene Loading!", 1920, 1080);

	Ref<Keyboard>	keyboard = Keyboard::Create(window);
	Ref<Mouse>		mouse = Mouse::Create(window);

	Mouse::SetRelativeMode(true);

	Ref<Context> context = Context::Create(window);

	Texture::Import("Textures/wood.jpg");
	Texture::Import("Textures/wood_specular.jpg");
	Texture::Import("Textures/container.png");
	Texture::Import("Textures/container_specular.png");

	Model::Import("Models/Plane.glb");
	Model::Import("Models/Monkey.glb");
	Model::Import("Models/Box.glb");

	g_PlaneModel		= Model::Load("Models/Plane.model");
	g_BoxModel			= Model::Load("Models/Box.model");
	g_MonkeyModel		= Model::Load("Models/Monkey.model");
	g_BoxMaterial		= Material::Load("Materials/Box.material");
	g_GroundMaterial	= Material::Load("Materials/Floor.material");
	g_WhiteMaterial		= Material::Load("Materials/BlueSphere.material");

	//Ref<World> world = BuildAndSaveScene();
	Ref<World> world = LoadScene();

	Ref<RenderPath> renderPath = RenderPath::Create();

	Ref<Material>	 postProcessMaterial = Material::Load("Materials/MyPostProcess.material");
	postProcessMaterial->SetTexture("uColorTexture", renderPath->GetColorTexture(), 0);
	Ref<PostProcess> postProcess = PostProcess::Create();
	postProcess->SetMaterial(postProcessMaterial);

	Ref<Camera>			camera			= world->GetSimObjectByName("MyMainCamera").Cast<Camera>();
	Ref<ModelInstance>	monkeyInstance	= world->GetSimObjectByName("MyMonkeyHead").Cast<ModelInstance>();

	camera->SetRenderTarget(window);
	camera->ChainPostProcess(postProcess);

	float camRotYAmount = 0.0f;
	float camRotXAmount = 0.0f;
	bool run = true;
	while ( run )
	{
		window->PollEvents();

		if ( keyboard->IsKeyPressed(KEY_ESCAPE)||window->IsClosed() )
			run = false;

		Quat monkeyRot;
		monkeyRot.FromRotation(2.0, { 0.0, 1.0, 0.0 });
		monkeyInstance->Rotate(monkeyRot);

		Vec3 camForward = camera->GetForward();
		Vec3 camRight= camera->GetRight();

		float speed = 0.1f;
		if ( keyboard->IsKeyDown(KEY_W) )
		{
			camera->Translate(camForward*speed);
		}
		if ( keyboard->IsKeyDown(KEY_S) )
		{
			camera->Translate(-camForward*speed);
		}
		if ( keyboard->IsKeyDown(KEY_D) )
		{
			camera->Translate(camRight*speed);
		}
		if ( keyboard->IsKeyDown(KEY_A) )
		{
			camera->Translate(-camRight*speed);
		}

		iVec2 mouseRelativeMotion = mouse->GetRelativeMotion();

		camRotXAmount += mouseRelativeMotion.y*0.1f;
		camRotYAmount += mouseRelativeMotion.x*0.1f;

		Quat camRotY, camRotX;
		camRotY.FromRotation(camRotYAmount, { 0.0, 1.0, 0.0 });
		camRotX.FromRotation(camRotXAmount, { 1.0, 0.0, 0.0 });

		camera->SetRotation(camRotY * camRotX);

		world->Update();

		renderPath->Execute(world);

		window->Present(true);
	}
}