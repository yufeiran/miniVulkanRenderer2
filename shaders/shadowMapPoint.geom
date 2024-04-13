#version 450 core 
#pragma vscode_glsllint_stage : vert

#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_scalar_block_layout : enable
#extension GL_ARB_shading_language_include : enable
#extension GL_GOOGLE_include_directive : enable

#extension GL_EXT_shader_explicit_arithmetic_types_int64 : require


#include "deviceDataStruct.h"

layout (triangles) in;
layout (triangle_strip, max_vertices = 18 ) out;

layout(binding = eLight,std140) uniform _LightUniforms {LightUniforms lightsUni;};

layout(location = 0) out vec4 FragPos;

void main()
{
    for(int face = 0; face < 6; ++face)
    {
        gl_Layer = face;
        for(int i = 0; i < 3; ++i)
        {
            FragPos = gl_in[i].gl_Position;
            gl_Position = lightsUni.lights[0].pointLightSpaceMatrix[face] * FragPos; 
            EmitVertex();
        }
        EndPrimitive();
    }
}