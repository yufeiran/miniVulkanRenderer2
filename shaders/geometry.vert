
#version 460

#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_scalar_block_layout : enable
#extension GL_ARB_shading_language_include : enable
#extension GL_GOOGLE_include_directive : enable

#extension GL_EXT_shader_explicit_arithmetic_types_int64 : require


#include "deviceDataStruct.h"

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec4 inColor;
layout(location = 3) in vec3 inTangent;
layout(location = 4) in vec3 inBitangent;
layout(location = 5) in vec2 inTexCoord;


layout(location = 1) out vec3 outWorldPos;
layout(location = 2) out vec3 outWorldNormal;
layout(location = 3) out vec3 outViewDir;
layout(location = 4) out vec2 outTexCoord;
layout(location = 5) out vec3 outTangent;
layout(location = 6) out vec3 outBitangent;
layout(location = 7) out vec3 outModelPos;
layout(location = 8) out vec3 outViewPos;
layout(location = 9) out vec3 outViewNormal;

layout( push_constant ) uniform _PushConstantRaster
{
    PushConstantRaster pcRaster;
};

layout(binding= eGlobals) uniform _GlobalUniforms{
    GlobalUniforms uni;
};


void main(){
    vec3 origin = vec3(uni.viewInverse * vec4(0,0,0,1));



    outWorldPos = vec3(pcRaster.modelMatrix * vec4(inPosition, 1.0));
    outViewDir     = vec3(outWorldPos - origin);
    outTexCoord    = inTexCoord;
    outWorldNormal = mat3(pcRaster.modelMatrix) * inNormal;
    outTangent     = inTangent;
    outBitangent   = inBitangent;
    outModelPos    = inPosition;

    outViewPos = vec3(uni.view * vec4(outWorldPos,1.0));
    outViewNormal = vec3(uni.view * vec4(outWorldNormal,0.0));

    gl_Position = uni.viewProj * vec4(outWorldPos,1.0);

    
    //gl_Position = pcRaster.lightSpaceMatrix * vec4(outWorldPos,1.0);

}