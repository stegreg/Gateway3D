#include "../Gateway/Gateway3D.h"

#include <iostream>

using namespace Gateway;

const int g_GridSizeX = 20;
const int g_GridSizeY = 50;
const int g_GridSizeZ = 20;

int main()
{
	Ref<Device> device = Device::Create();

	Ref<RenderWindow> window = RenderWindow::Create("Hello MultiModelInstance!", 1920, 1080);

	Ref<Keyboard>	keyboard = Keyboard::Create(window);
	Ref<Mouse>		mouse = Mouse::Create(window);

	Mouse::SetRelativeMode(true);

	Ref<Context> context = Context::Create(window);
	
	Texture::Import("Textures/wood.jpg");
	Texture::Import("Textures/wood_specular.jpg");

	Model::Import("Models/Plane.glb");
	Model::Import("Models/Sphere.glb");

	Ref<Material>	groundMaterial	= Material::Load("Materials/Floor.material");
	Ref<Material>	blueMaterial	= Material::Load("Materials/BlueSphere.material");
	Ref<Model>		planeModel		= Model::Load("Models/Plane.model");
	Ref<Model>		sphereModel		= Model::Load("Models/Sphere.model");

	Ref<World> world = World::Create();

	Ref<RenderPath> renderPath = RenderPath::Create();

	Ref<Camera> camera = Camera::Create(world, "MyMainCamera");
	camera->SetFOV(70.0);
	camera->SetPosition(0.0, 20.0, -80.0);
	camera->SetClearColor({ 0.1, 0.2, 0.3, 1.0 });
	camera->SetRenderTarget(window);

	Ref<Material>	 postProcessMaterial = Material::Load("Materials/MyPostProcess.material");
	postProcessMaterial->SetTexture("uColorTexture", renderPath->GetColorTexture(), 0);
	Ref<PostProcess> postProcess = PostProcess::Create();
	postProcess->SetMaterial(postProcessMaterial);

	camera->ChainPostProcess(postProcess);

	Quat sunRot;
	sunRot.FromRotation(45.0f, { 1.0, 0.0, 0.0 });
	Ref<DirectionalLight> sun = DirectionalLight::Create(world);
	sun->SetRotation(sunRot);
	sun->SetColor({ 1.0, 1.0, 1.0 });
	sun->SetIntensity(2.0f);

	Ref<ModelInstance> ground = ModelInstance::Create(world);
	ground->SetModel(planeModel);
	ground->SetOverrideMaterial(groundMaterial);
	ground->SetPosition(0.0, -(g_GridSizeY/2.0) * 5.0, 0.0);

	Ref<MultiModelInstance> multiSphere = MultiModelInstance::Create(world);
	multiSphere->SetModel(sphereModel);
	multiSphere->SetOverrideMaterial(blueMaterial);

	multiSphere->SetNumTransforms(g_GridSizeX*g_GridSizeY*g_GridSizeZ);

	unsigned int index = 0;
	for ( int x = -g_GridSizeX/2; x<g_GridSizeX/2; x++ )
	{
		for ( int y = -g_GridSizeY/2; y<g_GridSizeY/2; y++ )
		{
			for ( int z = -g_GridSizeZ/2; z<g_GridSizeZ/2; z++ )
			{
				Mat4 transform;
				transform.FromTranslation(x*5.0, y*5.0, z*5.0);
				
				multiSphere->SetTransform(index++, transform);
			}
		}
	}

	float camRotYAmount = 0.0f;
	float camRotXAmount = 0.0f;
	bool run = true;
	while ( run )
	{
		window->PollEvents();

		if ( keyboard->IsKeyPressed(KEY_ESCAPE)||window->IsClosed() )
			run = false;

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