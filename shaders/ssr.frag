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
layout(binding = eCubeMap) uniform samplerCube cubeMapTexture;
layout(binding = eDirShadowMap) uniform sampler2D shadowmap;
layout(binding = ePointShadowMap) uniform samplerCube pointShadowMapTexture;
layout(binding = eLight,std140) uniform _LightUniforms {LightUniforms lightsUni;};

layout( push_constant ) uniform _PushConstantRaster
{
    PushConstantRaster pcRaster;
};

layout(binding= eGlobals) uniform _GlobalUniforms{
    GlobalUniforms uni;
};

layout(location = 0) in vec2 TexCoords;

layout(location = 0) out vec4 FragColor;


layout(set = 1, binding = eSSRColor) uniform sampler2D ssrColorSampler;
layout(set = 1, binding = eSSRDepth)  uniform sampler2D ssrDepthSampler; 
layout(set = 1, binding = eSSRNormal)  uniform sampler2D ssrNormalSampler; // in view space
layout(set = 1, binding = eSSRPosition)  uniform sampler2D ssrPositionSampler; // in view space
layout(set = 1, binding = eSSRMetalRough) uniform sampler2D ssrMetalRoughSampler;


float gauss [] = float[]
(
    0.00000067, 0.00002292, 0.00019117, 0.00038771, 0.00019117, 0.00002292, 0.00000067,
    0.00002292, 0.00078633, 0.00655965, 0.01330373, 0.00655965, 0.00078633, 0.00002292,
    0.00019117, 0.00655965, 0.05472157, 0.11098164, 0.05472157, 0.00655965, 0.00019117,
    0.00038771, 0.01330373, 0.11098164, 0.22508352, 0.11098164, 0.01330373, 0.00038771,
    0.00019117, 0.00655965, 0.05472157, 0.11098164, 0.05472157, 0.00655965, 0.00019117,
    0.00002292, 0.00078633, 0.00655965, 0.01330373, 0.00655965, 0.00078633, 0.00002292,
    0.00000067, 0.00002292, 0.00019117, 0.00038771, 0.00019117, 0.00002292, 0.00000067
);

vec3 getGaussColor(vec2 uv)
{
    const vec2 screenSize = textureSize(ssrColorSampler, 0);
    const int size = 7;
    vec3 finalColor = vec3(0,0,0);
    int idx = 0;
    for(int i = -3; i <= 3; i++)
    {
        for(int j = -3; j <= 3; j++)
        {
            vec2 offset = uv + vec2(5.0 * i / screenSize.x, 5.0 * j / screenSize.y);
            vec3 color = texture(ssrColorSampler, offset).rgb;
            float weight = gauss[idx];
            idx++;
            finalColor += color * weight;
        }

    }
    return finalColor;

}

void main()
{

    const float e = 2.71828182846;

    vec3 fragPos = texture(ssrPositionSampler, TexCoords).xyz;
    vec3 normal = texture(ssrNormalSampler, TexCoords).xyz;
    vec3 color = texture(ssrColorSampler, TexCoords).xyz;
    float depth = texture(ssrDepthSampler, TexCoords).r;

    vec3 viewDir = normalize(fragPos);

    vec3 reflectDir = reflect(viewDir, normal); 

    vec3 viewReflectDir = reflectDir;

    mat4 dither = mat4(0.0,  0.5,  0.125,  0.625,
                        0.75, 0.25, 0.875, 0.375,
                        0.1875, 0.6875, 0.0625, 0.5625,
                        0.9375, 0.4375, 0.8125, 0.3125);

    const vec2 screenSize = textureSize(ssrColorSampler, 0);

    int sampleCoordX = int(mod((screenSize.x * TexCoords.x),4.0));
    int sampleCoordY = int(mod((screenSize.y * TexCoords.y),4.0));
    float offset = dither[sampleCoordX][sampleCoordY];

    vec3 albedo = vec3(0,0,0);
    //albedo = color;
    //albedo = mix(albedo, texture(cubeMapTexture, viewReflectDir).rgb, 0.2);

    float step = pcRaster.ssrStep;
    vec3 farPos = vec3(fragPos.x,fragPos.y,100);

    const float eps = pcRaster.ssrEpsilon;
    
    int stepNum = int(length(farPos)/step);

    stepNum = int(pcRaster.ssrMaxDistance / step);
    

    vec3 lastPos = fragPos;
    for(int i = 1; i < stepNum; i++)
    {
        // Ray Match
        float delta = step * i + offset;
        vec3 rayPos = fragPos + viewReflectDir * delta; // in view space 


        

        vec4 rayPosInScreenTmp = uni.proj * vec4(rayPos, 1.0);
        vec3 rayPosInScreen = rayPosInScreenTmp.xyz / rayPosInScreenTmp.w;

        float d = rayPosInScreen.z;

        vec2 uv = (rayPosInScreen.xy + vec2(1.0)) / 2.0;

        

        if(uv.x >=  0.0 && uv.x <= 1 && uv.y >= 0.0 && uv.y <= 1.0)
        {
            float depth = texture(ssrDepthSampler, uv).r;

            // hit!
            if(d > depth)
            {
                if(abs(d - depth ) < eps)
                {
                    vec3 nowColor =  getGaussColor(uv);

                    float distance = step * i;
                    float weight = pow(e,-distance * pcRaster.ssrAttenuation);
                    albedo = mix(albedo, nowColor, weight);
                    break;
                    //albedo = vec3(1,0,0);
                }

                if(pcRaster.ssrUseBinarySearch == 1)
                {
                    
                    //  binary search
                    vec3 begin = lastPos;
                    vec3 end = rayPos;
                    vec3 currPos = (begin + end) / 2.0;

                    int count = 0;

                    while(true)
                    {
                        count++;
                        d = currPos.z;
                        vec4 tmp = uni.proj * vec4(currPos, 1.0);
                        rayPosInScreen = tmp.xyz / tmp.w;

                        uv = (rayPosInScreen.xy + vec2(1.0)) / 2.0;
                        depth = texture(ssrDepthSampler, uv).r;

                        if(abs(d - depth) < 0.3 * eps)
                        {
                            vec3 nowColor  = getGaussColor(uv);
                            float distance = step * i;
                            float weight = pow(e,-distance * pcRaster.ssrAttenuation);
                            albedo = mix(albedo, nowColor, weight);
                            break;
                        }
                        if(d > depth)
                        {
                            end = currPos;
                        }
                        else if(d < depth)
                        {
                            begin = currPos;
                        }
                        currPos = (begin + end) / 2.0;
                        if(count > 5)
                        {
                            break;
                        
                        }

                    }


                }

            




                
            }
            else {
                lastPos = rayPos;
            }



        }




    }

    float metal = texture(ssrMetalRoughSampler, TexCoords).r;
    float rough = texture(ssrMetalRoughSampler, TexCoords).g;

    vec3 finalColor = vec3(0,0,0);
    finalColor = mix(finalColor, albedo, metal * rough);

    FragColor = vec4(finalColor, 1.0);

}