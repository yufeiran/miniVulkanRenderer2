#version 460 

#extension GL_EXT_ray_tracing : require
#extension GL_GOOGLE_include_directive : enable
#extension GL_EXT_shader_explicit_arithmetic_types_int64 : require
#extension GL_ARB_shader_clock : enable
#extension GL_EXT_scalar_block_layout : enable          // Usage of 'scalar' block layout

#extension GL_EXT_buffer_reference2 : require


#extension GL_EXT_nonuniform_qualifier : enable



#include "deviceDataStruct.h"
#include "globals.glsl"


layout(location = 1) rayPayloadInEXT shadowPayload prdShadow;

void main()
{
    prdShadow.isShadowed = false;
}