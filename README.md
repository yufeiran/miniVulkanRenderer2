# miniVulkanRenderer2
 A mini renderer using Vulkan
 ## 1. Features
 * PBR
 * PathTrace
 * SubPass
 * SSAO
 * Physical Based Bloom
 ## 2. Screenshot
* Raster
![mulitLight](assets/screenshot/multLights.png)
![PBBloom](assets/screenshot/physical_based_bloom.png)
![raster](assets/screenshot/shadowMappingPoint_raster.png)
* RayTracing
![raytracing](assets/screenshot/shadowMappingPoint_raytracing.png)


### 2.1 rasterization

#### Overall Process 
![rasterProcess](assets/screenshot/raster_process.png)
#### detail features

* ShadowMapPass
![shadowMap](assets/screenshot/shadowMap.png)

* SkyBoxPass
![skyBOxPass](assets/screenshot/skylightPass.png)

 <details> 
<summary>rasterization features is still on building</summary>

![sponaz](assets/screenshot/raster.png)
 </details> 

 ### 2.2 RayTracing
* Demo
![demo](assets/screenshot/sceneDemo.png)

* Normal Map 
![Noraml Map](assets/screenshot/normalMap.png)
![Emission1](assets/screenshot/emssion1.png)
 <details> 
<summary>more RayTracing sample </summary>


* Emission 
![Emission](assets/screenshot/emssion.png)
![Emission1](assets/screenshot/emssion1.png)
* Alpha Test
![Alpha Test](assets/screenshot/alphaTest.png)
* MetalRoughSpheres
![MetalRough](assets/screenshot/metal_roughness.png)
* Normal Tangent Test
![NormalTangent](assets/screenshot/NormalTangent.png)
![NormalTangent1](assets/screenshot/NormalTangent1.png)
* HDR
![sponaz](assets/screenshot/sponza.png)
![hdrOn](assets/screenshot/hdr.png)

 </details> 

 
## 3. ThirdParty
* imgui
* tinygltf
* tinyobjloader
* volk
* stb_image
