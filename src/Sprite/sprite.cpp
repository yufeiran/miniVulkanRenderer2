#include"sprite.h"
#include"ResourceManagement/model.h"

namespace mini
{

Sprite::Sprite(uint32_t id, Model& model, float x , float y , float z, float scale,
	float angleX, float angleY, float angleZ, std::string name)
	:id(id), model(model),x(x),y(y),z(z),scale(scale), angleX(angleX), angleY(angleY), angleZ(angleZ),name(name)
{
	calModelMat();
}

Model& Sprite::getModel() const
{
	return model;
}

void Sprite::calModelMat()
{
	glm::mat4 mat;
	mat = glm::translate(glm::mat4(1.0f), { x,y,z });

	mat = glm::rotate(mat, glm::radians( angleX), glm::vec3(1.0f,0.0f, 0.0f));
	mat = glm::rotate(mat, glm::radians(angleY), glm::vec3(0.0f, 1.0f, 0.0f));
	mat = glm::rotate(mat, glm::radians(angleZ), glm::vec3(0.0f, 0.0f, 1.0f));

	mat = glm::scale(mat, { scale,scale,scale });
	
	modelMat = mat;
}

glm::mat4 Sprite::getModelMat() const
{
	return modelMat;
}

}