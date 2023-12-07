#ifndef GLTFMATERIAL_GLSL
#define GLTFMATERIAL_GLSL 1 

#include "random.glsl"
#include "deviceDataStruct.h"
#include "layouts.glsl"

vec4 SRGBtoLINEAR(vec4 srgbIn)
{
    vec3 linOut = pow(srgbIn.xyz, vec3(2.2));

    return vec4(linOut, srgbIn.w);
}


// get diffuse and specular color base on the shading model: Metal-Roughness or Specular-Glossiness
void GetMetallicRoughness(inout State state, in GltfShadeMaterial material, int txtOffset)
{
    // KHR_materials_ior
    float dielectricSpecular = (material.ior - 1) / (material.ior + 1);
    dielectricSpecular *=  dielectricSpecular;

    float perceptualRoughness = 0.0;
    float metallic            = 0.0;
    vec4  baseColor           = vec4(0.0, 0.0, 0.0, 1.0);
    vec3  f0                  = vec3(dielectricSpecular);

    perceptualRoughness = material.pbrRoughnessFactor;
    metallic            = material.pbrMetallicFactor;
    if(material.pbrMetallicRoughnessTexture > -1)
    {
        // Roughness is stored in the 'g' channel, metallic is stored in the 'b' channel.
        // This layout intentionally reserves the 'r' channel for (optional) occlusion map data
        vec4 mrSample = textureLod(textureSamplers[nonuniformEXT(material.pbrMetallicRoughnessTexture)], state.texCoord, 0);
        perceptualRoughness = mrSample.g * perceptualRoughness;
        metallic            = mrSample.b * metallic;
    }

    // get albedo
    baseColor = material.pbrBaseColorFactor;
    if(material.pbrBaseColorTexture > -1)
    {
        baseColor *= SRGBtoLINEAR(textureLod(textureSamplers[nonuniformEXT(material.pbrBaseColorTexture)], state.texCoord, 0));
    }

    f0 = mix(vec3(dielectricSpecular), baseColor.xyz, metallic);

    state.mat.albedo    = baseColor.xyz;
    state.mat.metallic  = metallic;
    state.mat.roughness = perceptualRoughness;
    state.mat.f0        = f0;
    state.mat.alpha     = baseColor.a;
}


void GetMaterialsAndTextures(inout State state,in hitPayload prd, in Ray r)
{

        // Object data
    ObjDesc    objResource = objDesc.i[prd.instanceCustomIndex];
    MatIndices matIndices  = MatIndices(objResource.materialIndexAddress);
    Materials  materials   = Materials(objResource.materialAddress);
    Indices    indices     = Indices(objResource.indexAddress);
    Vertices   vertices    = Vertices(objResource.vertexAddress);

    int txtOffset = objResource.txtOffset;

    GltfShadeMaterial material = materials.m[state.matID];

    state.mat.specular      = 0.5;
    state.mat.subsurface    = 0;
    state.mat.specularTint  = 1;
    state.mat.sheen         = 0;
    state.mat.sheenTint     = vec3(0);


    // Uv Transform
    //state.texCoord = (vec4(state.texCoord.xy, 1, 1) * material.uvTransform).xy;
    mat3  TBN      = mat3(state.tangent, state.bitangent, state.normal);

    // apply normal map if this material have a nomal map
    if(material.normalTexture > -1)
    {
        vec3 normalVector = textureLod(textureSamplers[nonuniformEXT(material.normalTexture)], state.texCoord, 0).xyz;
        normalVector      = normalize(normalVector * 2.0 - 1.0);
        normalVector  *= vec3(material.normalTextureScale, material.normalTextureScale, 1.0);
        state.normal   = normalize(TBN * normalVector);
        state.ffnormal = dot(state.normal, r.direction) <= 0.0 ? state.normal : -state.normal;
        createCoordinateSystem(state.ffnormal, state.tangent, state.bitangent);
    }

    // Emissive term
    state.mat.emission = material.emissiveFactor;
    if(material.emissiveTexture > -1)
    {
        state.mat.emission *= 
            SRGBtoLINEAR(textureLod(textureSamplers[nonuniformEXT(material.emissiveTexture)], state.texCoord, 0)).rgb;
    }

    // Basic material
    if(material.shadingModel == MATERIAL_METALLICROUGHNESS)
        GetMetallicRoughness(state, material, txtOffset);
    // else 
    //     GetSpecularGlossiness(state, material, txtOffset);

    // Clamping roughness
    state.mat.roughness = max(state.mat.roughness, 0.001);

    // KHR_materials_transmission
    // state.mat.transmission = material.transmissionFactor;
    // if(material.transmissionTexture > -1)
    // {
    //     state.mat.transmission *= textureLod(textureSamplers[nonuniformEXT(txtOffset + material.transmissionTexture)], state.texCoord, 0).r;
    // }

    // KHR_materials_ior
    state.mat.ior = material.ior;
    //state.eta     = dot(state.normal, state.ffnormal) > 0.0 ? (1.0 / state.mat.ior) : state.mat.ior;

    // KHR_materials_unlit 
    // state.mat.unlit = (material.unlit == 1);

    // KHR_materials_anisotropy
    // if(material.anisotropy > 0)
    // {
    //     state.tangent = normalize(TBN * material.anisotropyDirection);
    //     state.bitangent = normalize(cross(state.normal, state.tangent));
    // }

    // KHR_materials_volume
    state.mat.attenuationColor     = material.attenuationColor;
    state.mat.attenuationDistance  = material.attenuationDistance;
    state.mat.thinwalled           = material.thicknessFactor == 0;

    // KHR_materials_clearcoat 
    // state.mat.clearcoat            = material.clearcoatFactor;
    // state.mat.clearcoatRoughness   = material.clearcoatRoughness;
    // if(material.clearcoatTexture > -1)
    // {
    //     state.mat.clearcoat *= textureLod(textureSamplers[nonuniformEXT(txtOffset + material.clearcoatTexture)], state.texCoord, 0).r;
    // }
    // if(material.clearcoatRoughnessTexture > -1)
    // {
    //     state.mat.clearcoatRoughness *= 
    //         textureLod(textureSamplers[nonuniformEXT(txtOffset + material.clearcoatRoughnessTexture)], state.texCoord, 0).g;
    // }
    // state.mat.clearcoatRoughness = max(state.mat.clearcoatRoughness, 0.001);

    // KHR_materials_sheen
    // vec4 sheen          = material.sheen;
    // state.mat.sheenTint = sheen.xyz;
    // state.mat.sheen = sheen.w;


}

#endif // GLTFMATERIAL_GLSL