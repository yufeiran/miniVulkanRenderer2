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

layout(binding = eHDRInput) uniform sampler2D HDRInput;
layout(binding = eHDRToCubeMapUniforms,std140) uniform _HDRToCubeMapUniforms {HDRToCubeMapUniforms hdrToCubeMapUni;};

vec4 SRGBtoLINEAR(vec4 srgbIn)
{
    vec3 linOut = pow(srgbIn.xyz, vec3(2.2));

    return vec4(linOut, srgbIn.w);
}

// this magic number is [ 1/(2*PI), 1/PI ],used to remap the value of the spherical coordinates to the range of [0,1]
const vec2 invAtan = vec2(0.1591, 0.3183);
vec2 sampleSphericalMap(vec3 v)
{
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

void main()
{
    vec2 uv = sampleSphericalMap(normalize(fragPos.xyz));
    vec3 color = SRGBtoLINEAR(texture(HDRInput, uv)).rgb;
    fragColor = vec4(color, 1.0);


}