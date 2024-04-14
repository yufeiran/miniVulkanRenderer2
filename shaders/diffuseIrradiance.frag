#version 450
#pragma vscode_glsllint_stage : vert

#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_scalar_block_layout : enable
#extension GL_ARB_shading_language_include : enable
#extension GL_GOOGLE_include_directive : enable

#extension GL_EXT_shader_explicit_arithmetic_types_int64 : require


#include "deviceDataStruct.h"

layout(location = 0) in vec4 fragPos;
layout(location = 0) out vec4 fragColor;

layout(binding = eDiffuseIrradianceInput) uniform samplerCube rawInput;
layout(binding = eDiffuseIrradianceUniforms,std140) uniform _HDRToCubeMapUniforms {HDRToCubeMapUniforms hdrToCubeMapUni;};

const float PI = 3.14159265359;

void main()
{
    vec3 normal = normalize(fragPos).xyz;

    vec3 irradiance = vec3(0.0);

    vec3 up    = vec3(0.0, 1.0, 0.0);
    vec3 right = normalize(cross(up, normal));
    up         = normalize(cross(normal, right));

    float sampleDelta = 0.025;
    float nrSamples   = 0.0;
    for(float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta)
    {
        for(float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta)
        {
            // sample in tangetnt space
            vec3 tangentSample = vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
            // transform to world space
            vec3 sampleVec = tangentSample.x *  right + tangentSample.y * up + tangentSample.z * normal;

            irradiance += texture(rawInput,sampleVec).rgb * cos(theta) * sin(theta);
            nrSamples++;
        }

    }

    irradiance = PI * irradiance * (1.0 / float(nrSamples));

    fragColor = vec4(irradiance, 1.0);
}