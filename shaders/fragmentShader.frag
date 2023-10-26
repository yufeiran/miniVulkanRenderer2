#version 450 
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : enable
#extension GL_GOOGLE_include_directive : enable
#extension GL_EXT_scalar_block_layout : enable

#extension GL_EXT_shader_explicit_arithmetic_types_int64 : require
#extension GL_EXT_buffer_reference2 : require

#include "deviceDataStruct.h"

layout(location = 0) out vec4 outColor;

layout(location = 0) in vec3 fragColor; 
layout(location = 1) in vec2 fragTexCoord;

layout(binding = 1)uniform sampler2D texSampler;


void main(){
    outColor = texture(texSampler,fragTexCoord);
}