#version 450 
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : enable
#extension GL_GOOGLE_include_directive : enable
#extension GL_EXT_scalar_block_layout : enable

#extension GL_EXT_shader_explicit_arithmetic_types_int64 : require
#extension GL_EXT_buffer_reference2 : require

#extension GL_ARB_shader_clock : enable


#include "deviceDataStruct.h"
#include "globals.glsl"
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



#include "pbr.glsl"
#include "gltfMaterial.glsl"

layout( push_constant ) uniform _PushConstantRaster
{
    PushConstantRaster pcRaster;
};

layout(binding= eGlobals) uniform _GlobalUniforms{
    GlobalUniforms uni;
};




layout(location = 1) in vec3 inWorldPos;
layout(location = 2) in vec3 inWorldNormal;
layout(location = 3) in vec3 inViewDir;
layout(location = 4) in vec2 inTexCoord;
layout(location = 5) in vec3 inTangent;
layout(location = 6) in vec3 inBitangent;


layout(location = 7) in vec3 inModelPos;

layout(location = 0) out vec3 gPosition;
layout(location = 1) out vec3 gNormal;
layout(location = 2) out vec4 gAlbedoSpce;
layout(location = 3) out vec4 gMetalRough;
layout(location = 4) out vec4 gEmissive;



void main() {
    // Material of the object 
    ObjDesc    objResource = objDesc.i[pcRaster.objIndex];
    MatIndices matIndices  = MatIndices(objResource.materialIndexAddress);
    Materials  materials   = Materials(objResource.materialAddress);

    int               matIndex = matIndices.i[gl_PrimitiveID];
    GltfShadeMaterial mat      = materials.m[matIndex];

    State state;
    state.position = inWorldPos;
    state.normal   = normalize(inWorldNormal);
    state.texCoord = inTexCoord;
    state.tangent  = normalize(inTangent);
    state.bitangent = normalize(inBitangent);

    int objIndex = pcRaster.objIndex;

    GetMaterialsAndTextures(state,objIndex);
    
        
    gPosition = inWorldPos;
    gNormal   = state.normal ;
    gAlbedoSpce = vec4(state.mat.albedo, state.mat.alpha);
    gMetalRough = vec4(state.mat.metallic, state.mat.roughness, 0.0, 0.0);
    gEmissive = vec4(state.mat.emission, 1.0);

        
    

}