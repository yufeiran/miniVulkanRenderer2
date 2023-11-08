#version 460
#extension GL_EXT_ray_tracing : require
#extension GL_GOOGLE_include_directive : enable
#extension GL_EXT_shader_explicit_arithmetic_types_int64 : require

#include "raycommon.glsl"
#include "deviceDataStruct.h" 

layout(location = 0) rayPayloadInEXT hitPayload prd;

layout(set = 1, binding = eCubeMap) uniform samplerCube cubeMapTexture;

layout(push_constant) uniform _PushConstantRay
{
    PushConstantRay pcRay;
};

void main()
{

    //prd.hitValue = pcRay.clearColor.xyz * 0.8;
    prd.hitValue = texture(cubeMapTexture,gl_WorldRayDirectionEXT).xyz;
}