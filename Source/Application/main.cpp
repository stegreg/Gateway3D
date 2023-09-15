#include "../Gateway/RefCounted.h"

#include "../Gateway/Device.h"
#include "../Gateway/Context.h"
#include "../Gateway/Window.h"
#include "../Gateway/Keyboard.h"
#include "../Gateway/Mouse.h"
#include "../Gateway/RenderPath.h"
#include "../Gateway/Shader.h"
#include "../Gateway/Material.h"

#include "../Gateway/Vec2.h"
#include "../Gateway/Vec3.h"
#include "../Gateway/Vec4.h"
#include "../Gateway/FileSystem.h"
#include "../Gateway/AmbientLight.h"
#include "../Gateway/DirectionalLight.h"
#include "../Gateway/PointLight.h"
#include "../Gateway/SpotLight.h"
#include "../Gateway/Texture.h"
#include "../Gateway/System.h"
#include "../Gateway/SimObject.h"
#include "../Gateway/Camera.h"
#include "../Gateway/PostProcess.h"
#include "../Gateway/Model.h"
#include "../Gateway/ModelInstance.h"
#include "../Gateway/MultiModelInstance.h"
#include "../Gateway/World.h"
#include "../Gateway/TextDocument.h"
#include "../Gateway/UI.h"
#include "../Gateway/UICanvas.h"
#include "../Gateway/SystemUI.h"

#include <tracy/Tracy.hpp>

#include <iostream>

using namespace Gateway;

int main()
{
	Ref<Device> device = Device::Create();

	Ref<RenderWindow>  window = RenderWindow::Create("MyApplication", 1920, 1080, 0, 0, WINDOW_CENTERED);

	Ref<Keyboard> keyboard = Keyboard::Create(window);

	Ref<Mouse> mouse = Mouse::Create(window);

	Mouse::SetRelativeMode(true);

	Ref<Context> context = Context::Create(window);

	Ref<SystemUI> imgui = SystemUI::Create(window, context);

	Ref<UI> ui = UI::Create();

	//Ref<World>	 world = World::Load("Worlds/MyWorld_02.world");
	Ref<World> world = World::Create();

	Ref<RenderPath> renderPath = RenderPath::Create();

	//Ref<Camera> camera = world->GetSimObjectByName("MyMainCamera").Cast<Camera>();

	Ref<Camera> camera = Camera::Create(world, "MyMainCamera");
	camera->SetFOV(70.0);
	camera->SetPosition(0.0, 5.0, 0.0);
	camera->SetClearColor({ 0.1, 0.2, 0.3, 1.0 });
	camera->SetRenderTarget(window);

	Ref<Material>	 postProcessMaterial = Material::Load("Materials/MyPostProcess.material");
	postProcessMaterial->SetTexture("uColorTexture", renderPath->GetColorTexture(), 0);
	Ref<PostProcess> postProcess = PostProcess::Create();
	postProcess->SetMaterial(postProcessMaterial);
		
	camera->ChainPostProcess(postProcess);

	//Ref<TextDocument> rmlDocument = TextDocument::Load("UI/test.rml");
	//Ref<UICanvas> canvas = UICanvas::Create(window, world);
	//canvas->SetRmlDocument(rmlDocument);


	Quat sunRot;
	sunRot.FromRotation(45.0, { 1.0, 0.0, 0.0 });
	
	//Ref<DirectionalLight> sun = DirectionalLight::Create(world);
	//sun->SetRotation(sunRot);
	//sun->SetColor({ 0.8, 0.7, 0.6});
	//sun->SetIntensity(1.0);

	//const iVec2 gridSize = { 4, 8 };
	//for ( int x = -gridSize.x; x<gridSize.x; x++ )
	//{
	//	for ( int y = -gridSize.y; y<gridSize.y; y++ )
	//	{
	//		Ref<PointLight> pointLight = PointLight::Create(world);
	//		pointLight->SetPosition(-40 + x * 50.0, 2.0, 20 + y * 50.0);

	//		float r = ((float)std::rand())/RAND_MAX + 0.2;
	//		float g = ((float)std::rand())/RAND_MAX + 0.2;
	//		float b = ((float)std::rand())/RAND_MAX + 0.2;

	//		pointLight->SetColor({ r, g, b });
	//		pointLight->SetRange(20.0f);
	//		pointLight->SetIntensity(100);
	//		pointLight->SetFalloffFactor(0.0);

	//	}
	//}

	//Ref<PointLight> pointLight2 = PointLight::Create(world);
	//pointLight2->SetPosition(-60.0, 1.0, 15.0);
	//pointLight2->SetRange(10.0f);
	//pointLight2->SetIntensity(10);
	//pointLight2->SetColor({ 1.0, 1.0, 1.0 });
	//pointLight2->SetFalloffFactor(0.9);

	Ref<PointLight> pointLight1 = PointLight::Create(world);
	pointLight1->SetPosition(0.0, 5.0, 0.0);
	pointLight1->SetColor({ 1.0, 1.0, 1.0 });
	pointLight1->SetRange(400.0f);
	pointLight1->SetIntensity(20);
	pointLight1->SetFalloffFactor(0.3);


	//Ref<PointLight> pointLight3 = PointLight::Create(world);
	//pointLight3->SetPosition(-15.0, 1.0, 0.0);
	//pointLight3->SetRange(10.0f);
	//pointLight3->SetIntensity(1);
	//pointLight3->SetColor({ 1.0, 1.0, 1.0 });
	//pointLight3->SetFalloffFactor(0.0);

	//Ref<PointLight> pointLight4 = PointLight::Create(world);
	//pointLight4->SetPosition(-15.0, 1.0, -15.0);
	//pointLight4->SetColor({ 0.0, 1.0, 1.0 });
	//pointLight4->SetRange(10.0f);
	//pointLight4->SetIntensity(1);
	//pointLight4->SetFalloffFactor(0.0);

	//Ref<SpotLight> spotLight1 = SpotLight::Create(world);
	//spotLight1->SetPosition(0.0, 5.0, 0.0);
	//spotLight1->SetRotation(sunRot);
	//spotLight1->SetRange(30.0);	
	//spotLight1->SetOuterAngle(50.0);
	//spotLight1->SetInnerAngle(40.0);
	//spotLight1->SetIntensity(5);
	//spotLight1->SetFalloffFactor(0.0);

	Model::Import("Models/Monkey.glb");
	Model::Import("Models/Plane.glb");
	Texture::Import("Textures/Concrete/042C/BaseColor.jpg");
	Texture::Import("Textures/Concrete/042C/Roughness.jpg");
	Texture::Import("Textures/Concrete/042C/Normal.jpg");
	Texture::Import("Textures/Concrete/042C/Displacement.jpg");
	Texture::Import("Textures/Concrete/042C/Metallic.jpg");

	Ref<Material> floorMaterial = Material::Load("Materials/Floor.material");
	//Ref<Material> monkeyMaterial = Material::Load("Materials/BlueSphere.material");
	//Ref<Model> monkeyModel = Model::Load("Models/Monkey.model");
	//Ref<ModelInstance> monkeyInstance1 = ModelInstance::Create(world);
	//monkeyInstance1->SetModel(monkeyModel);
	//monkeyInstance1->SetOverrideMaterial(monkeyMaterial);
	//monkeyInstance1->SetPosition(0.0, 1.0, 5.0);

	Ref<Model> floorModel = Model::Load("Models/Plane.model");
	Ref<ModelInstance> floorInstance = ModelInstance::Create(world);
	floorInstance->SetModel(floorModel);
	floorInstance->SetOverrideMaterial(floorMaterial);
	
	float heightScale = 0.0;

	float camRotYAmount = 0.0f;
	float camRotXAmount = 0.0f;
	iVec2 lastMousePos;
	bool uiVisible = true;
	bool run = true;
	while ( run )
	{
		ZoneScopedN("Game::Run()");

		window->PollEvents();

		ImGuiIO& io = ImGui::GetIO();

		if ( keyboard->IsKeyDown(KEY_ESCAPE) || window->IsClosed() || io.KeysDown[ImGuiKey_Escape] )
			run = false;

		if ( io.WantCaptureMouse||io.WantCaptureKeyboard )
		{
			keyboard->Flush();
			mouse->Flush();
		}

		if ( mouse->IsButtonPressed(MOUSE_RIGHT) )
		{
			lastMousePos = mouse->GetLocalPosition();
		}

		if ( mouse->IsButtonDown(MOUSE_RIGHT) )
		{
			Mouse::SetRelativeMode(true);

			Vec3 camForward = camera->GetForward();
			Vec3 camRight = camera->GetRight();

			float speed = 0.4f;
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

			camRotXAmount += (float)( mouseRelativeMotion.y )*0.1f;
			camRotYAmount += (float)( mouseRelativeMotion.x )*0.1f;

			Quat camRotY, camRotX;
			camRotY.FromRotation(camRotYAmount, { 0.0, 1.0, 0.0 });
			camRotX.FromRotation(camRotXAmount, { 1.0, 0.0, 0.0 });

			camera->SetRotation(camRotY*camRotX);
		}
		else
		{
			Mouse::SetRelativeMode(false);
		}
		if ( mouse->IsButtonReleased(MOUSE_RIGHT) )
		{
			mouse->SetLocalPosition(lastMousePos);
		}

		if ( keyboard->IsKeyPressed(KEY_F1) )
		{
			uiVisible = !uiVisible;

			//if ( canvas->GetRmlRoot()->IsVisible() )
			//	canvas->GetRmlRoot()->Hide();
			//else
			//	canvas->GetRmlRoot()->Show();
		}

		floorMaterial->SetConstant("uHeightScale", heightScale);

		{
			ZoneScopedN("World::Update()");
			world->Update();
		}

		{
			renderPath->Execute(world);
		}
		imgui->Begin();

		ImGui::DockSpaceOverViewport(0, ImGuiDockNodeFlags_PassthruCentralNode);

		if ( !ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow)&&
			!ImGui::IsAnyItemHovered()&&
			!ImGui::IsAnyItemActive()&&
			!ImGui::IsPopupOpen(NULL, ImGuiPopupFlags_AnyPopup) )
		{
			ImGui::SetWindowFocus(NULL);
		}

		ImGui::Begin("Surface Parameters");

		ImGui::LabelText("Camera Position:", "[%f, %f, %f]",
						 camera->GetPosition().x,
						 camera->GetPosition().y,
						 camera->GetPosition().z
		);

		ImGui::SliderFloat("Height Scale", &heightScale, 0.0, 5.0, "%.6f");

		ImGui::End();

		imgui->End();

		window->Present(true);

		FrameMark;
	}
}