# Add source to this project's executable.

set(RENDERER_FILES
    main.cpp
    miniVulkanRenderer.h 
    miniVulkanRenderer.cpp
)


file(GLOB_RECURSE VULKAN_FRAMEWORK_FILES CONFIGURE_DEPENDS
    "./Vulkan/*.h"
    "./Vulkan/*.cpp"
)

file(GLOB_RECURSE COMMON_FILES CONFIGURE_DEPENDS
    "./Common/*.h"
    "./Common/*.cpp"
)

file(GLOB_RECURSE PLATFORM_FILES CONFIGURE_DEPENDS
    "./Platform/*.h"
    "./Platform/*.cpp"
)


file(GLOB_RECURSE RENDERING_FILES CONFIGURE_DEPENDS
    "./Rendering/*.h"
    "./Rendering/*.cpp"
)


file(GLOB_RECURSE RESOURCE_MANAGEMENT_FILES CONFIGURE_DEPENDS
    "./ResourceManagement/*.h"
    "./ResourceManagement/*.cpp"
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
if (UNIX)
    set(VOLK_STATIC_DEFINES VK_USE_PLATFORM_XCB_KHR)
endif()

target_include_directories(miniVulkanRenderer2 PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(miniVulkanRenderer2 glm tinyobjloader tinygltf imgui stb_image volk )

target_link_libraries(miniVulkanRenderer2 Vulkan::Vulkan glfw)

add_dependencies(miniVulkanRenderer2 Shaders)