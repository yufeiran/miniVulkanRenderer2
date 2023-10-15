# CMakeList.txt : CMake project for miniVulkanRender2 , include source and define 
# project specific logic here.
#

cmake_minimum_required( VERSION 3.20)

set(CMAKE_TOOLCHAIN_FILE C:/src/vcpkg/scripts/buildsystems/vcpkg.cmake)

project("miniVulkanRenderer2")

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

#设置多线程编译
if (MSVC)
add_definitions("/MP")
endif()


set(CMAKE_CXX_STANDARD 17)

find_package(Vulkan REQUIRED)


add_subdirectory(third_party)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")

add_subdirectory(src)

set_property ( DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT miniVulkanRenderer2 )

find_program(GLSL_VALIDATOR glslangValidator HINTS /usr/bin /usr/local/bin $ENV{VULKAN_SDK}/Bin/ $ENV{VULKAN_SDK}/Bin32/)
## find all the shader files under the shaders folder
file(GLOB_RECURSE GLSL_SOURCE_FILES
    "${PROJECT_SOURCE_DIR}/shaders/*.frag"
    "${PROJECT_SOURCE_DIR}/shaders/*.vert"
    "${PROJECT_SOURCE_DIR}/shaders/*.comp"
    )

## iterate each shader
foreach(GLSL ${GLSL_SOURCE_FILES})
  message(STATUS "BUILDING SHADER")
  get_filename_component(FILE_NAME ${GLSL} NAME)
  set(SPIRV "${PROJECT_SOURCE_DIR}/shaders/${FILE_NAME}.spv")
  message(STATUS ${GLSL})
  ##execute glslang command to compile that specific shader
  add_custom_command(
    OUTPUT ${SPIRV}
    COMMAND ${GLSL_VALIDATOR} -V ${GLSL} -o ${SPIRV}
    DEPENDS ${GLSL})
  list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach(GLSL)


  add_custom_target(
      Shaders 
      DEPENDS ${SPIRV_BINARY_FILES}
  )

 
