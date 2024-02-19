// use for find geometric information used for shader

#ifndef SHADE_STATE_GLSL
#define SHADE_STATE_GLSL  1

#include "layouts.glsl"
#include "globals.glsl"

struct ShadeState 
{
    vec3 normal;
    vec3 geomNormal;
    vec3 position;
    vec2 textCoords[1];
    vec3 tangentU[1];
    vec3 tangentV[1];
    vec3 color;
    uint matIndex;
};

ShadeState GetShadeState(in hitPayload hstate)
{
    ShadeState sstate;

    const uint idGeo  = hstate.instanceCustomIndex;  // Geometry Index of this instance
    const uint idPrim = hstate.primitiveID;          // Triangle ID in this geometry

    
    // Object data
    ObjDesc    objResource = objDesc.i[idGeo];
    MatIndices matIndices  = MatIndices(objResource.materialIndexAddress);
    Materials  materials   = Materials(objResource.materialAddress);
    Indices    indices     = Indices(objResource.indexAddress);
    Vertices   vertices    = Vertices(objResource.vertexAddress);

    // Indices of the triangle
    ivec3 ind = indices.i[idPrim];

    // Vertex of the triangle
    Vertex v0 = vertices.v[ind.x];
    Vertex v1 = vertices.v[ind.y];
    Vertex v2 = vertices.v[ind.z];

    // Material of the object 
    int                matIdx = matIndices.i[idPrim];

    const vec3 barycentrics = vec3(1.0 - hstate.baryCoord.x - hstate.baryCoord.y, hstate.baryCoord.x, hstate.baryCoord.y);

    // Computing the coordinates of the hit position
    const vec3 pos      = v0.pos * barycentrics.x + v1.pos * barycentrics.y + v2.pos * barycentrics.z;
    const vec3 worldPos = vec3(hstate.objectToWorld * vec4(pos, 1.0)); // Transforming the position to world space

    // Computing the normal at hit position 
    vec3 normal      = normalize(v0.normal * barycentrics.x + v1.normal * barycentrics.y + v2.normal * barycentrics.z);
    vec3 worldNormal = normalize(vec3(normal * hstate.worldToObject)); // Transforming the normal to world space
    vec3 geomNormal  = normalize(cross(v1.pos - v0.pos, v2.pos - v0.pos));
    vec3 wgeoNormal  = normalize(vec3(geomNormal * hstate.worldToObject)); // ?? why

    vec3 tangent       = normalize(v0.tangent.xyz * barycentrics.x + v1.tangent.xyz * barycentrics.y + v2.tangent.xyz * barycentrics.z);
    vec3 worldTangent  = normalize(vec3(mat4(hstate.objectToWorld) *  vec4(tangent.xyz, 0))); // Transforming the tangent to world space
    //vec3 worldTangent  = normalize(vec3(tangent * hstate.worldToObject)); // Transforming the tangent to world space (faster than above line, but not sure if it's correct or not
    
    worldTangent       = normalize(worldTangent - dot(worldTangent , worldNormal) * worldNormal);
    // vec3 worldBinormal = cross(worldNormal, worldTangent) * v0.tangent.w;
    vec3 bitangent     = normalize(v0.bitangent.xyz * barycentrics.x + v1.bitangent.xyz * barycentrics.y + v2.bitangent.xyz * barycentrics.z);
    vec3 worldBinormal = normalize(vec3(mat4(hstate.objectToWorld) *  vec4(bitangent.xyz, 0))); // Transforming the bitangent to world space
    //vec3 worldBinormal = normalize(vec3(bitangent * hstate.worldToObject)); // Transforming the bitangent to world space

    vec4 color      = v0.color * barycentrics.x + v1.color * barycentrics.y + v2.color * barycentrics.z;


    vec2 texCoord = v0.texCoord * barycentrics.x + v1.texCoord * barycentrics.y + v2.texCoord * barycentrics.z;

    

    sstate.normal        = worldNormal;
    sstate.geomNormal    = wgeoNormal;
    sstate.position      = worldPos;
    sstate.textCoords[0] = texCoord;
    sstate.tangentU[0]   = worldTangent;
    sstate.tangentV[0]   = worldBinormal;
    sstate.color         = color.rgb;
    sstate.matIndex      = matIdx;


    // move normal to same side as geometric normal
    if(dot(sstate.normal, sstate.geomNormal) <= 0)
    {
        //sstate.normal *= -1.0f;
        sstate.normal *= -1.0f;
    }

    return sstate;
}



#endif // SHADE_STATE_GLSL