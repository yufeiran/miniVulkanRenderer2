#version 450

#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : enable
#extension GL_GOOGLE_include_directive : enable
#extension GL_EXT_scalar_block_layout : enable

#extension GL_EXT_shader_explicit_arithmetic_types_int64 : require
#extension GL_EXT_buffer_reference2 : require

#extension GL_ARB_shader_clock : enable

#pragma vscode_glsllint_stage : vert    

#include "deviceDataStruct.h"



layout(location = 0) in vec2 TexCoords;

layout(location = 0) out vec4 FragColor;

layout(binding = 1) uniform sampler2D offscreenColor;

layout( push_constant ) uniform _PushConstantPost
{
    PushConstantPost pcPost;
};



void main()
{
    const float gamma = 2.2;

    vec3 color = texture(offscreenColor,TexCoords).rgb;

    // exposure tone mapping
    vec3 mapped = vec3(1.0) - exp(-color * pcPost.exposure);

    mapped = pow(mapped, vec3(1.0 / gamma));

    FragColor = vec4(mapped,1.0);
}