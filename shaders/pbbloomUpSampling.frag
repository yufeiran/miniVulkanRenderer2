#version 450  

#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_scalar_block_layout : enable
#extension GL_ARB_shading_language_include : enable
#extension GL_GOOGLE_include_directive : enable

#extension GL_EXT_shader_explicit_arithmetic_types_int64 : require


#include "deviceDataStruct.h"

layout (binding = epbbloomInput) uniform sampler2D inTexture;
layout (binding = epbInputSize) buffer InResolution{vec2 inResolution;};

layout (location = 0) in vec2 texCoord;
layout (location = 0) out vec3 upsample;


layout( push_constant ) uniform _PushConstantPost
{
    PushConstantPost pcPost;
};


void main()
{
    float x = pcPost.pbbloomRadius;
    float y = pcPost.pbbloomRadius;

    // Sample 9 pixels
    // a - b - c
    // d - e - f
    // g - h - i
    // === ('e' is current texel)
    vec3 a = texture(inTexture, texCoord + vec2(-x, +y)).rgb;
    vec3 b = texture(inTexture, texCoord + vec2( 0, +y)).rgb;
    vec3 c = texture(inTexture, texCoord + vec2(+x, +y)).rgb;

    vec3 d = texture(inTexture, texCoord + vec2(-x,  0)).rgb;
    vec3 e = texture(inTexture, texCoord + vec2( 0,  0)).rgb;
    vec3 f = texture(inTexture, texCoord + vec2(+x,  0)).rgb;

    vec3 g = texture(inTexture, texCoord + vec2(-x, -y)).rgb;
    vec3 h = texture(inTexture, texCoord + vec2( 0, -y)).rgb;
    vec3 i = texture(inTexture, texCoord + vec2(+x, -y)).rgb;

    // Apply weighted distribution
    //  1   | 1 2 1 |
    // -- * | 2 4 2 |
    // 16   | 1 2 1 |
    upsample = e * 4.0;
    upsample += (b + d + f + h) * 2.0;
    upsample += (a + c + g + i) * 1.0;
    upsample /= 16.0;

}