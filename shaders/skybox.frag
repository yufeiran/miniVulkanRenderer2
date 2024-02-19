#version 460 
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_scalar_block_layout : enable
#extension GL_ARB_shading_language_include : enable
#extension GL_GOOGLE_include_directive : enable

#extension GL_EXT_shader_explicit_arithmetic_types_int64 : require


#include "deviceDataStruct.h"

layout(location = 0) out vec4 FragColor;

layout(location = 0) in vec3 TexCoords;

layout(set = eObjDescs, binding = eCubeMap) uniform samplerCube cubeMapTexture;

void main()
{
    FragColor = texture(cubeMapTexture, TexCoords);
}