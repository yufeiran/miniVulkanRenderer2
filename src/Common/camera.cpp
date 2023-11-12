#include"camera.h"

namespace mini
{




Camera::Camera(glm::vec3 cameraPos, glm::vec3 cameraUp, double yaw, double pitch, double roll, float fov, double step, double changeDirSensitivity):
	cameraPos(cameraPos),cameraUp(cameraUp),yaw(yaw),pitch(pitch),roll(roll),step(step), changeDirSensitivity(changeDirSensitivity),fov(fov)
{
	worldUp = cameraUp;
	updateCameraVec();
	calViewMat();
}

void Camera::calViewMat()
{
	cameraFront = glm::normalize(cameraFront);
	cameraUp = glm::normalize(cameraUp);
	viewMat = glm::lookAt(cameraPos, cameraPos+ cameraFront, cameraUp);

}

void Camera::updateCameraVec()
{

	glm::vec3 direction;
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(direction);

	cameraRight = glm::normalize(glm::cross(cameraFront, worldUp));
	cameraUp = glm::normalize(glm::cross(cameraRight, cameraFront));
	calViewMat();
}

void Camera::setPos(glm::vec3& pos)
{
	cameraPos = pos;
	calViewMat();
}

void Camera::setViewDir(double yaw, double pitch)
{
	this->yaw   = yaw;
	this->pitch = pitch;
	updateCameraVec();
	calViewMat();
}

void Camera::move(CAMERA_MOVE_DIR dir, float deltaTime)
{



	glm::vec3 stepFront = step * deltaTime * cameraFront;
	glm::vec3 stepUp = step * deltaTime * worldUp;
	glm::vec3 stepSide = step * deltaTime * cameraRight;
	
	if (dir == UP_DIR)
	{
		cameraPos += stepFront;
	}
	else if (dir == DOWN_DIR)
	{
		cameraPos -= stepFront;
	}
	else if (dir == LEFT_DIR)
	{
		cameraPos -= stepSide;
	}
	else if (dir == RIGHT_DIR)
	{
		cameraPos += stepSide;
	}
	else if (dir == FRONT_DIR)
	{
		cameraPos += stepUp;
	}
	else if (dir == END_DIR)
	{
		cameraPos -= stepUp;
	}
	calViewMat();
}

void Camera::changeDir(double xoffset, double yoffset)
{

	//if (abs(xoffset) > 10 || abs(yoffset) > 10)return;
	//Log("xoffset " + toString(xoffset) + " yoffset " + toString(yoffset)+", yaw "+toString(yaw)+ " pitch "+toString(pitch));
	xoffset *= changeDirSensitivity;
	yoffset *= changeDirSensitivity;

	yaw += xoffset;
	pitch += yoffset;

	//Log("Yaw " + toString(yaw) + " Pitch " + toString(pitch));

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	updateCameraVec();
	calViewMat();
}

glm::mat4 Camera::getViewMat() const
{
	return viewMat;
}
}