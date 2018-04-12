#pragma once
#include "Init.h"
class Camera {
public:
	Camera(glm::vec3 cameraPos, glm::vec3 cameraFront, glm::vec3 cameraUp);
	void moveFront(float speed);
	void moveBack(float speed);
	void moveLeft(float speed);
	void moveRight(float speed);
	void setFront(glm::vec3 front);
	void setPos(glm::vec3 pos);
	glm::vec3 getPos();
	glm::vec3 getFront();
	glm::vec3 getUp();
private:
	glm::vec3 Pos;
	glm::vec3 Front;
	glm::vec3 Up;
};
Camera::Camera(glm::vec3 cameraPos, glm::vec3 cameraFront, glm::vec3 cameraUp) {
	Pos = cameraPos;
	Front = cameraFront;
	Up = cameraUp;
}
void Camera::moveFront(float speed) {
	Pos += Front * speed;
}
void Camera::moveBack(float speed) {
	Pos -= Front * speed;
}
void Camera::moveLeft(float speed) {
	Pos -= glm::normalize(glm::cross(Front, Up)) * speed;
}
void Camera::moveRight(float speed) {
	Pos += glm::normalize(glm::cross(Front, Up)) * speed;
}
void Camera::setFront(glm::vec3 front) {
	Front = front;
}
void Camera::setPos(glm::vec3 pos) {
	Pos = pos;
}
glm::vec3 Camera::getPos() {
	return Pos;
}
glm::vec3 Camera::getFront() {
	return Front;
}
glm::vec3 Camera::getUp() {
	return Up;
}