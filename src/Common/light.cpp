#include "light.h"
#include "ResourceManagement/ResourceManager.h"



using namespace mini;

double random(double min, double max)
{
	return (double)rand() / RAND_MAX * (max - min) + min;
}

void mini::addRandomLight(std::vector<Light>& lights, ResourceManager& resManager)
{

    float randomRange = 10;
    
    vec3 randomPos = vec3(0.0f);
    randomPos.x = random(-randomRange, randomRange);
    randomPos.y = random(0, randomRange);
    randomPos.z = random(-randomRange, randomRange);


    vec3 randomColor = vec3(0.0f);

    randomColor.x = random(0.0f, 1.0f);
    randomColor.y = random(0.0f, 1.0f);
    randomColor.z = random(0.0f, 1.0f);

    vec3 randomDirection = vec3(0.0f);

    randomDirection.x = random(-1.0f, 1.0f);
    randomDirection.y = random(-1.0f, 1.0f);
    randomDirection.z = random(-1.0f, 1.0f);

    float randomIntensity = random(0.0f, 3.0f);

    addLight(lights, resManager,LIGHT_TYPE_POINT, randomPos, randomColor,randomDirection, randomIntensity);



}

void mini::addLight(std::vector<Light>& lights, ResourceManager& resManager,LightType lightType, glm::vec3 position, glm::vec3 direction, glm::vec3 color, float intensity, bool isShadowCaster)
{
    Light l{ lightType, position, direction, color, intensity, isShadowCaster };
    int lightId = lights.size();
    int instanceId = resManager.addLightCubeInstance(l, lightId);
    l.setInstanceId(instanceId);
    
    
    lights.push_back(l);
}
