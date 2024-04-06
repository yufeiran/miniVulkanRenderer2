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

layout(binding = 2) uniform sampler2D shadowmap;

layout(binding = 3) uniform sampler2D bloomBlur;

layout(binding = 4) uniform sampler2D ssrSampler;


layout( push_constant ) uniform _PushConstantPost
{
    PushConstantPost pcPost;
};


vec3 getBloomColor(vec2 texCoords)
{

    vec3 bloomColor = vec3(0.0);
    vec2 texelSize = 1.0 / textureSize(offscreenColor, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            vec2 offset = vec2(x, y) * texelSize;
            bloomColor += texture(bloomBlur, texCoords + offset).rgb;

        }
    }
    bloomColor /= 9.0;

    


    return bloomColor;
}

void main()
{
    const float gamma = 2.2;

    vec3 color;
    color = texture(offscreenColor,TexCoords).xyz;
    vec3 bloomColor = texture(bloomBlur, TexCoords).rgb;
    if(pcPost.pbbloomMode == 1)
    {

        color = mix(color, bloomColor, pcPost.pbbloomIntensity);

    }

    vec3 ssrColor = texture(ssrSampler,TexCoords).rgb;
    if(pcPost.debugSSR == 1)
    {
        color = ssrColor;
    }
    else if(pcPost.raytraceMode == 0){
        color += ssrColor * pcPost.ssrIntensity;
    }



    vec3 colorExposure = vec3(color.r * pcPost.exposure, color.g * pcPost.exposure, color.b * pcPost.exposure);

    // exposure tone mapping
    vec3 mapped = vec3(1.0) - exp(-colorExposure);

    

    mapped = pow(mapped, vec3(1.0 / gamma));

    FragColor = vec4(mapped,1.0);

    if(pcPost.debugShadowMap == 1)
        FragColor = vec4(vec3(texture(shadowmap,TexCoords).r,texture(shadowmap,TexCoords).r,texture(shadowmap,TexCoords).r),1.0);
    if(pcPost.debugBloom == 1)
        FragColor = vec4(bloomColor,1.0);
    // if(pcPost.debugSSR == 1)
    //     FragColor = vec4(texture(ssrSampler,TexCoords).rgb,1.0);

}