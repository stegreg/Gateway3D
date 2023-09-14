#include "../Gateway/Gateway3D.h"

#include <iostream>

using namespace Gateway;

int main()
{
	Ref<Device> device = Device::Create();

	Texture::Import("Textures/Marble/016/BaseColor.jpg");
	Texture::Import("Textures/Marble/016/Roughness.jpg");

	Ref<RenderWindow> window = RenderWindow::Create("Hello Scene!", 1920, 1080);

	Ref<Keyboard>	keyboard = Keyboard::Create(window);
	Ref<Mouse>		mouse	 = Mouse::Create(window);

	Mouse::SetRelativeMode(true);

	Ref<Context> context = Context::Create(window);

	Model::Import("Models/Plane.glb");
	Model::Import("Models/Sphere.glb");
	Model::Import("Models/Box.glb");

	Ref<Material>	boxMaterial		= Material::Load("Materials/Box.material");
	Ref<Material>	groundMaterial	= Material::Load("Materials/Floor.material");
	Ref<Material>	blueMaterial	= Material::Load("Materials/BlueSphere.material");
	Ref<Model>		planeModel		= Model::Load("Models/Plane.model");
	Ref<Model>		sphereModel		= Model::Load("Models/Sphere.model");
	Ref<Model>		boxModel		= Model::Load("Models/Box.model");

	Ref<World> world = World::Create();
	
	Ref<RenderPath> renderPath = RenderPath::Create();

	Ref<Camera> camera = Camera::Create(world, "MyMainCamera");
	camera->SetFOV(70.0);
	camera->SetPosition(0.0, 5.0, -10.0);
	camera->SetClearColor({ 0.1, 0.2, 0.3, 1.0 });
	camera->SetRenderTarget(window);

	Ref<Material>	 postProcessMaterial = Material::Load("Materials/MyPostProcess.material");
	postProcessMaterial->SetTexture("uColorTexture", renderPath->GetColorTexture(), 0);
	Ref<PostProcess> postProcess = PostProcess::Create();
	postProcess->SetMaterial(postProcessMaterial);

	camera->ChainPostProcess(postProcess);

	Ref<DirectionalLight> moon = DirectionalLight::Create(world);
	moon->SetColor({ 0.2, 0.3, 0.4 });	
	Quat moonRot;
	moonRot.FromRotation(180.0, { 0.0, 1.0, 0.0 });
	moon->SetRotation(moonRot);

	Quat sunRot;
	sunRot.FromRotation(90.0f, { 1.0, 0.0, 0.0 });
	Ref<DirectionalLight> sun = DirectionalLight::Create(world);
	sun->SetRotation(sunRot);
	sun->SetColor({ 1.0, 1.0, 1.0 });
	sun->SetIntensity(1.0);

	Ref<PointLight> redLight = PointLight::Create(world);
	redLight->SetPosition(0.0, 5.0, -20.0);
	redLight->SetColor({ 1.0, 0.0, 0.0 });
	redLight->SetRange(30.0f);
	redLight->SetIntensity(5.0);

	Ref<PointLight> greenLight = PointLight::Create(world);
	greenLight->SetPosition(20.0, 5.0, 0.0);
	greenLight->SetRange(20.0f);
	greenLight->SetColor({ 0.0, 1.0, 0.0 });
	greenLight->SetIntensity(5.0);

	Ref<PointLight> blueLight = PointLight::Create(world);
	blueLight->SetPosition(-20.0, 5.0, 0.0);
	blueLight->SetRange(20.0f);
	blueLight->SetColor({ 0.0, 0.0, 1.0 });
	blueLight->SetIntensity(5.0);

	//Ref<SpotLight> whiteSpotLight = SpotLight::Create(world);
	//whiteSpotLight->SetPosition(0.0, 5.0, 5.0);
	//whiteSpotLight->SetRange(100.0);

	Ref<ModelInstance> boxInstance = ModelInstance::Create(world);
	boxInstance->SetModel(boxModel);
	boxInstance->SetOverrideMaterial(boxMaterial);
	boxInstance->SetPosition(0.0, 0.0, 5);

	Ref<ModelInstance> ground = ModelInstance::Create(world);
	ground->SetModel(planeModel);
	ground->SetOverrideMaterial(groundMaterial);
	ground->SetPosition(0.0, -2.0, 0.0);

	groundMaterial->SetConstant("uHeightScale", 0.5);

	for ( int x = -3; x<3; x++ )
	{
		for ( int z = -3; z<3; z++ )
		{
			Ref<ModelInstance> sphere = ModelInstance::Create(world);
			sphere->SetModel(sphereModel);
			sphere->SetOverrideMaterial(blueMaterial);
			sphere->SetPosition(x * 10.0, 1.0, z * 10.0);
		}
	}

	float xOffset = 0.0, yOffset = 0.0;

	float angleOffset = 0.0;

	float camRotYAmount = 0.0f;
	float camRotXAmount = 0.0f;
	bool run = true;
	while ( run )
	{
		window->PollEvents();

		if ( keyboard->IsKeyPressed(KEY_ESCAPE)||window->IsClosed() )
			run = false;

		xOffset += 0.01f;
		yOffset += 0.01f;
		redLight->SetPosition(Vec3{ sinf(xOffset)*40.0f, 5.0, cosf(yOffset)*40.0f });
		greenLight->SetPosition(Vec3{ -sinf(xOffset+0.2f)*30.0f, 5.0, cosf(yOffset+0.2f)*30.0f });
		blueLight->SetPosition(Vec3{ sinf(xOffset+0.5f)*20.0f, 5.0, cosf(yOffset+0.5f)*20.0f });

		angleOffset += 0.01f;

		//Quat lampPitch, lampRoll;
		//lampRoll.FromRotation(sinf(angleOffset)*60.0, { 0.0, 0.0, 1.0 });
		//lampPitch.FromRotation(90.0, { 1.0, 0.0, 0.0 });
		//whiteSpotLight->SetRotation(lampRoll*lampPitch);

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