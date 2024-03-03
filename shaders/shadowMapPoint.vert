#version 450
#pragma vscode_glsllint_stage : vert

#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_scalar_block_layout : enable
#extension GL_ARB_shading_language_include : enable
#extension GL_GOOGLE_include_directive : enable

#extension GL_EXT_shader_explicit_arithmetic_types_int64 : require


#include "deviceDataStruct.h"

layout(binding = eLight,std140) uniform _LightUniforms {LightUniforms lightsUni;};

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec3 aColor;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec3 aBitangent;
layout(location = 5) in vec2 aTexCoords;

layout(location = 0) out vec3 TexCoords;

layout( push_constant ) uniform _PushConstantRaster
{
    PushConstantRaster pcRaster;
};


void main()
{

    mat4 lightSpaceMatrix = lightsUni.lights[0].dirLightSpaceMatrix;
    gl_Position = pcRaster.modelMatrix * vec4(aPos, 1.0);
}