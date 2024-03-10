#version 450 
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : enable
#extension GL_GOOGLE_include_directive : enable
#extension GL_EXT_scalar_block_layout : enable

#extension GL_EXT_shader_explicit_arithmetic_types_int64 : require
#extension GL_EXT_buffer_reference2 : require

#extension GL_ARB_shader_clock : enable

#include "deviceDataStruct.h"
#include "wavefront.glsl"




layout(buffer_reference, scalar) buffer Vertices {Vertex v[]; };
layout(buffer_reference, scalar) buffer Indices {uint i[]; };
layout(buffer_reference, scalar) buffer Materials { GltfShadeMaterial m[]; };
layout(buffer_reference, scalar) buffer MatIndices { int i[]; };

layout(binding = eObjDescs, scalar) buffer ObjDesc_ { ObjDesc i[]; } objDesc;
layout(binding = eTextures) uniform sampler2D[] textureSamplers;
layout(binding = eDirShadowMap) uniform sampler2D shadowmap;
layout(binding = ePointShadowMap) uniform samplerCube pointShadowMapTexture;
layout(binding = eLight,std140) uniform _LightUniforms {LightUniforms lightsUni;};

layout( push_constant ) uniform _PushConstantRaster
{
    PushConstantRaster pcRaster;
};

layout(binding= eGlobals) uniform _GlobalUniforms{
    GlobalUniforms uni;
};

layout(location = 0) in vec2 TexCoords;

layout(location = 0) out vec4 FragColor;


layout(set = 1, binding = eSSAOSAMPLE) buffer SsaoSampler{vec3 ssaoSampler[];};
layout(set = 1, binding = eSSAONoise)  uniform sampler2D ssaoNoiseSampler;
layout(set = 1, binding = eSSAOPosition)  uniform sampler2D posViewSpaceSampler;
layout(set = 1, binding = eSSAONormal)  uniform sampler2D   normalViewSpaceSampler;
layout(set = 1, binding = eSSAOAlbedo)  uniform sampler2D   albedoSampler;



void main()
{

    vec2 noiseScale = vec2(pcRaster.screenWidth / 4.0, pcRaster.screenHeight / 4.0);

    vec3 fragPosInView   = texture(posViewSpaceSampler, TexCoords).xyz;
    vec3 normalInView    = texture(normalViewSpaceSampler, TexCoords).xyz;

    vec3 randomVec = texture(ssaoNoiseSampler, TexCoords * noiseScale).xyz;
    vec3 tangent   = normalize(randomVec - normalInView * dot(randomVec, normalInView));
    vec3 bitangent = cross(normalInView, tangent);
    mat3 TBN       = mat3(tangent, bitangent, normalInView);

    float occlusion = 0.0;
    const int kernelSize = 32;
    const float radius = 0.5;
    const float bias = 0.025;
    for(int i = 0; i < kernelSize; ++i)
    {
        // get sample position:
        vec3 samplePos = TBN * ssaoSampler[i]; // from tangent to view-space
        samplePos      = fragPosInView + samplePos * radius;

        vec4 offset    = vec4(samplePos, 1.0);
        offset         = uni.proj * offset;  // from view to clip-space
        offset.xyz     /= offset.w;           // perspective divide
        offset.xyz     = offset.xyz * 0.5 + 0.5; // transform to range 0.0 - 1.0

        vec3  targetSamplePosInView = texture(posViewSpaceSampler, offset.xy).xyz;
        float sampleDepth =  targetSamplePosInView.z; // get depth value of kernel sample

        float rangeCheck  = smoothstep(0.0, 1.0, radius / abs(fragPosInView.z - sampleDepth));

        occlusion += (sampleDepth >= samplePos.z + bias ? 1.0 : 0.0) * rangeCheck;
    }
    occlusion = 1.0 - (occlusion / kernelSize);
    FragColor = vec4(pow(occlusion, 2.2));

}