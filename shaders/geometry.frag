#version 450 
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : enable
#extension GL_GOOGLE_include_directive : enable
#extension GL_EXT_scalar_block_layout : enable

#extension GL_EXT_shader_explicit_arithmetic_types_int64 : require
#extension GL_EXT_buffer_reference2 : require

#extension GL_ARB_shader_clock : enable


#include "globals.glsl"
#include "wavefront.glsl"

#include "deviceDataStruct.h"

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
layout(location = 6) in float inTangentSign;
// layout(location = 6) in vec3 inBitangent;


layout(location = 7) in vec3 inModelPos;

layout(location = 8) in vec3 inViewPos;
layout(location = 9) in vec3 inViewNormal;

layout(location = 0) out vec3 gPosition;
layout(location = 1) out vec3 gNormal;
layout(location = 2) out vec4 gAlbedoSpce;
layout(location = 3) out vec4 gMetalRough;
layout(location = 4) out vec4 gEmissive;
layout(location = 5) out vec3 gPositionViewSpace;
layout(location = 6) out vec3 gNormalViewSpace;

vec3 toneMap(vec3 color)
{
    return color / (color + vec3(1.0));
}

mat3 getTBNFromUV(vec3 N, vec3 p, vec2 uv) {
    // 获取像素在屏幕空间的导数
    vec3 dp1 = dFdx(p);
    vec3 dp2 = dFdy(p);
    vec2 duv1 = dFdx(uv);
    vec2 duv2 = dFdy(uv);

    // 求解线性方程组
    vec3 dp2perp = cross(dp2, N);
    vec3 dp1perp = cross(N, dp1);
    vec3 T = dp2perp * duv1.x + dp1perp * duv2.x;
    vec3 B = dp2perp * duv1.y + dp1perp * duv2.y;

    // 构建互相垂直的分量
    float invmax = inversesqrt(max(dot(T,T), dot(B,B)));
    return mat3(T * invmax, B * invmax, N);
}

void main() {
    
    vec3 N = normalize(inWorldNormal);
    vec3 T = normalize(inTangent);

    T = normalize(T - dot(T, N) * N);

    vec3 B = cross(N, T) * inTangentSign;
    
    
    // Material of the object 

    ObjDesc    objResource = objDesc.i[pcRaster.objIndex];
    MatIndices matIndices  = MatIndices(objResource.materialIndexAddress);
    Materials  materials   = Materials(objResource.materialAddress);

    int               matIndex = matIndices.i[gl_PrimitiveID];
    GltfShadeMaterial mat      = materials.m[matIndex];



    State state;
    state.position = inWorldPos;
    state.normal   = N;
    state.texCoord = inTexCoord;
    state.tangent  = T;
    state.bitangent = B;

    int objIndex = pcRaster.objIndex;

    GetMaterialsAndTextures(state,objIndex);

    if(state.mat.alpha < 0.5)
        discard;

    state.mat.emission = SRGBtoLINEAR(vec4(state.mat.emission, 1.0)).rgb;

    if(pcRaster.objType == 1)
    {
        LightDesc light = lightsUni.lights[pcRaster.lightIndex];
        
        state.mat.emission = light.color.xyz * max(light.intensity,1.0);
    }

        
    gPosition = inWorldPos;
    gNormal   = state.normal ;
    gAlbedoSpce = SRGBtoLINEAR(vec4(state.mat.albedo, state.mat.alpha));
    gMetalRough = vec4(state.mat.metallic, state.mat.roughness, 0.0, 0.0);
    gEmissive = vec4(state.mat.emission, 1.0);

    gPositionViewSpace = inViewPos;
    gNormalViewSpace   = normalize(inViewNormal);

        
    

}