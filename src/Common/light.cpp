#include "light.h"
#include "ResourceManagement/ResourceManager.h"



using namespace mini;

double random(double min, double max)
{
	return (double)rand() / (double)RAND_MAX * (max - min) + min;
}

void mini::addRandomLight(std::vector<Light>& lights, ResourceManager& resManager)
{

    float randomRange = 10;
    
    vec3 randomPos = vec3(0.0f);
    randomPos.x = random(-randomRange, randomRange);
    randomPos.y = random(0, randomRange);
    randomPos.z = random(-randomRange, randomRange);


    vec3 randomColor = vec3(0.0f);

    while(randomColor.x + randomColor.y + randomColor.z < 1.5f)
	{
		randomColor.x = random(0.0f, 1.0f);
		randomColor.y = random(0.0f, 1.0f);
		randomColor.z = random(0.0f, 1.0f);
	}


    vec3 randomDirection = vec3(0.0f);

    randomDirection.x = random(-1.0f, 1.0f);
    randomDirection.y = random(-1.0f, 1.0f);
    randomDirection.z = random(-1.0f, 1.0f);

    float randomIntensity = random(0.0f, 3.0f);

    addLight(lights, resManager,LIGHT_TYPE_POINT, randomPos,randomDirection, randomColor, randomIntensity);



}

void mini::addLight(std::vector<Light>& lights, ResourceManager& resManager,LightType lightType, glm::vec3 position, glm::vec3 direction, glm::vec3 color, float intensity, bool isShadowCaster)
{
    Light l{ lightType, position, direction, color, intensity, isShadowCaster };
    int lightId = lights.size();
    int instanceId = resManager.addLightCubeInstance(l, lightId);
    l.setInstanceId(instanceId);
    
    
    lights.push_back(l);
}


void mini::delLight(std::vector<Light>& lights, ResourceManager& resManager, int lightId)
{
    if (lightId < 0 || lightId >= lights.size())
	{
    		return;
    }   
    auto& light = lights[lightId];
    
    int lightInstanceId = light.getInstanceId();
    lights.erase(lights.begin() + lightId);


    auto& instances = resManager.getInstances();
    instances.erase(instances.begin() + lightInstanceId);

    // update instanceId
    for(int i = 0; i < lights.size(); i++)
    {
        int instanceId = lights[i].getInstanceId();
        if(instanceId > lightInstanceId)
		{
        	lights[i].setInstanceId(instanceId - 1);
        }
        auto& nowLightINstance = instances[lights[i].getInstanceId()];
        nowLightINstance.lightIndex = i;

    }

    


    
}