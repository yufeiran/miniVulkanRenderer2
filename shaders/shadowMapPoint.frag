#version 450
#pragma vscode_glsllint_stage : vert

#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_scalar_block_layout : enable
#extension GL_ARB_shading_language_include : enable
#extension GL_GOOGLE_include_directive : enable

#extension GL_EXT_shader_explicit_arithmetic_types_int64 : require


#include "deviceDataStruct.h"

layout(location = 0) in vec4 FragPos;

layout(binding = eLight,std140) uniform _LightUniforms {LightUniforms lightsUni;};


void main()
{
    vec3 lightPos = lightsUni.lights[0].position.xyz;
    float lightDistance = length(lightPos - FragPos.xyz);

    float farPlane = lightsUni.lights[0].farPlane;
    
    lightDistance = lightDistance / farPlane;

    gl_FragDepth = lightDistance;   


}