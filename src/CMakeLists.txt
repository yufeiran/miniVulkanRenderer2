# Add source to this project's executable.

set(RENDERER_FILES
    main.cpp
    miniVulkanRenderer.h 
    miniVulkanRenderer.cpp
)


set(VULKAN_FRAMEWORK_FILES
    ./Vulkan/device.h
    ./Vulkan/device.cpp
    ./Vulkan/instance.h
    ./Vulkan/instance.cpp
    ./Vulkan/physicalDevice.h
    ./Vulkan/physicalDevice.cpp
    ./Vulkan/queue.h 
    ./Vulkan/queue.cpp 
    ./Vulkan/commandBuffer.h 
    ./Vulkan/commandBuffer.cpp
    ./Vulkan/commandPool.h 
    ./Vulkan/commandPool.cpp 
    ./Vulkan/swapchain.h 
    ./Vulkan/swapchain.cpp 
    ./Vulkan/imageView.h 
    ./Vulkan/imageView.cpp 
    ./Vulkan/image.h
    ./Vulkan/image.cpp
    ./Vulkan/shaderModule.h 
    ./Vulkan/shaderModule.cpp 
    ./Vulkan/graphicsPipeline.h 
    ./Vulkan/graphicsPipeline.cpp 
    ./Vulkan/framebuffer.h 
    ./Vulkan/framebuffer.cpp
    ./Vulkan/semaphore.h 
    ./Vulkan/semaphore.cpp 
    ./Vulkan/fence.h 
    ./Vulkan/fence.cpp 
    ./Vulkan/buffer.cpp 
    ./Vulkan/buffer.h
    ./Vulkan/deviceMemory.h 
    ./Vulkan/deviceMemory.cpp 
    ./Vulkan/descriptorSet.h 
    ./Vulkan/descriptorSet.cpp 
    ./Vulkan/descriptorSetLayout.h 
    ./Vulkan/descriptorSetLayout.cpp 
    ./Vulkan/descriptorPool.h 
    ./Vulkan/descriptorPool.cpp 
    ./Vulkan/sampler.h 
    ./Vulkan/sampler.cpp 
    ./Vulkan/shaderInfo.h 
    ./Vulkan/shaderInfo.cpp 
    ./Vulkan/pipelineLayout.h 
    ./Vulkan/pipelineLayout.cpp
    ./Vulkan/renderPass.h 
    ./Vulkan/renderPass.cpp 
    ./Vulkan/rayTracingPipeline.h 
    ./Vulkan/rayTracingPipeline.cpp 
    ./Vulkan/queryPool.h 
    ./Vulkan/queryPool.cpp 
    ./Vulkan/descriptorSetBindings.h 
    ./Vulkan/descriptorSetBindings.cpp 
    ./Vulkan/texture.h 
    ./Vulkan/texture.cpp

)

set(COMMON_FILES
    ./Common/miniLog.h 
    ./Common/miniLog.cpp 
    ./Common/miniError.h
    ./Common/material.h
    ./Common/material.cpp 
    ./Common/Vertex.h 
    ./Common/Vertex.cpp 
    ./Common/camera.h 
    ./Common/camera.cpp 
    ./Common/common.h
    ./Common/common.cpp
    ./Common/light.h 
    ./Common/light.cpp
)

set(PLATFORM_FILES
    ./Platform/platform.h 
    ./Platform/platform.cpp 
    ./Platform/GUIWindow.h 
    ./Platform/GUIWindow.cpp 

)

set(RENDERING_FILES 
    ./Rendering/pipeline.h 
    ./Rendering/pipeline.cpp
    ./Rendering/renderContext.h 
    ./Rendering/renderContext.cpp 
    ./Rendering/renderTarget.h 
    ./Rendering/renderTarget.cpp 
    ./Rendering/renderFrame.h 
    ./Rendering/renderFrame.cpp 
    ./Rendering/postQuad.h 
    ./Rendering/postQuad.cpp
    ./Rendering/graphicsRenderPass.h 
    ./Rendering/graphicsRenderPass.cpp
    ./Rendering/skyLightCube.h 
    ./Rendering/skyLightCube.cpp
    ./Rendering/graphicsPipelineBuilder.h
    ./Rendering/graphicsPipelineBuilder.cpp
    ./Rendering/rayTracingBuilder.h 
    ./Rendering/rayTracingBuilder.cpp 
    ./Rendering/shadowPipelineBuilder.h 
    ./Rendering/shadowPipelineBuilder.cpp 
    ./Rendering/ssaoPipelineBuilder.h
    ./Rendering/ssaoPipelineBuilder.cpp
    ./Rendering/pbbloomPipelineBuilder.h
    ./Rendering/pbbloomPipelineBuilder.cpp
    ./Rendering/SSRPipelineBuilder.h
    ./Rendering/SSRPipelineBuilder.cpp
    ./Rendering/makeCubeMapPipeline.h
    ./Rendering/makeCubeMapPipeline.cpp
)

set(RESOURCE_MANAGEMENT_FILES
    ./ResourceManagement/ResourceManager.h 
    ./ResourceManagement/ResourceManager.cpp 
    ./ResourceManagement/objLoader.h 
    ./ResourceManagement/objLoader.cpp 
    ./ResourceManagement/gltfLoader.h 
    ./ResourceManagement/gltfLoader.cpp
    ./ResourceManagement/boundingBox.h

)

set(SHADER_HEADER 
    ../shaders/deviceDataStruct.h)

source_group("\\" FILES ${RENDERER_FILES})
source_group("Common\\" FILES ${COMMON_FILES})
source_group("Platform\\" FILES ${PLATFORM_FILES})
source_group("Vulkan\\" FILES ${VULKAN_FRAMEWORK_FILES})
source_group("Rendering\\" FILES ${RENDERING_FILES})
source_group("ResourceManagement\\" FILES ${RESOURCE_MANAGEMENT_FILES})
source_group("ShaderHeader\\" FILES ${SHADER_HEADER})


# Add source to this project's executable.
add_executable(miniVulkanRenderer2
    ${RENDERER_FILES}
    ${COMMON_FILES}
    ${VULKAN_FRAMEWORK_FILES}
    ${PLATFORM_FILES}
    ${RENDERING_FILES}
    ${RESOURCE_MANAGEMENT_FILES}
    ${SHADER_HEADER}
)



find_package(glfw3 REQUIRED)
set_property(TARGET miniVulkanRenderer2 PROPERTY VS_DEBUGGER_WORKING_DIRECTORY 
"$<TARGET_FILE_DIR:miniVulkanRenderer2>")

if (WIN32)
   set(VOLK_STATIC_DEFINES VK_USE_PLATFORM_WIN32_KHR)
endif()

target_include_directories(miniVulkanRenderer2 PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(miniVulkanRenderer2 glm tinyobjloader tinygltf imgui stb_image volk )

target_link_libraries(miniVulkanRenderer2 Vulkan::Vulkan glfw)

add_dependencies(miniVulkanRenderer2 Shaders)