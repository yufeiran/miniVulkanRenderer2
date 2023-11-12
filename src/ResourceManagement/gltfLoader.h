#pragma once
#include "Common/common.h"
#include "tiny_gltf.h"

#define KHR_LIGHTS_PUNCTUAL_EXTENSION_NAME "KHR_lights_punctual"

struct GltfMaterial
{
	int shadingModel{0}; // 0: metallic-roughness, 1: specular-glossiness

	// pbrMetallicRoughness
	glm::vec4 baseColorFactor{1.f, 1.f, 1.f, 1.f};
	int       baseColorTexture{-1};
	float     metallicFactor{1.f};
	float     roughnessFactor{1.f};
	int       metallicRoughnessTexture{-1};

	int       emissiveTexture{-1};
	glm::vec3 emissiveFactor{0, 0, 0};
	int       alphaMode{0};
	float     alphaCutoff{0.5f};
	int       doubleSided{0};

	int    normalTexture{-1};
	float  normalTextureScale{1.f};
	int    occlusionTexture{-1};
	float  occlusionTextureStrength{1};

	// Tiny Reference
	const tinygltf::Material* tmaterial{nullptr};
};

struct GltfNode
{
	glm::mat4             worldMatrix{1};
	int                   primMesh{0};
	const tinygltf::Node* tnode{nullptr};
};

struct GltfPrimMesh
{
	uint32_t firstIndex{0};
	uint32_t indexCount{0};
	uint32_t vertexOffset{0};
	uint32_t vertexCount{0};
	int      materialIndex{0};

	glm::vec3   posMin{0, 0, 0};
	glm::vec3   posMax{0, 0, 0};
	std::string name;

	// Tiny Reference
	const tinygltf::Mesh*      tmesh{nullptr};
	const tinygltf::Primitive* tprim{nullptr};
};

struct GltfCamera
{
	glm::mat4  worldMatrix{1};
	glm::vec3  eye{0, 0, 0};
	glm::vec3  center{0, 0, 0};
	glm::vec3  up{0, 1, 0};

	tinygltf::Camera cam;
};

struct GltfLight
{
	glm::mat4       worldMatrix{1};
	tinygltf::Light light;
};


enum class GltfAttributes: uint8_t 
{
	NoAttribs  = 0,
	Position   = 1,
	Normal     = 2,
	Texcoord_0 = 4,
	Texcoord_1 = 8,
	Tangent    = 16,
	Color_0    = 32,
	All        = 0xFF
};

using GltfAttributes_t = std::underlying_type_t<GltfAttributes>;

inline GltfAttributes operator|(GltfAttributes lhs, GltfAttributes rhs)
{
	return static_cast<GltfAttributes>(static_cast<GltfAttributes_t>(lhs) | static_cast<GltfAttributes_t>(rhs));
}

inline GltfAttributes operator&(GltfAttributes lhs, GltfAttributes rhs)
{
	return static_cast<GltfAttributes>(static_cast<GltfAttributes_t>(lhs) & static_cast<GltfAttributes_t>(rhs));
}

class GltfLoader
{
public:
	void loadScene(const std::string& filename, glm::mat4 transform = glm::mat4{1});

	// Import all materials in a vector of GltfMaterial structure
	void importMaterials(const tinygltf::Model& tmodel);

	// Import all Mesh and primitives in a vector of GltfPrimMesh
	void importDrawableNodes(const tinygltf::Model& tmodel,
							 glm::mat4              transform,
		                     GltfAttributes         requestedAttributes,
							 GltfAttributes         forceRequested = GltfAttributes::All);

	


	// Scene data
	std::vector<GltfMaterial> materials;   // Material for shading
	std::vector<GltfNode>     nodes;       // Drawable nodes, flat hierarchy
	std::vector<GltfPrimMesh> primMeshes;  // Primitive promoted to meshes
	std::vector<GltfCamera>   cameras;
	std::vector<GltfLight>    lights;

	// Attributes, all same length if valid
	std::vector<glm::vec3>    positions;
	std::vector<uint32_t>     indices;
	std::vector<glm::vec3>    normals;
	std::vector<glm::vec4>    tangents;
	std::vector<glm::vec2>    texcoords0;
	std::vector<glm::vec2>    texcoords1;
	std::vector<glm::vec4>    colors0;

	tinygltf::Model    tmodel;


	struct Dimensions
	{
		glm::vec3 min = glm::vec3(std::numeric_limits<float>::max());
		glm::vec3 max = glm::vec3(std::numeric_limits<float>::min());
		glm::vec3 size{0.f};
		glm::vec3 center{0.f};
		float     radius{0};
	} dimensions;


private:
	void processNode(const tinygltf::Model& tmodel, int& nodeIdx, const glm::mat4& parentMatrix);
	void processMesh(const tinygltf::Model&       tmodel,
		             const tinygltf::Primitive&   tmesh,
		             GltfAttributes               requestedAttributes,
		             GltfAttributes               forceRequested,
		             const std::string&           name);

	void computeSceneDimensions();
	void computeCamera();

	// Temp data
	std::unordered_map<int, std::vector<uint32_t>> meshToPrimMeshes;
	std::vector<uint32_t>                          primitiveIndices32u;
	std::vector<uint16_t>                          primitiveIndices16u;
	std::vector<uint8_t>                           primitiveIndices8u;

	std::unordered_map<std::string, GltfPrimMesh> cachePrimMesh;

	void checkRequiredExtensions(const tinygltf::Model& tmodel);
	void findUsedMeshes(const tinygltf::Model& tmodel, std::set<uint32_t>& usedMeshes, int nodeIdx);
	
	void createNormals(GltfPrimMesh& resultMesh);
	void createTexcoords(GltfPrimMesh& resultMesh);
	void createTangents(GltfPrimMesh& resultMesh);
	void createColors(GltfPrimMesh& resultMesh);
};

glm::mat4 getLocalMatrix(const tinygltf::Node& tnode);

template<class T>
void forEachSparseValue(const tinygltf::Model&                            tmodel,
	                    const tinygltf::Accessor&                         accessor,
	                    size_t                                            accessorFirstElement,
	                    size_t                                            numElementsToProcess,
	                    std::function<void(size_t index, const T* value)> fn)
{
	if(!accessor.sparse.isSparse)
	{
		return; 
	}

	const auto& idxs = accessor.sparse.indices;
	if(!(idxs.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE
		|| idxs.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT
		|| idxs.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT))
	{
		assert(!"Unsupported sparse accessor index type.");
		return;
	}

	const tinygltf::BufferView& idxBufferView = tmodel.bufferViews[idxs.bufferView];
	const unsigned char*        idxBuffer     = &tmodel.buffers[idxBufferView.buffer].data[idxBufferView.byteOffset];
	const size_t                idxBufferByteStride = 
		idxBufferView.byteStride ? idxBufferView.byteStride : tinygltf::GetComponentSizeInBytes(idxs.componentType);
	if(idxBufferByteStride == size_t(-1))
		return;   // Invalid

	const auto&                 vals           = accessor.sparse.values;
	const tinygltf::BufferView& valBufferView  = tmodel.bufferViews[vals.bufferView];
	const unsigned char*        valBuffer      = &tmodel.buffers[valBufferView.buffer].data[valBufferView.byteOffset];
	const size_t                valBufferByteStride = accessor.ByteStride(valBufferView);
	if(valBufferByteStride == size_t(-1))
		return;

	for(size_t pairIdx = 0; pairIdx < accessor.sparse.count; pairIdx++)
	{
		// Read the index from the index buffer, converting its type
		size_t               index = 0;
		const unsigned char* pIdx  = idxBuffer + idxBufferByteStride * pairIdx;
		switch(idxs.componentType)
		{
		case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
			index = *reinterpret_cast<const uint8_t*>(pIdx);
			break;
		case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
			index = *reinterpret_cast<const uint16_t*>(pIdx);
			break;
		case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
			index = *reinterpret_cast<const uint16_t*>(pIdx);
			break;
		}

		// if it's not in range, skip it
		if(index < accessorFirstElement || (index - accessorFirstElement) >= numElementsToProcess)
		{
			continue;
		}

		fn(index, reinterpret_cast<const T*>(valBuffer + valBufferByteStride * pairIdx));
	}
}


template<class T>
void copyAccessorData(T*                        outData,
	                  size_t                    outDataSizeInElements,
	                  size_t                    outFirstElement,
	                  const tinygltf::Model&    tmodel,
	                  const tinygltf::Accessor& accessor,
	                  size_t                    accessorFirstElement,
	                  size_t                    numElementsToCopy)
{
	if(outFirstElement >= outDataSizeInElements)
	{
		assert(!"Invalid outFirstElement!");
		return;
	}
	
	if(accessorFirstElement >= accessor.count)
	{
		assert(!"Invalid accessorFirstElement!");
		return;
	}
	
	const tinygltf::BufferView& bufferView = tmodel.bufferViews[accessor.bufferView];
	const unsigned char* buffer = &tmodel.buffers[bufferView.buffer].data[accessor.byteOffset + bufferView.byteOffset];

	const size_t maxSafeCopySize = std::min(accessor.count - accessorFirstElement, outDataSizeInElements - outFirstElement);
	numElementsToCopy            = std::min(numElementsToCopy, maxSafeCopySize);

	if(bufferView.byteStride == 0)
	{
		memcpy(outData + outFirstElement, reinterpret_cast<const T*>(buffer) + accessorFirstElement, numElementsToCopy * sizeof(T));
	}
	else 
	{
		// Must copy one-by-one
		for(size_t i = 0; i < numElementsToCopy; i++)
		{
			outData[outFirstElement + i] = *reinterpret_cast<const T*>(buffer + bufferView.byteStride * i);
		}
	}

	// Handle sparse accessors by overwriting already copied elements
	forEachSparseValue<T>(tmodel, accessor, accessorFirstElement, numElementsToCopy,
		                 [&outData](size_t index, const T* value){ outData[index] = *value;});
}

template<class T>
void copyAccessorData(std::vector<T>&           outData,
	                  size_t                    outFirstElement,
	                  const tinygltf::Model&    tmodel,
	                  const tinygltf::Accessor& accessor,
	                  size_t                    accessorFirstElement,
	                  size_t                    numElementsToCopy)
{
	copyAccessorData<T>(outData.data(), outData.size(), outFirstElement, tmodel, accessor, accessorFirstElement, numElementsToCopy);
}

template<typename T>
static bool getAccessorData(const tinygltf::Model& tmodel, const tinygltf::Accessor& accessor, std::vector<T>& attribVec)
{
	// Retrieving the data of the accessor 
	const auto nbElems = accessor.count;

	const size_t oldNumElements = attribVec.size();
	attribVec.resize(oldNumElements + nbElems);

	// Copying the attributes
	if(accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT)
	{
		copyAccessorData<T>(attribVec, oldNumElements, tmodel, accessor, 0, accessor.count);
	}
	else
	{
		// The component is smaller than float and need to be converted
		const auto&          bufView    = tmodel.bufferViews[accessor.bufferView];
		const auto&          buffer     = tmodel.buffers[bufView.buffer];
		const unsigned char* bufferByte = &buffer.data[accessor.byteOffset + bufView.byteOffset];

		// 2, 3, 4 for VEC2, VEC3, VEC4
		const int nbComponents = tinygltf::GetNumComponentsInType(accessor.type);
		if(nbComponents == -1)
			return false; // Invalid

		// Stride per element 
		const size_t byteStride = accessor.ByteStride(bufView);
		if(byteStride == size_t(-1))
			return false; // Invalid

		if(!(accessor.componentType == TINYGLTF_COMPONENT_TYPE_BYTE || accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE
			|| accessor.componentType == TINYGLTF_COMPONENT_TYPE_SHORT || accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT))
		{
			assert(!"Unhandled tinygltf component type!");
			return false;
		}

		const auto& copyElementFn = [&](size_t elementIdx, const unsigned char* pElement){
			T vecValue;

			 for(int c = 0; c < nbComponents; c++)
			  {
				switch(accessor.componentType)
				{
				  case TINYGLTF_COMPONENT_TYPE_BYTE:
					vecValue[c] = float(*(reinterpret_cast<const char*>(pElement) + c));
					if(accessor.normalized)
					{
					  vecValue[c] = std::max(vecValue[c] / 127.f, -1.f);
					}
					break;
				  case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
					vecValue[c] = float(*(reinterpret_cast<const unsigned char*>(pElement) + c));
					if(accessor.normalized)
					{
					  vecValue[c] = vecValue[c] / 255.f;
					}
					break;
				  case TINYGLTF_COMPONENT_TYPE_SHORT:
					vecValue[c] = float(*(reinterpret_cast<const short*>(pElement) + c));
					if(accessor.normalized)
					{
					  vecValue[c] = std::max(vecValue[c] / 32767.f, -1.f);
					}
					break;
				  case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
					vecValue[c] = float(*(reinterpret_cast<const unsigned short*>(pElement) + c));
					if(accessor.normalized)
					{
					  vecValue[c] = vecValue[c] / 65535.f;
					}
					break;
				}
			  }


			attribVec[oldNumElements + elementIdx] = vecValue;
		};

		for(size_t i = 0; i < nbElems; i++)
		{
			copyElementFn(i, bufferByte + byteStride * i);
		}

		forEachSparseValue<unsigned char>(tmodel, accessor, 0, nbElems, copyElementFn);
	}

	return true;
}

template<typename T>
static bool getAttribute(const tinygltf::Model& tmodel, const tinygltf::Primitive& primitive,
	std::vector<T>& attribVec, const std::string& attribName)
{
	const auto& it = primitive.attributes.find(attribName);
	if(it == primitive.attributes.end())
		return false;
	const auto& accessor = tmodel.accessors[it->second];
	return getAccessorData(tmodel, accessor, attribVec);
}