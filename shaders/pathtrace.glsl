#include "globals.glsl"
#include "random.glsl"
#include "shadeState.glsl"
#include "gltfMaterial.glsl"
#include "pbr_gltf.glsl"


vec3 Sample(in State state, in vec3 V, in vec3 N,inout vec3 L, inout float pdf, inout uint seed)
{
    // if(pcRay.pbrMode == 0){
    //     return 
    // }
    // TODO: Add DisneySmaple
    return PbrSample(state, V, N, L, pdf, seed);
}

void ClosestHit(Ray r)
{
    //uint  rayFlags = gl_RayFlagsOpaqueEXT | gl_RayFlagsCullBackFacingTrianglesEXT;
    uint  rayFlags = gl_RayFlagsCullBackFacingTrianglesEXT;
    prd.hitT       = INFINITY;

    // set min to avoid self intersections
    float tMin     = 0.00;

    traceRayEXT(topLevelAS, // acceleration structure
                rayFlags,       // rayFlags 
                0xFF,           // cullMask
                0,              // sbtRecordOffset
                0,              // sbtRecordStride
                0,              // missIndex
                r.origin,    // ray origin
                tMin,             // ray min range
                r.direction,  // ray direction
                INFINITY,        // ray max range
                0                // payload (location = 0)
    );

}

vec3 DebugInfo(in State state)
{
    switch(pcRay.debugMode)
    {
        case eNormal:
           return (state.normal + vec3(1)) * .5;
        case eMetallic:
           return vec3(state.mat.metallic);
        case eBaseColor:
           return state.mat.albedo;
        case eEmissive:
           return state.mat.emission;
        case eAlpha:
           return vec3(state.mat.alpha);
        case eRoughness:
           return vec3(state.mat.roughness);
        case eTexCoord:
           return vec3(state.texCoord, 0);
        case eTangent:
           return vec3(state.tangent.xyz + vec3(1)) * .5;
        case eBitangent:
           return vec3(state.bitangent.xyz + vec3(1)) * .5;

    }
    return vec3(1000, 0 , 0);
}

vec3 PathTrace(Ray r)
{

    vec3 radiance   = vec3(0.0);
    vec3 throughput = vec3(1.0);
    vec3 absorption = vec3(0.0);

    for(int depth = 0; depth < pcRay.maxDepth; depth++)
    {
        ClosestHit(r);

        // Hitting the environment 
        if(prd.hitT == INFINITY)
        {
            vec3 cubeMapColor = texture(cubeMapTexture,r.direction).xyz;

            float factor;
            if(depth == 0)
            {
                factor =  0.8 ;
            }
            else {
                factor = 0.02  * pcRay.skyLightIntensity; // Tiny contribution from environment;
            }

            return radiance + cubeMapColor * factor * throughput;
        }

        BsdfSampleRec bsdfSampleRec;

        ShadeState sstate = GetShadeState(prd);

        //return vec3(sstate.tangentU[0].xyz + vec3(1)) * .5;

        State state;
        state.position       = sstate.position;
        state.normal         = sstate.normal;
        state.tangent        = sstate.tangentU[0];
        state.bitangent      = sstate.tangentV[0];
        state.texCoord       = sstate.textCoords[0];
        state.matID          = sstate.matIndex;
        state.isEmitter      = false;
        state.specularBounce = false;
        state.isSubsurface   = false;

    


        
        // fill the material 
        GetMaterialsAndTextures(state, prd, r);



        if(pcRay.debugMode !=  eNoDebug)
        {
            return DebugInfo(state);
        }

        // Color at vertices 
        state.mat.albedo *= sstate.color;



        // KHR_materials_unlit :use this can shading without light!
        // if(state.mat.unlit)
        // {
        //     return radiance + state.mat.albedo * throughput;
        // }

        // Reset absorption when ray is going out of surface
        // if(dot(state.normal, state.ffnormal) > 0.0)
        // {
        //     absorption = vec3(0.0);
        // }

        // Emissive
        radiance += state.mat.emission * throughput;

        // Add absoption (transmission / volume)
        throughput *= exp(-absorption * prd.hitT);

        // Light and environment contribution
        // TODO:add cubemap light

        // Sampling for the next ray
        // find next direction and weight
        bsdfSampleRec.f = Sample(state, -r.direction, state.ffnormal, bsdfSampleRec.L, bsdfSampleRec.pdf, prd.seed);

        // Set absorption only if the ray is currently inside the object
        // if(dot(state.ffnormal, bsdfSampleRec.L) < 0.0)
        // {
        //     absorption = - log(state.mat.attenuationColor) / vec3(state.mat.attenuationDistance);
        // }


        if(bsdfSampleRec.pdf > 0.0)
        {
            throughput *= bsdfSampleRec.f * abs(dot(state.ffnormal, bsdfSampleRec.L)) / bsdfSampleRec.pdf;
        }
        else 
        {
            break;
        }

        // Direction Light


        r.direction = bsdfSampleRec.L;
        //r.origin    = sstate.position;
        r.origin    = OffsetRay(sstate.position, dot(bsdfSampleRec.L, state.ffnormal) > 0 ? state.ffnormal : -state.ffnormal );
        //r.origin = OffsetRay(sstate.position, dot(bsdfSampleRec.L, state.ffnormal) > 0 ? state.ffnormal : -state.ffnormal);


    }

    return radiance;

}

vec3 samplePixel(ivec2 imageCoords, ivec2 sizeImage)
{
    vec3 pixelColor = vec3(0);

    float r1 = rnd(prd.seed);
    float r2 = rnd(prd.seed);
    // Subpixel jitter: send the ray through a different position inside the pixel 
    // each time, to provide antialiasing
    vec2 subpixel_jtter = pcRay.frame == 0 ? vec2(0.5f, 0.5f) : vec2(r1, r2);

    const vec2 pixelCenter = vec2(imageCoords) + subpixel_jtter;
    const vec2 inUV = pixelCenter/vec2(sizeImage);
    vec2 d = inUV * 2.0 - 1.0;

    vec4 originView     = vec4(0, 0, 0, 1);
    vec4 originWorld    = uni.viewInverse * originView;
    vec4 targetView     = uni.projInverse * vec4(d.x, d.y, 1, 1);
    vec4 direction      = uni.viewInverse * vec4(normalize(targetView.xyz - originView.xyz), 0);

    Ray ray = Ray(originWorld.xyz, direction.xyz);

    vec3 radiance = PathTrace(ray);

    return radiance;
    
}