
#version 450

#extension GL_ARB_separate_shader_objects : enable
#extension GL_GOOGLE_include_directive: enable
#extension GL_EXT_scalar_block_layout : enable

#pragma vscode_glsllint_stage : vert

#extension GL_EXT_shader_explicit_arithmetic_types_int64 : require

#include "deviceDataStruct.h"

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inColor;
layout(location = 3) in vec2 inTexCoord;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;

layout( push_constant ) uniform _PushConstantRaster
{
    PushConstantRaster pcRaster;
    
};

layout(binding=0) uniform _GlobalUniforms{
    GlobalUniforms uni;
};


void main(){
    gl_Position = uni.viewProj * pcRaster.modelMatrix * vec4(inPosition,1.0);
    fragColor = inColor;
    fragTexCoord = inTexCoord;

}