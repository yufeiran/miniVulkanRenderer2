#ifndef LAYOUTS_GLSL
#define LAYOUTS_GLSL 1



#include "deviceDataStruct.h"

layout(push_constant) uniform _PushConstantRay {PushConstantRay pcRay; };
layout(location = 0) rayPayloadEXT hitPayload prd;
layout(location = 1) rayPayloadEXT shadowPayload prdShadow;

layout(set = 0, binding = eTlas) uniform accelerationStructureEXT topLevelAS;
layout(set = 0, binding = eOutImage, rgba32f) uniform image2D image;

layout(set = 1, binding = eGlobals) uniform _GlobalUniforms { GlobalUniforms uni; };
layout(set = 1, binding = eObjDescs, scalar) buffer ObjDesc_ { ObjDesc i[]; } objDesc;
layout(set = 1, binding = eTextures) uniform sampler2D[] textureSamplers;
layout(set = 1, binding = eCubeMap) uniform samplerCube cubeMapTexture;


layout(set = 1, binding = eLight,std140) uniform _LightUniforms {LightUniforms lightsUni;};


layout(buffer_reference, scalar) buffer Vertices {Vertex v[]; };
layout(buffer_reference, scalar) buffer Indices {ivec3 i[]; };
layout(buffer_reference, scalar) buffer Materials { GltfShadeMaterial m[]; };
layout(buffer_reference, scalar) buffer MatIndices { int i[]; };


#endif // LAYOUT_GLSL