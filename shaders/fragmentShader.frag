#version 450 
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : enable
#extension GL_GOOGLE_include_directive : enable
#extension GL_EXT_scalar_block_layout : enable

#extension GL_EXT_shader_explicit_arithmetic_types_int64 : require
#extension GL_EXT_buffer_reference2 : require

#extension GL_ARB_shader_clock : enable

#include "deviceDataStruct.h"


layout(buffer_reference, scalar) buffer Vertices {Vertex v[]; };
layout(buffer_reference, scalar) buffer Indices {uint i[]; };
layout(buffer_reference, scalar) buffer Materials { WaveFrontMaterial m[]; };
layout(buffer_reference, scalar) buffer MatIndices { int i[]; };

layout(binding = eObjDescs, scalar) buffer ObjDesc_ { ObjDesc i[]; } objDesc;
layout(binding = eTextures) uniform sampler2D[] textureSamplers;

layout( push_constant ) uniform _PushConstantRaster
{
    PushConstantRaster pcRaster;
};


layout(location = 0) out vec4 outColor;

layout(location = 0) in vec3 fragColor; 
layout(location = 1) in vec2 fragTexCoord;


void main() {
    // Material of the object 
    ObjDesc    objResource = objDesc.i[pcRaster.objIndex];
    MatIndices matIndices  = MatIndices(objResource.materialIndexAddress);
    Materials  materials   = Materials(objResource.materialAddress);

    int               matIndex = matIndices.i[gl_PrimitiveID];
    WaveFrontMaterial mat      = materials.m[matIndex];


    // Diffuse 
    vec3 diffuseTxt;
    if(mat.textureId >= 0)
    {
        int txtOffset = objDesc.i[pcRaster.objIndex].txtOffset;
        uint txtId    = txtOffset + mat.textureId;
        diffuseTxt = texture(textureSamplers[nonuniformEXT(txtId)],fragTexCoord).xyz; 
    }


    outColor = vec4(diffuseTxt,1.0);
}