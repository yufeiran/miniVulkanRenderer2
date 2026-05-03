#pragma once

#include<imgui.h>
#include<Common/common.h>
#include<Common/camera.h>
#include<Vulkan/physicalDevice.h>
#include<Vulkan/device.h>
#include<Vulkan/instance.h>
#include<Vulkan/descriptorPool.h>
#include<Vulkan/commandBuffer.h>
#include<Vulkan/commandPool.h>
#include<Vulkan/renderPass.h>
#include<Vulkan/swapchain.h>
#include "Common/hostDataStruct.h"
#include "Common/Light.h"
#include "ResourceManagement/ResourceManager.h"




namespace mini
{
	class UI
	{
	public:
		

		void initImGui(const Instance& instance,
			const PhysicalDevice &phyDevice, 
			Device& device,
			const Swapchain &swapchain,
			const RenderPass& postRenderPass);

		// ui ---------------------
		bool renderUI(std::vector<VkClearValue>& clearValues, VkExtent2D screenSize, bool sizeChange, bool& lightSizeChange, bool& useRaytracing, 
			PushConstantRay &pcRay,PushConstantRaster& pcRaster, PushConstantPost &pcPost,Camera& camera, std::vector<Light>& lights, ResourceManager& rm);
		bool uiLights(VkExtent2D screenSize, bool sizeChange, bool& lightSizeChange, std::vector<Light>& lights, ResourceManager& rm, 
			PushConstantRay& pcRay, PushConstantRaster& pcRaster, PushConstantPost& pcPost);
		bool uiSettings(VkExtent2D screenSize, bool sizeChange);
		bool uiInstance(VkExtent2D screenSize, bool sizeChange, std::vector<Light>& lights, ResourceManager& rm);
	private:
		std::unique_ptr<DescriptorPool> imguiDescPool;
		std::unique_ptr<CommandPool> tempCommandPool;



	};
}

