find_package(Vulkan REQUIRED)
find_package(glfw3  REQUIRED)


add_library(glm INTERFACE)
add_library(stb_image INTERFACE)
add_library(tinyobjloader STATIC)


# header only lib
target_include_directories(glm INTERFACE glm)
target_include_directories(stb_image INTERFACE stb_image)

# tinyobj
target_sources(tinyobjloader PRIVATE 
    tinyobjloader/tiny_obj_loader.h
    tinyobjloader/tiny_obj_loader.cc
    )

target_include_directories(tinyobjloader PUBLIC tinyobjloader)

set_property(TARGET tinyobjloader PROPERTY FOLDER third_party )

# imgui
add_definitions(-DVK_NO_PROTOTYPES)
add_library(imgui STATIC)
target_include_directories(imgui PUBLIC imgui imgui/backends)
target_sources(imgui PRIVATE
    imgui/imgui.h
    imgui/backends/imgui_impl_vulkan.h
    imgui/backends/imgui_impl_glfw.h
    imgui/imgui.cpp

    imgui/imgui_demo.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_widgets.cpp
    imgui/imgui_tables.cpp

    imgui/backends/imgui_impl_vulkan.cpp
    imgui/backends/imgui_impl_glfw.cpp

)
target_link_libraries(imgui PUBLIC Vulkan::Vulkan glfw)
set_property(TARGET imgui PROPERTY FOLDER third_party )

# volk
add_library(volk STATIC)
if (WIN32)
   set(VOLK_STATIC_DEFINES VK_USE_PLATFORM_WIN32_KHR)
endif()
target_link_libraries(volk PUBLIC Vulkan::Vulkan)
target_include_directories(volk PUBLIC volk)
target_sources(volk PRIVATE 
    volk/volk.h
    volk/volk.c
)
set_property(TARGET volk PROPERTY FOLDER third_party )