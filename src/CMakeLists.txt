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
    ./Vulkan/vk_common.h
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

)

set(COMMON_FILES
    ./Common/miniLog.h 
    ./Common/miniLog.cpp 
    ./Common/miniError.h
)

set(PLATFORM_FILES
    ./Platform/platform.h 
    ./Platform/platform.cpp 
    ./Platform/glfwWindow.h 
    ./Platform/glfwWindow.cpp 

)

set(RENDERING_FILES 
    ./Rendering/renderContext.h 
    ./Rendering/renderContext.cpp 
    ./Rendering/renderTarget.h 
    ./Rendering/renderTarget.cpp 
    ./Rendering/renderFrame.h 
    ./Rendering/renderFrame.cpp 
)

source_group("\\" FILES ${RENDERER_FILES})
source_group("Common\\" FILES ${COMMON_FILES})
source_group("Platform\\" FILES ${PLATFORM_FILES})
source_group("Vulkan\\" FILES ${VULKAN_FRAMEWORK_FILES})
source_group("Rendering\\" FILES ${RENDERING_FILES})


# Add source to this project's executable.
add_executable(miniVulkanRenderer2
    ${RENDERER_FILES}
    ${COMMON_FILES}
    ${VULKAN_FRAMEWORK_FILES}
    ${PLATFORM_FILES}
    ${RENDERING_FILES}
)



find_package(glfw3 REQUIRED)
set_property(TARGET miniVulkanRenderer2 PROPERTY VS_DEBUGGER_WORKING_DIRECTORY 
"$<TARGET_FILE_DIR:miniVulkanRenderer2>")

if (WIN32)
   set(VOLK_STATIC_DEFINES VK_USE_PLATFORM_WIN32_KHR)
endif()

target_include_directories(miniVulkanRenderer2 PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(miniVulkanRenderer2 glm tinyobjloader imgui stb_image volk)

target_link_libraries(miniVulkanRenderer2 Vulkan::Vulkan glfw)

add_dependencies(miniVulkanRenderer2 Shaders)