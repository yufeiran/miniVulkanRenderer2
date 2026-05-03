#include"UI.h"
#include <Vulkan/descriptorPool.h>
#include "imgui.h"
#include "imgui_impl_vulkan.h"
#include "imgui_impl_glfw.h"

namespace mini
{

	void UI::initImGui(const Instance& instance, const PhysicalDevice& phyDevice, Device&device, const Swapchain& swapchain,const RenderPass& postRenderPass)
	{
		if (tempCommandPool == nullptr)
		{
			tempCommandPool = std::make_unique<CommandPool>(device);
		}
		

		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.IniFilename = nullptr; // Avoiding the INI file
		io.LogFilename = nullptr;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
		io.FontGlobalScale = 1.5f;

		ImGuiStyle& style = ImGui::GetStyle();

		ImVec4 TETSU = ImVec4(38.f / 255.f, 69.f / 255.f, 61.f / 255.f, 0.9f);
		ImVec4 AIMIRUCHA = ImVec4(15.f / 255.f, 76.f / 255.f, 58.f / 255.f, 1.0f);
		ImVec4 KAMENOZOKI = ImVec4(165.f / 255.f, 222.f / 255.f, 228.f / 255.f, 1.0f);

		//// set background color
		//style.Colors[ImGuiCol_WindowBg] = TETSU;

		//// set text color
		//style.Colors[ImGuiCol_Text]     = KAMENOZOKI;

		//// set button background color 
		//style.Colors[ImGuiCol_Button]   = AIMIRUCHA;


		imguiDescPool = std::make_unique<DescriptorPool>(device);

		ImGui_ImplVulkan_InitInfo init_info = {};
		init_info.Instance = instance.getHandle();
		init_info.PhysicalDevice = phyDevice.getHandle();
		init_info.Device = device.getHandle();
		init_info.QueueFamily = device.getGraphicQueue().getFamilyIndex();
		init_info.Queue = device.getGraphicQueue().getHandle();
		init_info.PipelineCache = VK_NULL_HANDLE;
		init_info.DescriptorPool = imguiDescPool->getHandle();
		init_info.Subpass = 0;
		init_info.MinImageCount = 2;
		init_info.ImageCount = static_cast<int>(swapchain.getImageCount());
		init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
		init_info.CheckVkResultFn = nullptr;
		init_info.Allocator = nullptr;

		ImGui_ImplVulkan_LoadFunctions(
			[](const char* function_name, void* vulkan_instance) {
				return vkGetInstanceProcAddr(*(reinterpret_cast<VkInstance*>(vulkan_instance)), function_name); }, &init_info.Instance);

		ImGui_ImplVulkan_Init(&init_info, postRenderPass.getHandle());

		auto tempCmd = tempCommandPool->createCommandBuffer();
		tempCmd->beginSingleTime();
		ImGui_ImplVulkan_CreateFontsTexture(tempCmd->getHandle());
		tempCmd->endSingleTime(device.getGraphicQueue());
	}


	bool UI::renderUI(std::vector<VkClearValue>& clearValues, VkExtent2D screenSize, bool sizeChange, bool& lightSizeChange,bool& useRaytracing,
		PushConstantRay& pcRay, PushConstantRaster& pcRaster, PushConstantPost& pcPost,Camera &camera, std::vector<Light>& lights, ResourceManager& rm)
	{
		static ImGuiTreeNodeFlags_ isLightHeaderOpen = ImGuiTreeNodeFlags_DefaultOpen;
		static ImGuiTreeNodeFlags_ isRenderingHeaderOpen = ImGuiTreeNodeFlags_DefaultOpen;

		static bool debugSSR = false;

		static int debugModeIndex = 0;

		static bool needSSAO = true;
		bool changed = false;

		ImGui::SetNextWindowSize(ImVec2(500, 0));
		ImGui::Begin("Setting");
		changed |= ImGui::ColorEdit3("clearColor", (float*)(&(clearValues[0].color)));
		changed |= ImGui::Checkbox("Ray Tracer mode", &useRaytracing);
		pcPost.raytraceMode = useRaytracing;


		if (ImGui::CollapsingHeader("Rendering", ImGuiTreeNodeFlags_DefaultOpen))
		{
			changed |= ImGui::SliderInt("Max Frames", &maxFrames, 1, MAX_FRAMES_LIMIT);
			changed |= ImGui::SliderInt("Sample Number", &pcRay.nbSample, 1, 10);
			changed |= ImGui::SliderInt("Max Depth", &pcRay.maxDepth, 1, 20);
			ImGui::Text("Now Frame %d ", pcRay.frame);

			changed |= ImGui::Checkbox("SSAO", &needSSAO);
			if (needSSAO == false)
			{
				pcRaster.needSSAO = 0;
			}
			else {
				pcRaster.needSSAO = 1;
			}


		}
		if (ImGui::CollapsingHeader("Shadow", ImGuiTreeNodeFlags_DefaultOpen))
		{
			const int SHADOW_MODE_SUM = 3;
			const char* SHADOW_MODE_STR[SHADOW_MODE_SUM] = {
				"normal",
				"PCF",
				"PCSS"
			};

			changed |= ImGui::Combo("shadow mode", &pcRaster.shadowMode, SHADOW_MODE_STR, SHADOW_MODE_SUM);
			if (pcRaster.shadowMode == 2)
			{
				changed |= ImGui::SliderFloat("shadow light size", &pcRaster.shadowLightSize, 1, 60);
			}

		}

		if (ImGui::CollapsingHeader("Debug", ImGuiTreeNodeFlags_DefaultOpen))
		{
			const int DEBUG_MODE_SUM = 12;
			const char* DEBUG_MODE_STR[DEBUG_MODE_SUM] = {
				"no debug",
				"base color",
				"normal",
				"metallic",
				"emissive",
				"alpha",
				"roughness",
				"texcoord",
				"tangent",
				"bitangent",
				"specular",
				"SSAO"
			};
			changed |= ImGui::Combo("mode", &debugModeIndex, DEBUG_MODE_STR, DEBUG_MODE_SUM);
			pcRay.debugMode = debugModeIndex;
			pcRaster.debugMode = debugModeIndex;

		}
		if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::Text("Camera pos:%.1f %.1f %.1f , yaw %.1f pitch %.1f ", camera.getPos()[0], camera.getPos()[1], camera.getPos()[2],
				camera.getYaw(), camera.getPitch());
		}
		if (ImGui::CollapsingHeader("SSR", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::Checkbox("debugSSR", &debugSSR);
			pcPost.debugSSR = debugSSR;

			ImGui::SliderFloat("step", &pcRaster.ssrStep, 0.001, 10);

			ImGui::SliderFloat("maxDistance", &pcRaster.ssrMaxDistance, 0, 100);

			static int epsilonIndex = -4;
			ImGui::SliderInt("epsilon", &epsilonIndex, -5, -1);
			pcRaster.ssrEpsilon = pow(10, epsilonIndex);

			ImGui::SliderFloat("attenuation", &pcRaster.ssrAttenuation, 0.01, 3);

			ImGui::SliderFloat("intensity", &pcPost.ssrIntensity, 0.01, 3);

			static bool useBinarySearch = pcRaster.ssrUseBinarySearch;
			ImGui::Checkbox("useBinarySearch", &useBinarySearch);
			pcRaster.ssrUseBinarySearch = useBinarySearch;



		}

		if (ImGui::CollapsingHeader("Post", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::SliderFloat("exposure", &pcPost.exposure, 0, 10);

			static bool debugShadowMap = pcPost.debugShadowMap;
			ImGui::Checkbox("debugShadowMap", &debugShadowMap);
			pcPost.debugShadowMap = debugShadowMap;

			static bool debugBloom = pcPost.debugBloom;
			ImGui::Checkbox("debugBloom", &debugBloom);
			pcPost.debugBloom = debugBloom;



			ImGui::SliderFloat("bloomRadius", &pcPost.pbbloomRadius, 0, 0.1);
			ImGui::SliderFloat("bloomIntensity", &pcPost.pbbloomIntensity, 0, 0.3);
			static bool bloomMode = pcPost.pbbloomMode;
			ImGui::Checkbox("bloom", &bloomMode);
			pcPost.pbbloomMode = bloomMode;

		}


		ImGui::End();

		changed |= uiLights(screenSize, sizeChange, lightSizeChange,lights,rm,pcRay,pcRaster,pcPost);
		changed |= uiInstance(screenSize, sizeChange,lights,rm);
		changed |= uiSettings(screenSize, sizeChange);


		return changed;

	}

	bool UI::uiLights(VkExtent2D screenSize, bool sizeChange, bool& lightSizeChange, std::vector<Light>& lights, 
		ResourceManager& rm, PushConstantRay& pcRay, PushConstantRaster& pcRaster, PushConstantPost& pcPost)
	{
		bool changed = false;
		static bool init = true;

		int windowWidth = 500;
		int windowHeight = 1000;

		lightSizeChange = false;

		init = sizeChange;

		if (init == true)
		{
			ImGui::SetNextWindowPos(ImVec2(screenSize.width - windowWidth - 50, 50));
			ImGui::SetNextWindowSize(ImVec2(windowWidth, windowHeight));
			init = false;

		}

		//ImGui::ShowDemoWindow();

		//ImGui::SetNextWindowSize(ImVec2(700,0));

		ImGui::Begin("Light Setting");


		bool addChecked = ImGui::Button("add");

		if (addChecked)
		{

			addRandomLight(lights, rm);
			lightSizeChange = true;
		}

		for (int i = 0; i < lights.size(); i++)
		{
			ImGui::PushID(i);

			if (ImGui::CollapsingHeader(("Light" + std::to_string(i)).c_str(), ImGuiTreeNodeFlags_None))
			{
				auto& light = lights[i];

				auto pos = light.getPosition();
				changed |= ImGui::SliderFloat3("Position", &pos.x, -20.f, 20.f);
				light.setPosition(pos);
				int lightInstanceId = light.getInstanceId();
				if (lightInstanceId != -1)
				{
					auto& instance = rm.getInstances()[lightInstanceId];
					instance.translation = pos;
					instance.updateTransformByFactor();
				}

				auto intensity = light.getIntensity();
				changed |= ImGui::SliderFloat("Intensity", &intensity, 0.f, 10.f);
				light.setIntensity(intensity);

				int type = light.getType();
				changed |= ImGui::RadioButton("Point", &type, 0);
				ImGui::SameLine();

				changed |= ImGui::RadioButton("Infinite", &type, 1);
				light.setType(static_cast<LightType>(type));

				auto color = light.getColor();
				changed |= ImGui::ColorEdit3("Color", &color.x);
				light.setColor(color);

				if (ImGui::Button("remove"))
				{
					delLight(lights, rm, i);
					lightSizeChange = true;
					changed = true;
				}


			}
			ImGui::PopID();

		}


		if (ImGui::CollapsingHeader("Skylight", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::Text("Skylight");
			changed |= ImGui::SliderFloat("SkylightIntensity", &pcRay.skyLightIntensity, 0.f, 300.f);
			pcRaster.skyLightIntensity = pcRay.skyLightIntensity;
		}


		ImGui::End();

		return changed;
	}
	bool UI::uiSettings(VkExtent2D screenSize, bool sizeChange)
	{
		bool changed = false;
		static bool init = true;

		int windowWidth = 800;
		int windowHeight = 200;

		if (init == true)
		{
			ImGui::SetNextWindowPos(ImVec2(screenSize.width - windowWidth - 50, screenSize.height - windowHeight - 50));
			ImGui::SetNextWindowSize(ImVec2(windowWidth, windowHeight));

			init = false;
		}

		auto& gs = getGlobalMiniSetting();

		ImGui::Begin("Settings");
		{
			ImGui::Checkbox("SSR", &gs.ssr_on);
			ImGui::Checkbox("Bloom", &gs.bloom_on);
		}

		ImGui::End();

		return false;
	}
	bool UI::uiInstance(VkExtent2D screenSize, bool sizeChange, std::vector<Light>& lights, ResourceManager& rm)
	{
		bool changed = false;
		static bool init = true;

		init = sizeChange;

		int windowWidth = 800;
		int windowHeight = 200;

		static bool open = false;

		auto& instance = rm.getInstances();

		if (init == true)
		{
			ImGui::SetNextWindowPos(ImVec2(20, screenSize.height - windowHeight - 50));
			ImGui::SetNextWindowSize(ImVec2(windowWidth, windowHeight));

			init = false;
			if (instance.size() < 10)
			{
				open = true;

			}

		}

		ImGui::Begin("Instance", &open);


		for (int i = 0; i < instance.size(); i++)
		{
			ImGui::PushID(i);
			auto& inst = instance[i];
			if (ImGui::CollapsingHeader((std::to_string(i) + "_" + inst.name).c_str(), ImGuiTreeNodeFlags_None))
			{


				/*	glm::mat4& transform = inst.transform;
					glm::vec3 scale;
					glm::quat rotation;
					glm::vec3 translation;
					glm::vec3 skew;
					glm::vec4 perspective;
					glm::decompose(transform,scale,rotation,translation,skew,perspective);

					glm::mat4 rotationMat =  glm::mat4_cast(rotation);
					glm::vec3 eular =  glm::eulerAngles(rotation);*/

					//Log("pitch "+ std::to_string(glm::degrees(glm::pitch(rotation))) + 
					//	" yaw " + std::to_string(glm::degrees(glm::yaw(rotation))) + 
					//	" roll " + std::to_string(glm::degrees(glm::roll(rotation))));

				changed |= ImGui::SliderFloat3("Translation", &inst.translation.x, -20.f, 20.f);

				changed |= ImGui::SliderFloat3("Scale", &inst.scale.x, 0.1f, 10.f);



				changed |= ImGui::SliderAngle("rotateX", &inst.rotation.x);
				changed |= ImGui::SliderAngle("rotateY", &inst.rotation.y);
				changed |= ImGui::SliderAngle("rotateZ", &inst.rotation.z);

				inst.updateTransformByFactor();
				if (inst.type == INSTANCE_TYPE_LIGHT_CUBE)
				{
					int lightID = inst.lightIndex;
					if (inst.lightIndex != -1)
					{
						lights[lightID].setPosition(inst.translation);
					}


				}

				//glm::mat4 scaleMat = glm::scale(glm::mat4(1.0f),scale);


				////glm::mat4 rotationMatX = glm::rotate(glm::mat4(1.0f),eular.x,glm::vec3(1,0,0));
				////glm::mat4 rotationMatY = glm::rotate(glm::mat4(1.0f),eular.y,glm::vec3(0,1,0));
				////glm::mat4 rotationMatZ = glm::rotate(glm::mat4(1.0f),eular.z,glm::vec3(0,0,1));

				////rotationMat = rotationMatX * rotationMatY * rotationMatZ;

				//rotationMat = glm::eulerAngleXYZ(eular.x,eular.y,eular.z);
				//
				//glm::mat4 translationMat = glm::translate(glm::mat4(1.0f),translation);

				//transform = translationMat * rotationMat * scaleMat;


			}
			ImGui::PopID();

		}

		ImGui::End();

		return changed;
	}
}