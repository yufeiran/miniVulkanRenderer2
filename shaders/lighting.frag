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




layout (input_attachment_index = eGPosition  , set = 1, binding = eGPosition   ) uniform subpassInput  inputPosition;
layout (input_attachment_index = eGNormal    , set = 1, binding = eGNormal     ) uniform subpassInput  inputNormal;
layout (input_attachment_index = eGAlbedo    , set = 1, binding = eGAlbedo     ) uniform subpassInput  inputAlbedo;
layout (input_attachment_index = eGMetalRough, set = 1, binding = eGMetalRough ) uniform subpassInput  inputMetalRough;
layout (input_attachment_index = eGEmission  , set = 1, binding = eGEmission   ) uniform subpassInput  inputEmission;
layout (input_attachment_index = eGDepth     , set = 1, binding = eGDepth      ) uniform subpassInput  inputDepth;
layout (input_attachment_index = eGSSAOBlur  ,set = 1, binding = eGSSAOBlur    ) uniform subpassInput  inputSSAOBlur;



layout(location = 0) in vec2 TexCoords;

layout(location = 0) out vec4 outColor;

#include "pbr.glsl"



vec3 calcLight(inout State state, vec3 V, vec3 L, vec3 lightIntensity, float lightPdf)
{
    vec3 Li = vec3(0);
    if(dot(state.ffnormal, L)>0){
        BsdfSampleRec directBsdf;
        directBsdf.f = PbrEval(state, V, state.ffnormal, L,directBsdf.pdf);
        float misWeightBsdf = max(0.0, powerHeuristic(directBsdf.pdf, lightPdf));
        Li =  directBsdf.f * misWeightBsdf * abs( dot(state.ffnormal, L)) * lightIntensity  / directBsdf.pdf;
        if(any(isnan(Li)))
        {
            Li = vec3(0.0);
        }

    }
    return Li;
}



float shadowCalculationDir(vec4 fragPosLightSpace, float bias)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    vec2 projCoordsXY = projCoords.xy * 0.5 + 0.5;
    float closestDepth = texture(shadowmap, projCoordsXY.xy).r;
    float currentDepth = projCoords.z;



    float shadow = 0.0;

    // PCF 
    vec2 texelSize = 1.0 / textureSize(shadowmap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowmap, projCoordsXY.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;

    if(projCoords.z > 1.0)
    {
        shadow = 0.0;
    }

    return shadow;

}

vec3 sampleOffsetDirections[20] = vec3[]
(
   vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
   vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
   vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
   vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
   vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
);   

float shadowCalculationPoint(vec3 fragPos,LightDesc light)
{

    vec3 fragToLight   = fragPos - light.position.xyz;
    // float closestDepth = texture(pointShadowMapTexture, fragToLight).r;
    // float farPlane     = light.farPlane;
    // closestDepth       *= farPlane;

    float currentDepth = length(fragToLight);
    // float bias         = 0.05;
    // float shadow       = currentDepth - bias > closestDepth  ? 1.0 : 0.0;

    // PCF(percentage-closer filtering)
    // float shadow = 0.0;
    // float bias = 0.05;
    // float samples = 4.0;
    // float offset = 0.1;

    // for(float x = -offset; x < offset; x += offset / (samples * 0.5))
    // {
    //     for(float y = -offset; y < offset; y += offset / (samples * 0.5))
    //     {
    //         for(float z = -offset; z < offset; z += offset / (samples * 0.5))
    //         {
    //             float closestDepth = texture(pointShadowMapTexture, fragToLight + vec3(x, y, z)).r;
    //             float farPlane     = light.farPlane;
    //             closestDepth       *= farPlane;
    //             if(currentDepth - bias > closestDepth)
    //             {
    //                 shadow += 1.0;
    //             }
    //         }
    //     }
    // }

    // shadow /= samples * samples * samples;

    // Better PCF
    float shadow = 0.0;
    float bias = 0.05;
    int   samples = 20;
    float viewDistance = length(fragToLight);
    float diskRadius = (1.0 + (viewDistance / light.farPlane)) / 25.0;
    for(int i = 0; i < samples; ++i)
    {
        float closestDepth = texture(pointShadowMapTexture, fragToLight + diskRadius * sampleOffsetDirections[i]).r;
        closestDepth *= light.farPlane;
        if(currentDepth - bias > closestDepth)
        {
            shadow += 1.0;
        }
    }
    shadow /= float(samples);

    return shadow;
}


vec3 DebugInfo(State state,vec3 color)
{
    vec3 outColor = color;

    switch(pcRaster.debugMode)
    {
        case eNoDebug:
            return outColor;
        case eNormal:
            outColor = vec3(state.normal.xyz + vec3(1)) * 0.5;
            return outColor;
        case eMetallic:
            outColor = vec3(state.mat.metallic);
            return outColor;
        case eBaseColor:
            outColor = vec3(state.mat.albedo);
            return outColor;
        case eEmissive:
            outColor = vec3(state.mat.emission);
            return outColor;
        case eAlpha:
            outColor = vec3(1.0);
            return outColor;
        case eRoughness:
            outColor = vec3(state.mat.roughness);
            return outColor;
        case eTexCoord:
            outColor = vec3(1.0);
            return outColor;
        case eTangent:
            outColor = vec3(1);
            return outColor;
        case eBitangent:
            outColor = vec3(1);
            return outColor;
        case eSpecular:
            outColor = vec3( 1.0);
            return outColor;
        case eSSAO:
            outColor = vec3(state.mat.ao);
            return outColor;

    }
}

void main() {


    vec3 fragPos = subpassLoad(inputPosition).xyz;

    vec3 origin = vec3(uni.viewInverse * vec4(0,0,0,1));

    vec3 viewDir = normalize(fragPos - origin);

    float depth = subpassLoad(inputDepth).r;

    if(depth == 1.0)
    {
        discard;
    }



    State state;
    state.position = fragPos;
    state.normal = subpassLoad(inputNormal).rgb;
    state.ffnormal = dot(state.normal, viewDir) >= 0.0 ? state.normal : -state.normal;
    createCoordinateSystem(state.normal, state.tangent, state.bitangent);

    state.mat.albedo = subpassLoad(inputAlbedo).rgb;
    state.mat.metallic = subpassLoad(inputMetalRough).r;
    state.mat.roughness = subpassLoad(inputMetalRough).g;
    state.mat.ior = 1.4;
    state.mat.ao  = subpassLoad(inputSSAOBlur).r;

    float dielectricSpecular = (state.mat.ior - 1) / (state.mat.ior + 1);
    dielectricSpecular *=  dielectricSpecular;

    state.mat.f0 = mix(vec3(dielectricSpecular), state.mat.albedo, state.mat.metallic);

    


    state.mat.emission = subpassLoad(inputEmission).rgb;
    state.mat.transmission = 0.0;


    vec3 color = vec3(0.0);

    for(int i = 0; i < lightsUni.lightCount; i++)
    {
        vec3 L = vec3(0); // vector to light
        float lightIntensity = 0.0;
        float lightDistance = 100000.0;
        float lightPdf = 1.0;

        
        LightDesc light = lightsUni.lights[i];
        lightIntensity = light.intensity;
        vec3 lightPos = light.position.xyz;
        if(light.type == 0) // point light
        {
            vec3  lDir     = lightPos - state.position;
            float d        = length(lDir);
            lightIntensity = lightIntensity / (d *d);
                //(LIGHT_QUADRATIC * d * d + LIGHT_LINEAR * d + LIGHT_CONSTANT + 0.0001);
            L              = normalize(-lDir);   
        }
        else // directional light
        { 
            L = normalize(-lightPos);
        }

        vec3 lightIntensitys = vec3(lightIntensity);

        vec3 Li = calcLight(state, viewDir, L, lightIntensitys, lightPdf);

        float isShadowed = 0.0;
        if(light.type == 0)
        {
            
            isShadowed = shadowCalculationPoint(state.position,light);
            
        }
        else if(light.type == 1) // directional light
        {
            vec4 fragPosLightSpace = light.dirLightSpaceMatrix * vec4(state.position, 1.0);
            float bias = 0.005;
            isShadowed = shadowCalculationDir(fragPosLightSpace,bias);
        }



        color +=(1.0 - isShadowed ) * Li;

    }

    // vec3 L = vec3(0); // vector to light
    // float lightIntensity = 0.0;
    // float lightDistance = 100000.0;
    // float lightPdf = 1.0;

    
    // lightIntensity = lightsUni.lights[0].intensity;
    // vec3 lightPos = lightsUni.lights[0].position.xyz;
    // if(lightsUni.lights[0].type == 0)
    // {
    //     vec3  lDir     = lightPos - state.position;
    //     float d        = length(lDir);
    //     lightIntensity = lightIntensity / (d *d);
    //         //(LIGHT_QUADRATIC * d * d + LIGHT_LINEAR * d + LIGHT_CONSTANT + 0.0001);
    //     L              = normalize(-lDir);   
    // }
    // else 
    // { 
    //     L = normalize(-lightPos);
    // }

    // vec3 lightIntensitys = vec3(lightIntensity);

    // vec3 Li = calcLight(state, viewDir, L, lightIntensitys, lightPdf);



    // color += Li;

    color += state.mat.emission;
    vec3 ambient = vec3(0.001) * pcRaster.skyLightIntensity * state.mat.albedo;
    if(pcRaster.needSSAO == 1)
    {
        ambient *= state.mat.ao;
    }
    color += ambient;

    color = DebugInfo(state, color);

    outColor = vec4(color, 1.0);
}