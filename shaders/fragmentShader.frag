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

layout( push_constant ) uniform _PushConstantRaster
{
    PushConstantRaster pcRaster;
};



layout(location = 1) in vec3 inWorldPos;
layout(location = 2) in vec3 inWorldNormal;
layout(location = 3) in vec3 inViewDir;
layout(location = 4) in vec2 inTexCoord;

layout(location = 0) out vec4 outColor;


void main() {
    // Material of the object 
    ObjDesc    objResource = objDesc.i[pcRaster.objIndex];
    MatIndices matIndices  = MatIndices(objResource.materialIndexAddress);
    Materials  materials   = Materials(objResource.materialAddress);

    int               matIndex = matIndices.i[gl_PrimitiveID];
    GltfShadeMaterial mat      = materials.m[matIndex];

    vec3 N = normalize(inWorldNormal);

    // Vector toward light 
    vec3 L;
    float lightIntensity = pcRaster.lightIntensity;
    if(pcRaster.lightType == 0)
    {
        vec3  lDir     = pcRaster.lightPosition - inWorldPos;
        float d        = length(lDir);
        lightIntensity = pcRaster.lightIntensity / (d * d);
        L              = normalize(lDir);   
    }
    else 
    {
        L = normalize(pcRaster.lightPosition);
    }

    if(mat.type == 0)
    {
        // Diffuse 
        vec3 diffuse = computeDiffuse(mat, L, N);
        if(mat.textureId >= 0)
        {
            int  txtOffset  = objDesc.i[pcRaster.objIndex].txtOffset;
            uint txtId      = txtOffset + mat.textureId;
            vec3 diffuseTxt = texture(textureSamplers[nonuniformEXT(txtId)], inTexCoord).xyz;
            diffuse *=diffuseTxt;
        }

        // Specular 
        vec3 specular = computeSpecular(mat, inViewDir, L, N);

        outColor = vec4(lightIntensity * (diffuse + specular), 1);
    }
    // PBR
    else if(mat.type == 1)
    {
        // Diffuse 
        vec3 diffuse = computeDiffuse(mat, L, N);
        if(mat.pbrBaseColorTexture >= 0)
        {
            // uint txtId    = mat.pbrBaseColorTexture + objDesc.i[pcRaster.objIndex].txtOffset;
            uint txtId    = mat.pbrBaseColorTexture;
            vec3 diffuseTxt = texture(textureSamplers[nonuniformEXT(txtId)], inTexCoord).xyz;
            diffuse *=diffuseTxt;
        }
        else 
        {
            diffuse *= mat.pbrBaseColorFactor.xyz;
        }

        // Specular 
        vec3 specular = computeSpecular(mat, inViewDir, L, N);

        outColor = vec4(lightIntensity * (diffuse + specular), 1);
       
    }


}