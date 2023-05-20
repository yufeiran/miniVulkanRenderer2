# Add source to this project's executable.
add_executable(miniVulkanRender2
    main.cpp
)
find_package(glfw3 REQUIRED)
set_property(TARGET miniVulkanRender2 PROPERTY VS_DEBUGGER_WORKING_DIRECTORY 
"$<TARGET_FILE_DIR:miniVulkanRender2>")

target_include_directories(miniVulkanRender2 PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(miniVulkanRender2 glm tinyobjloader imgui stb_image)

target_link_libraries(miniVulkanRender2 Vulkan::Vulkan glfw)

add_dependencies(miniVulkanRender2 Shaders)