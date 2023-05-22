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
)

set(COMMON_FILES
    ./Common/miniLog.h 
    ./Common/miniLog.cpp 
    ./Common/miniError.h
)

source_group("\\" FILES ${RENDERER_FILES})
source_group("Common\\" FILES ${COMMON_FILES})
source_group("Vulkan\\" FILES ${VULKAN_FRAMEWORK_FILES})

# Add source to this project's executable.
add_executable(miniVulkanRenderer2
    ${RENDERER_FILES}
    ${COMMON_FILES}
    ${VULKAN_FRAMEWORK_FILES}

)



find_package(glfw3 REQUIRED)
set_property(TARGET miniVulkanRenderer2 PROPERTY VS_DEBUGGER_WORKING_DIRECTORY 
"$<TARGET_FILE_DIR:miniVulkanRenderer2>")

target_include_directories(miniVulkanRenderer2 PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(miniVulkanRenderer2 glm tinyobjloader imgui stb_image volk)

target_link_libraries(miniVulkanRenderer2 Vulkan::Vulkan glfw)

add_dependencies(miniVulkanRenderer2 Shaders)