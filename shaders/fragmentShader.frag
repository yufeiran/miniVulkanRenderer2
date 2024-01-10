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
#include "pbr_gltf.glsl"


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
layout(location = 5) in vec4 inTangent;

layout(location = 0) out vec4 outColor;

vec4 SRGBtoLINEAR(vec4 srgbIn)
{
    vec3 linOut = pow(srgbIn.xyz, vec3(2.2));

    return vec4(linOut, srgbIn.w);
}


float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

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
        lightIntensity = pcRaster.lightIntensity / (d *d);
            //(LIGHT_QUADRATIC * d * d + LIGHT_LINEAR * d + LIGHT_CONSTANT + 0.0001);
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
    if(mat.type == 1)
    {
        // get normal 
        vec3 tangent   = (pcRaster.modelMatrix * inTangent).xyz;
        vec3 bitangent = cross(inWorldNormal, tangent) * inTangent.w;
        vec3 normal    = inWorldNormal;
        
        mat3  TBN      = mat3(tangent, bitangent, inWorldNormal);
        //apply normal map if this material have a nomal map
        if(mat.normalTexture > -1)
        {
            vec3 normalVector = textureLod(textureSamplers[nonuniformEXT(mat.normalTexture)], inTexCoord, 0).xyz;
            normalVector      = normalize(normalVector * 2.0 - 1.0);
            normalVector  *= vec3(mat.normalTextureScale, mat.normalTextureScale, 1.0);
            normal   = normalize(TBN * normalVector);
        }


        // get texture  
        vec3 albedo = mat.pbrBaseColorFactor.xyz;
        if(mat.pbrBaseColorTexture >= 0)
        {
            //uint txtId    = mat.pbrBaseColorTexture + objDesc.i[pcRaster.objIndex].txtOffset;
            uint txtId    = mat.pbrBaseColorTexture;
            vec3 albedoTxt = SRGBtoLINEAR(texture(textureSamplers[nonuniformEXT(txtId)], inTexCoord)).xyz;
            albedo *=albedoTxt;
        }
        float metallic = mat.pbrMetallicFactor;
        float roughness = mat.pbrRoughnessFactor;
        float ao        = mat.pbrOcclusionTextureStrength;
        vec3 emission  = mat.emissiveFactor;

        float aoTemp = 1.0;
        if(mat.pbrMetallicRoughnessTexture > -1)
        {
            // Roughness is stored in the 'g' channel, metallic is stored in the 'b' channel.
            // This layout intentionally reserves the 'r' channel for (optional) occlusion map data
            vec4 mrSample = textureLod(textureSamplers[nonuniformEXT(mat.pbrMetallicRoughnessTexture)], inTexCoord, 0);
            roughness     = mrSample.g * roughness;
            metallic      = mrSample.b * metallic;
            aoTemp        = mrSample.r;

        }
        if(mat.pbrOcclusionTexture > -1)
        {
            ao = texture(textureSamplers[nonuniformEXT(mat.pbrOcclusionTexture)], inTexCoord).r;
        }
        else 
        {
            ao = aoTemp;
        }

        // Emissive term
        if(mat.emissiveTexture > -1)
        {
            emission *= 
                textureLod(textureSamplers[nonuniformEXT(mat.emissiveTexture)], inTexCoord, 0).rgb;
        }


        vec3 N = normalize(normal);
        vec3 V = normalize(inViewDir);
        vec3 H = normalize(L + V);

        // calculate reflectance: if dia-electric use F0 
        // of 0.04 and if it's a metal, use the albedo color as F0
        vec3 F0 = vec3(0.04);
        F0 = mix(F0, albedo, metallic);
        vec3  specularCol = F0;
        float reflectance = max(max(specularCol.r, specularCol.g),specularCol.b);
        vec3  F90         = vec3(clamp(reflectance * 50.0 , 0.0, 1.0));

        // reflectance equation
        vec3 L0 = vec3(0.0);

        float NdotH = max(dot(N, H), 0.0);
        float NdotL = max(dot(N, L), 0.0);
        float NdotV = max(dot(N, V), 0.0);
        float VdotH = max(dot(V, H), 0.0);

        
        // Cook-Torrance BRDF
        float NDF = D_GGX(NdotH, roughness);
        float G   = V_GGX(NdotL, NdotV, roughness);
        vec3  F   = F_Schlick(F0, F90, VdotH);
        // float NDF = DistributionGGX(N, H, roughness);   
        // float G   = GeometrySmith(N, V, L, roughness);      
        // vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);

        vec3 numerator = NDF * G * F;
        float denominator =  4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001; // prevent divide by zero.
        

        
        vec3 specular = numerator;



        // outColor = vec4(specular, 1.0);
        // outColor = vec4(lightIntensity,lightIntensity,lightIntensity,0);
        // return;


        // kS is equal to Fresnel
        vec3 kS = F;

        // energy conservation
        vec3 kD = vec3(1.0) - kS;

        // metallic can't reflect
        kD *= 1.0 - metallic;

        L0 += (kD * albedo / PI + specular) * lightIntensity * NdotL;

        vec3 ambient = vec3(0.002) * pcRaster.skyLightIntensity * albedo * ao;

        vec3 color = ambient + L0 + emission;

        outColor = vec4(color, 1.0);


        // // // Diffuse 
        // vec3 diffuse = computeDiffuse(mat, L, N);
        // if(mat.pbrBaseColorTexture >= 0)
        // {
        //     //uint txtId    = mat.pbrBaseColorTexture + objDesc.i[pcRaster.objIndex].txtOffset;
        //     uint txtId    = mat.pbrBaseColorTexture;
        //     vec3 diffuseTxt = texture(textureSamplers[nonuniformEXT(txtId)], inTexCoord).xyz;
        //     diffuse *=diffuseTxt;

        // }
        // else 
        // {
        //     diffuse *= mat.pbrBaseColorFactor.xyz;
        // }

        // // Specular 
        // vec3 specular = computeSpecular(mat, inViewDir, L, N);

        // // vec3 specular = vec3(0.0, 0.0, 0.0);;
        // // vec3 diffuse = vec3(1.0, 1.0, 1.0);

        // outColor = vec4(lightIntensity * (diffuse + specular), 1);

        //outColor = vec4(inTexCoord,0.0,1);
        //outColor = vec4(vec3(inTangent.xyz + vec3(1)) * .5,0);
       
    }
    // outColor = vec4(1.0, 1.0, 1.0, 1.0);
    // outColor = vec4(inTexCoord,0.0,1);

}