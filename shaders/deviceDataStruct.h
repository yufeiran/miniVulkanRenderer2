// 用来存同步在GPU内存和CPU内存的结构,比如uniform结构
#ifndef COMMON_DEVICE_DATA_STRUCT
#define COMMON_DEVICE_DATA_STRUCT

#ifdef __cplusplus
#include"../src/Common/common.h"
using vec2 = glm::vec2;
using vec3 = glm::vec3;
using vec4 = glm::vec4;
using mat4 = glm::mat4;
using uint = unsigned int;
#endif

#ifdef __cplusplus
	#define START_ENUM(a) enum a{ 
	#define END_ENUM() }
#else 
	#define START_ENUM(a) const uint
	#define END_ENUM() 
#endif

START_ENUM(SceneBindings)
	eGlobals         = 0,
	eObjDescs        = 1,
	eTextures        = 2,
	eCubeMap         = 3,
	eDirShadowMap    = 4,
	ePointShadowMap  = 5,
	eLight           = 6,
	eSSAOSamples	 = 7
END_ENUM();

START_ENUM(PBBloomBindings)
	epbbloomInput = 0,
	epbInputSize = 1
END_ENUM();


START_ENUM(RtBindings)
	eTlas     = 0,  // Top-level acceleration structure
	eOutImage = 1
END_ENUM();

START_ENUM(GBufferType)
	eGPosition          = 0,
	eGNormal            = 1,
	eGAlbedo            = 2,
	eGMetalRough        = 3,
	eGEmission          = 4,
	eGSSAO              = 5,
	eGSSAOBlur          = 6,
	eGPositionViewSpace = 7,
	eGNormalViewSpace   = 8,
	eGDepth             = 9
END_ENUM();

START_ENUM(SSAOBindings)
	eSSAOSAMPLE = 0,
	eSSAONoise  = 1,
	eSSAOPosition   = 2,
	eSSAONormal     = 3,
	eSSAOAlbedo     = 4,
	eSSAODepth      = 5
END_ENUM();

START_ENUM(SSAOBlurBindings)
    eSSAOInput = 0
END_ENUM();

START_ENUM(DebugMode)
	eNoDebug   = 0,  //
	eBaseColor = 1,  //
	eNormal    = 2,  //
	eMetallic  = 3,  //
	eEmissive  = 4,  //
	eAlpha     = 5,  //
	eRoughness = 6,  //
	eTexCoord  = 7,  //
	eTangent   = 8,  //
	eBitangent = 9,  //
	eSpecular  = 10, //
	eSSAO      = 11  //

END_ENUM();

struct ObjDesc
{
	int txtOffset;
	uint64_t vertexAddress;
	uint64_t indexAddress;
	uint64_t materialAddress;
	uint64_t materialIndexAddress;
};

struct LightDesc
{
	vec4 position;
	vec4 color;
	vec4 direction;

	mat4  dirLightSpaceMatrix;
	mat4  pointLightSpaceMatrix[6];

	float intensity;

	int type; // 0:directional 1:point 2:spot

	float nearPlane;
	float farPlane;
};

#define MAX_LIGHTS 99
struct LightUniforms
{
	int       lightCount;
	int       pointShadowIndex;
	int       dirShadowIndex;
	int       spotShadowIndex;

	LightDesc lights[MAX_LIGHTS];
};



struct GlobalUniforms
{
	mat4 viewProj;
	mat4 view;
	mat4 viewNoTranslate;
	mat4 proj;
	mat4 viewInverse;
	mat4 projInverse;

};

struct Vertex
{
	vec3 pos;
	vec3 normal;
	vec4 color;
	vec3 tangent;
	vec3 bitangent;
	vec2 texCoord;
};



struct PushConstantRaster
{
	mat4  modelMatrix;
	int  objIndex;
	float skyLightIntensity;
	int   debugMode;      // 0:no degbug 1:normal 
	int   screenWidth;
	int   screenHeight;
	int   needSSAO;   // 0: no 1:yes
	int   objType;    // 0:normal 1: light
	int   lightIndex; // light index
};

struct PushConstantRay
{
	vec4  clearColor;
	vec3  lightPosition;
	float lightIntensity;
	int   lightType;
	int   frame;
	int   nbSample;
	int   maxDepth;
	int   pbrMode;        // 0-Disney, 1-Gltf
	int   debugMode;      // 0:no degbug 1:normal 
	float skyLightIntensity; 
	int   totalFrameCount;
};

struct PushConstantPost
{
	float exposure;
	int   debugShadowMap;
	int   debugBloom;
	float pbbloomRadius;
	float pbbloomIntensity;
	int   pbbloomMode; // 0:off 1:on
};

#define MATERIAL_METALLICROUGHNESS 0
#define MATERIAL_SPECULARGLOSSINESS 1
#define ALPHA_OPAQUE 0
#define ALPHA_MASK 1
#define ALPHA_BLEND 2
struct GltfShadeMaterial
{

	int type; // type = 0  Blinn-Phong type = 1 PBR

	// Blinn-Phong
	vec3  ambient;

	vec3  diffuse;
	vec3  specular;
	vec3  transmittance;
	vec3  emission;

	float shininess;

	float dissolve;
	int   illum;

	int   textureId;


	// PBR
	vec4  pbrBaseColorFactor;

    int   pbrBaseColorTexture;
	int   pbrOcclusionTexture;

	float pbrMetallicFactor;
	float pbrRoughnessFactor;
	int   pbrMetallicRoughnessTexture;
	float pbrOcclusionTextureStrength;

	vec4  khrDiffuseFactor;   // KHR_materials_pbrSpecularGlossiness
	vec3  khrSpecularFactor;
	int   khrDiffuseTexture;

	int   shadingModel; // 0: metallic-roughness, 1: specular-glossiness
	float khrGlossinessFactor;
	int   khrSpecularGlossinessTexture;
	int   emissiveTexture;

	vec3  emissiveFactor; 
	int   alphaMode;

	float alphaCutoff;
	int   doubleSided;
	int   normalTexture;
	float normalTextureScale;

	mat4  uvTransform;

	int   unlit;

	float transmissionFactor;
	int   transmissionTexture;

	float ior;

	vec3  anisotropyDirection;
	float anisotropy;

	vec3  attenuationColor ;
	float thicknessFactor;
	int   thicknessTexture;
	float attenuationDistance;

	float clearcoatFactor;
	float clearcoatRoughness;

	int   clearcoatTexture;
	int   clearcoatRoughnessTexture;
	vec4  sheen;
	int   pad;
};

// for raytracing


#endif