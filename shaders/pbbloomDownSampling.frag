#version 450  

#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_scalar_block_layout : enable
#extension GL_ARB_shading_language_include : enable
#extension GL_GOOGLE_include_directive : enable

#extension GL_EXT_shader_explicit_arithmetic_types_int64 : require


#include "deviceDataStruct.h"

layout (set = 0, binding = epbbloomInput) uniform sampler2D inputTexture;

layout(set = 0, binding = epbInputSize) buffer InResolution{vec2 inResolution;};

layout (location = 0) in vec2 texCoord;

layout (location = 0) out vec3 downSample;



void main()
{
    vec2 inTexelSize = 1.0 / inResolution;
    float x = inTexelSize.x;
    float y = inTexelSize.y;

    // Take 13 samples around current texel:
    // a - b - c
    // - j - k -
    // d - e - f
    // - l - m -
    // g - h - i
    // === ('e' is the current texel) ===
    vec3 a = texture(inputTexture, vec2(texCoord.x - 2*x, texCoord.y + 2*y)).rgb;
    vec3 b = texture(inputTexture, vec2(texCoord.x,       texCoord.y + 2*y)).rgb;
    vec3 c = texture(inputTexture, vec2(texCoord.x + 2*x, texCoord.y + 2*y)).rgb;

    vec3 d = texture(inputTexture, vec2(texCoord.x - 2*x, texCoord.y)).rgb;
    vec3 e = texture(inputTexture, vec2(texCoord.x,       texCoord.y)).rgb;
    vec3 f = texture(inputTexture, vec2(texCoord.x + 2*x, texCoord.y)).rgb;

    vec3 g = texture(inputTexture, vec2(texCoord.x - 2*x, texCoord.y - 2*y)).rgb;
    vec3 h = texture(inputTexture, vec2(texCoord.x,       texCoord.y - 2*y)).rgb;
    vec3 i = texture(inputTexture, vec2(texCoord.x + 2*x, texCoord.y - 2*y)).rgb;

    vec3 j = texture(inputTexture, vec2(texCoord.x - x, texCoord.y + y)).rgb;
    vec3 k = texture(inputTexture, vec2(texCoord.x + x, texCoord.y + y)).rgb;
    vec3 l = texture(inputTexture, vec2(texCoord.x - x, texCoord.y - y)).rgb;
    vec3 m = texture(inputTexture, vec2(texCoord.x + x, texCoord.y - y)).rgb;

    // Apply weighted distribution:
    // 0.5 + 0.125 + 0.125 + 0.125 + 0.125 = 1
    // a,b,d,e * 0.125
    // b,c,e,f * 0.125
    // d,e,g,h * 0.125
    // e,f,h,i * 0.125
    // j,k,l,m * 0.5

    downSample = e * 0.125;
    downSample += (a + c + g + i) * 0.03125;
    downSample += (b + d + f + h) * 0.0625;
    downSample += (j + k + l + m) * 0.125;


}