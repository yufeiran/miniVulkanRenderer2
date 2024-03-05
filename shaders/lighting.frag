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
#include "pbr.glsl"


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



layout (input_attachment_index = eGPosition   , set = 1, binding = eGPosition   ) uniform subpassInput inputPosition;
layout (input_attachment_index = eGNormal     , set = 1, binding = eGNormal     ) uniform subpassInput inputNormal;
layout (input_attachment_index = eGAlbedo     , set = 1, binding = eGAlbedo     ) uniform subpassInput inputAlbedo;
layout (input_attachment_index = eGMetalRough , set = 1, binding = eGMetalRough ) uniform subpassInput inputMetalRough;
layout (input_attachment_index = eGEmission   , set = 1, binding = eGEmission   ) uniform subpassInput inputEmission;
layout (input_attachment_index = eGDepth      , set = 1, binding = eGDepth      ) uniform subpassInput inputDepth;


layout(location = 0) in vec2 TexCoords;

layout(location = 0) out vec4 outColor;





void main() {


    vec3 fragPos = subpassLoad(inputPosition).rgb;

    vec3 origin = vec3(uni.viewInverse * vec4(0,0,0,1));

    vec3 viewDir = normalize(fragPos - origin);



    State state;
    state.position = fragPos;
    state.normal = subpassLoad(inputNormal).rgb * 2.0 - 1.0;;
    state.ffnormal = dot(state.normal, viewDir) >= 0.0 ? state.normal : -state.normal;
    createCoordinateSystem(state.normal, state.tangent, state.bitangent);

    state.mat.albedo = subpassLoad(inputAlbedo).rgb;
    state.mat.metallic = subpassLoad(inputMetalRough).r;
    state.mat.roughness = subpassLoad(inputMetalRough).g;
    state.mat.ior = 1.4;




}