#include "light.h"



using namespace mini;

double random(double min, double max)
{
	return (double)rand() / RAND_MAX * (max - min) + min;
}

Light mini::getRandomLight()
{
    srand(time(0));

    float randomRange = 10;
    
    vec3 randomPos = vec3(0.0f);
    randomPos.x = random(-randomRange, randomRange);
    randomPos.y = random(0, randomRange);
    randomPos.z = random(-randomRange, randomRange);


    vec3 randomColor = vec3(0.0f);

    randomColor.x = random(0.0f, 1.0f);
    randomColor.y = random(0.0f, 1.0f);
    randomColor.z = random(0.0f, 1.0f);

    float randomIntensity = random(0.0f, 10.0f);

    Light l{ LIGHT_TYPE_POINT, randomPos, vec3(0.0f), randomColor, randomIntensity, false };

    return l;

}
