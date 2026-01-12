# miniVulkanRenderer2

<div align="center">
<img src="assets/logo.png">
<p>
 A mini renderer using Vulkan
</div>

 ## 0. TODO
 - [x] Linux Support
 - [ ] Compute Shader(Bloom / Animation)
 - [ ] OpenXR Support
 - [ ] MacOS Support
 - [ ] Rendering performance optimization
 - [ ] Configurable render quality options
 ## 1 Install && Build

 ### 1.1 Linux
 Ubuntu 24.04:
```
sudo apt update
sudo apt install libvulkan-dev vulkan-tools vulkan-utility-libraries-dev libglfw3-dev vulkan-validationlayers

```

 ## 2. Features
 * PBR
 * PathTrace
 * SubPass
 * SSAO
 * SSR
 * Physical Based Bloom

## 3. Screenshot
![raytracing](assets/screenshot/shadowMappingPoint_raytracing.png)

[Detail](Screenshot.md)
 
## 4. ThirdParty
* imgui 
* tinygltf
* tinyobjloader
* volk
* stb_image
