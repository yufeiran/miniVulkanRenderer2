#version 460 
#extension GL_EXT_ray_tracing : require
#extension GL_EXT_nonuniform_qualifier : enable
#extension GL_EXT_scalar_block_layout : enable
#extension GL_GOOGLE_include_directive : enable

#extension GL_EXT_shader_explicit_arithmetic_types_int64 : require
#extension GL_EXT_buffer_reference2 : require

#include "deviceDataStruct.h"
#include "raycommon.glsl"

hitAttributeEXT vec2 attribs;

layout(location = 0) rayPayloadInEXT hitPayload prd;
layout(location = 1) rayPayloadEXT bool isShadowed;

layout(buffer_reference, scalar) buffer Vertices {Vertex v[]; };
layout(buffer_reference, scalar) buffer Indices {uint i[]; };
layout(buffer_reference, scalar) buffer Materials { WaveFrontMaterial m[]; };
layout(buffer_reference, scalar) buffer MatIndices { int i[]; };

layout(set = 0, binding = eTlas) uniform accelerationStructureEXT topLevelAS;
layout(set = 1, binding = eObjDescs, scalar) buffer ObjDesc_ { ObjDesc i[]; } objDesc;
layout(set = 1, binding = eTextures) uniform sampler2D[] textureSamplers;

layout(push_constant) uniform _PushConstantRay {PushConstantRay pcRay; };

void main()
{
    // Object data
    ObjDesc    objResource = objDesc.i[pcRaster.objIndex];
    MatIndices matIndices  = MatIndices(objResource.materialIndexAddress);
    Materials  materials   = Materials(objResource.materialAddress);
    Indices    indices     = Indices(objResource.indexAddress);
    Vertices   vertices    = Vertices(objResource.vertexAddress);

    // Indices of the triangle
    ivec3 ind = indices.i[gl_PrimitiveID];

    // Vertex of the triangle
    Vertex v0 = vertices.v[ind.x];
    Vertex v1 = vertices.v[ind.y];
    Vertex v2 = vertices.v[ind.z];

    const vec3 barycentrics = vec3(1.0 - attribs.x - attribs.y, attribs.x, attribs.y);

    // Computing the coordinates of the hit position
    const vec3 pos      = v0.pos * barycentrics.x + v1.pos * barycentrics.y + v2.pos * barycentrics.z;
    const vec3 worldPos = vec3(gl_ObjectToWorldEXT * vec4(pos, 1.0)); // Transforming the position to world space

    // Computing the normal at hit position 
    const vec3 normal      = v0.normal * barycentrics.x + v1.normal * barycentrics.y + v2.normal * barycentrics.z;
    const vec3 worldNormal = vec3(gl_ObjectToWorldEXT * vec4(normal, 1.0)); // Transforming the normal to world space

    // Vector toward the light 
    vec3  L; // direction of light
    float lightIntensity = pcRay.lightIntensity;
    float lightDistance  = 100000.0;

    // Point light 
    if(pcRay.lightType == 0)
    {
        vec3 lDir      = pcRay.lightPosition - worldPos; 
        lightDistance  = length(lDir);
        lightIntensity = pcRay.lightIntensity / (lightDistance * lightDistance);
        L              = normalize(lDir);
    }
    else // Directional light
    {
        L = normalize(pcRay.lightPosition);
    }

    // Material of the object 
    int               matIdx = matIndices.i[gl_PrimitiveID];
    WaveFrontMaterial mat    = materials.m[matIdx];

    // Diffuse
    vec3 diffuse = 


}