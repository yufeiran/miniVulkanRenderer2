// all Gltf sampling end evaluation methods

#ifndef PBR_GLTF_GLSL
#define PBR_GLTF_GLSL 1 

#include "random.glsl"

float clampedDot(vec3 x, vec3 y)
{
    return clamp(dot(x, y), 0.0, 1.0);
}

vec3 CosineSampleHemisphere(float r1, float r2)
{
  vec3  dir;
  float r   = sqrt(r1);
  float phi = TWO_PI * r2;
  dir.x     = r * cos(phi);
  dir.y     = r * sin(phi);
  dir.z     = sqrt(max(0.0, 1.0 - dir.x * dir.x - dir.y * dir.y));

    return dir;
}

// The following equation models the Fresnel reflectance term of the spec equation (aka F())
// Implementation of fresnel from [4], Equation 15
vec3 F_Schlick(vec3 f0, vec3 f90, float VdotH)
{
    return f0 + (f90 - f0) * pow(clamp(1.0 - VdotH, 0.0, 1.0), 5.0);
}

float F_Schlick(float f0, float f90, float VdotH)
{
    return f0 + (f90 - f0) * pow(clamp(1.0 - VdotH, 0.0, 1.0), 5.0);
}

// The following equation(s) model the distribution of microfacet normals across the area being drawn (aka D())
// Implementation from "Average Irregularity Representation of a Roughened Surface for Ray Reflection" by T. S. Trowbridge, and K. P. Reitz
// Follows the distribution function recommended in the SIGGRAPH 2013 course notes from EPIC Games [1], Equation 3.
// Normal distribution function
float D_GGX(float NdotH, float alphaRoughness)
{
    float alphaRoughnessSq = alphaRoughness * alphaRoughness;
    float f                = (NdotH * NdotH) * (alphaRoughnessSq - 1.0) + 1.0;
    return alphaRoughnessSq / (M_PI * f *f);
}

// Smith Joint GGX 
// Note: Vis = G / (4 * NdotL * NdotV)
// ? Geometry function
float V_GGX(float NdotL, float NdotV, float alphaRoughness)
{
    float alphaRoughnessSq = alphaRoughness * alphaRoughness;

    float GGXV = NdotL * sqrt(NdotV * NdotV * (1.0 - alphaRoughnessSq) + alphaRoughnessSq);
    float GGXL = NdotV * sqrt(NdotL * NdotL * (1.0 - alphaRoughnessSq) + alphaRoughnessSq);

    float GGX = GGXV + GGXL;
    if(GGX > 0.0)
    {
        return 0.5 / GGX;
    }
    return 0.0;
}

//  https://github.com/KhronosGroup/glTF/tree/master/specification/2.0#acknowledgments AppendixB
vec3 BRDF_specularGGX(vec3 f0, vec3 f90, float alphaRoughness, float VdotH, float NdotL, float NdotV, float NdotH)
{
    vec3  F = F_Schlick(f0, f90, VdotH);
    float V = V_GGX(NdotL, NdotV, alphaRoughness);
    float D = D_GGX(NdotH, max(0.001, alphaRoughness));

    return F * V * D; 
}

//https://github.com/KhronosGroup/glTF/tree/master/specification/2.0#acknowledgments AppendixB
vec3 BRDF_lambertian(vec3 f0, vec3 f90, vec3 diffuseColor, float VdotH, float metallic)
{
    return (1.0 - metallic) * (diffuseColor / M_PI);
}

vec3 GgxSampling(float specularAlpha, float r1, float r2)
{
    float phi = r1 * 2.0 * M_PI;

    float cosTheta = sqrt((1.0 - r2) / (1.0 + (specularAlpha * specularAlpha - 1.0) *r2));
    float sinTheta = clamp(sqrt(1.0 - (cosTheta * cosTheta)), 0.0, 1.0);
    float sinPhi   = sin(phi);
    float cosPhi   = cos(phi);

    return vec3(sinTheta * cosPhi, sinTheta * sinPhi, cosTheta);
}

vec3 EvalDiffuseGltf(State state, vec3 f0, vec3 f90, vec3 V, vec3 N, vec3 L, vec3 H, out float pdf)
{
    pdf          = 0;
    float NdotV  = dot(N, V);
    float NdotL  = dot(N, L);
            // brdf = state.mat.albedo / M_PI * (1.0 - state.mat.metallic);
            // pdf = dot(L, N) / M_PI;

    NdotL = abs(NdotL);
    NdotV = abs(NdotV);

    NdotL = clamp(abs(NdotL), 0.0000000000000000000001, 1.0);
    NdotV = clamp(abs(NdotV), 0.0000000000000000000001, 1.0);

    float VdotH = dot(V, H);

    pdf = dot(L, N)  * M_1_OVER_PI;

    return (1.0 - state.mat.metallic) * (state.mat.albedo.xyz / M_PI);

}


vec3 EvalSpecularGltf(State state, vec3 f0, vec3 f90, vec3 V, vec3 N, vec3 L, vec3 H, out float pdf)
{
    // TODO: Add anisotropy

    pdf         = 0;
    float NdotL = dot(N, L);

    if(NdotL < 0.0)
        return vec3(0.0);
    
    float NdotV = dot(N, V);
    float NdotH = clamp(dot(N, H), 0, 1);
    float LdotH = clamp(dot(L, H), 0, 1);
    float VdotH = clamp(dot(V, H), 0, 1);

    NdotL = clamp(NdotL, 0.001, 1.0);
    NdotV = clamp(abs(NdotV), 0.001, 1.0);

    pdf = D_GGX(NdotH, state.mat.roughness) * NdotH / (4.0 * LdotH);
    return BRDF_specularGGX(f0, f90, state.mat.roughness, VdotH, NdotL, NdotV, NdotH);

}



vec3 PbrSample(in State state, vec3 V, vec3 N, inout vec3 L, inout float pdf, inout uint seed)
{
    pdf       = 0.0;
    vec3 brdf = vec3(0.0);

    float probability   = rnd(seed);
    float diffuseRatio  = 0.5 * (1.0 - state.mat.metallic);
    float specularRatio = 1.0 - diffuseRatio;
    float transWeight   = (1.0 - state.mat.metallic) * state.mat.transmission;

    float r1 = rnd(seed);
    float r2 = rnd(seed);

    // TODO: ADD Transmission
    // https://viclw17.github.io/2018/08/05/raytracing-dielectric-materials
    if(rnd(seed) < transWeight)
    {
        float eta = state.eta;

        float n1          = 1.0;
        float n2          = state.mat.ior;
        float R0          = (n1 - n2) / (n1 + n2);
        vec3  H           = GgxSampling(state.mat.roughness, r1, r2);
        H                 = state.tangent * H.x + state.bitangent * H.y + N * H.z;
        float VdotH       = dot(V,H);
        float F           = F_Schlick(R0 * R0, 1.0, VdotH);   // Reflection
        float discriminat = 1.0 - eta * eta * (1.0 - VdotH * VdotH); // (Total internal reflection)

        // Reflection/Total internal reflection
        if(discriminat < 0.0 || rnd(seed) < F)
        {
            L = normalize(reflect(-V, H));
        }
        else 
        {
            // Find the pure refeactive ray
            L = normalize(refract(-V, H, eta));

            // Cought rays perpendicular to surface, and simply continue
            if(isnan(L.x) || isnan(L.y) || isnan(L.z))
            {
                L = -V;
            }
        }
        
        // Transmission
        pdf = abs(dot(N, L));
        brdf = state.mat.albedo;
    }
    // Normal Scattering
    else{
        vec3  specularCol = state.mat.f0;
        float reflectance = max(max(specularCol.r, specularCol.g),specularCol.b);
        vec3  f0          = specularCol.rgb;
        vec3  f90         = vec3(clamp(reflectance * 50.0 , 0.0, 1.0));

        vec3 T = state.tangent;
        vec3 B = state.bitangent;

        // f_r = k_d * f_lambert + k_s * f_cook-torrance

        // cal f_lambert
        if(probability < diffuseRatio) // sample diffuse
        {
            L = CosineSampleHemisphere(r1, r2);
            L = L.x * T + L.y * B + L.z * N;

            vec3 H = normalize(L + V);

            // brdf = state.mat.albedo / M_PI * (1.0 - state.mat.metallic);
            // pdf = dot(L, N) / M_PI;
            brdf = EvalDiffuseGltf(state, f0, f90, V, N, L, H, pdf);
            //pdf *= (1.0 - state.mat.subsurface) * diffuseRatio;
            //pdf *= 1.0 * diffuseRatio;
        }
        // cal f_cook-torrance
        else 
        {
            // TODO: support clearcoat!
            float roughness;
            roughness = state.mat.roughness;

            vec3 H = GgxSampling(roughness, r1, r2);
            H      = T * H.x + B * H.y + N * H.z;
            L      = reflect(-V, H);


            brdf = EvalSpecularGltf(state, f0, f90, V, N, L, H, pdf);
            pdf *= specularRatio;
            
        }

        // brdf *= (1.0 - transWeight);
        // pdf  *= (1.0 - transWeight);

    }
    return brdf;

}

#endif // PBR_GLTF_GLSL