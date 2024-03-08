#pragma once
#include"Common/common.h"
#include "glm/matrix.hpp"
#include "../shaders/deviceDataStruct.h"


namespace mini
{

	const int SHADOW_WIDTH = 4096;
	const int SHADOW_HEIGHT = 4096;


	enum LightType
	{
		LIGHT_TYPE_POINT = 0,
		LIGHT_TYPE_DIRECTIONAL,
		LIGHT_TYPE_SPOT
	};

	class Light
	{
	public:
		Light(LightType type, glm::vec3 position, glm::vec3 direction, glm::vec3 color, float intensity, bool isShadowCaster = false)
			: type(type), position(position), direction(direction), color(color), intensity(intensity), isShadowCaster(isShadowCaster)
		{

		}

		inline void setShadowCaster(bool isShadowCaster)
		{
			this->isShadowCaster = isShadowCaster;
		}

		inline void setType(LightType type)
		{
			this->type = type;
		}

		inline void setPosition(glm::vec3 position)
		{
			this->position = position;
		}

		inline void setDirection(glm::vec3 direction)
		{
			this->direction = direction;
		}

		inline void setColor(glm::vec3 color)
		{
			this->color = color;
		}

		inline void setIntensity(float intensity)
		{
			this->intensity = intensity;
		}


		inline LightType getType() const
		{
			return type;
		}

		inline glm::vec3 getPosition() const
		{
			return position;
		}

		inline glm::vec3 getDirection() const
		{
			return direction;
		}

		inline glm::vec3 getColor() const
		{
			return color;
		}

		inline float getIntensity() const
		{
			return intensity;
		}

		inline bool getShadowCaster() const
		{
			return isShadowCaster;
		}

		LightType type;
		glm::vec3 position;
		glm::vec3 direction;
		glm::vec3 color;
		float intensity;
		bool isShadowCaster;
	private:



	};

	 LightUniforms inline createLightUniforms(std::vector<Light> lights)
	{
		LightUniforms lightUniforms = {};
		lightUniforms.lightCount = lights.size();

		lightUniforms.pointShadowIndex = -1;
		lightUniforms.dirShadowIndex = -1;
		lightUniforms.spotShadowIndex = -1;

		for (int i = 0; i < lights.size(); i++)
		{

			auto& light = lights[i];
			lightUniforms.lights[i].position = glm::vec4(lights[i].getPosition(),1.0);
			lightUniforms.lights[i].color = glm::vec4( lights[i].getColor(),1.0);
			lightUniforms.lights[i].intensity = lights[i].getIntensity();
			lightUniforms.lights[i].type = lights[i].getType();

			if(lights[i].getShadowCaster() == true)
			{

				if (lights[i].getType() == LightType::LIGHT_TYPE_DIRECTIONAL)
				{
					lightUniforms.dirShadowIndex = i;
					
					float near = 0.1f, far = 60.0f;

					lightUniforms.lights[i].nearPlane = near;
					lightUniforms.lights[i].farPlane = far;

					glm::mat4 lightProjection = glm::ortho(-30.0f, 30.0f, -30.0f, 30.0f, near, far);
					lightProjection[1][1] *= -1;

					glm::mat4 lightView = glm::lookAt( light.getPosition(), 
														glm::vec3( 0.0f, 0.0f,  0.0f), 
														glm::vec3( 0.0f, 1.0f,  0.0f));  

					glm::mat4 lightSpaceMatrix = lightProjection * lightView;


					lightUniforms.lights[i].dirLightSpaceMatrix = lightSpaceMatrix;
				}
				else if (lights[i].getType() == LightType::LIGHT_TYPE_POINT)
				{
					lightUniforms.pointShadowIndex = i;

					float aspect = (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT;
					float near  = 1.0f;
					float far   = 25.0f;
					lightUniforms.lights[i].nearPlane = near;
					lightUniforms.lights[i].farPlane = far;
					glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), aspect, near, far);

					auto& lightMartix = lightUniforms.lights[i].pointLightSpaceMatrix;

					lightMartix[0] = shadowProj * glm::lookAt(light.getPosition(), light.getPosition() + glm::vec3( 1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
					lightMartix[1] = shadowProj * glm::lookAt(light.getPosition(), light.getPosition() + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
					lightMartix[2] = shadowProj * glm::lookAt(light.getPosition(), light.getPosition() + glm::vec3( 0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
					lightMartix[3] = shadowProj * glm::lookAt(light.getPosition(), light.getPosition() + glm::vec3( 0.0f,-1.0f, 0.0f), glm::vec3(0.0f, 0.0f,-1.0f));
					lightMartix[4] = shadowProj * glm::lookAt(light.getPosition(), light.getPosition() + glm::vec3( 0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
					lightMartix[5] = shadowProj * glm::lookAt(light.getPosition(), light.getPosition() + glm::vec3( 0.0f, 0.0f,-1.0f), glm::vec3(0.0f, -1.0f, 0.0f));


				}
			}


		}
		return lightUniforms;
	}

}