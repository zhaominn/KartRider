#pragma once
#include <iostream>
#include "Mode.h"
#include "RoadModel.h"
#include "KartModel.h"
#include "shaderMaker.h"
#include "LoadProgress.h"
#include "root.h"
#include "KeyBoard.h"
#include "Light.h"


class PlayMode : public Mode {
public:

	glm::vec3 cameraPos = glm::vec3(0.0, 0.0, 5.0);
	glm::vec3 cameraDirection = glm::vec3(0.0, 0.0, -1.0);
	glm::vec3 cameraUp = glm::vec3(0.0, 1.0, 0.0);
	glm::mat4 projection = glm::mat4(1.0f);
	glm::mat4 view = glm::mat4(1.0f);

	int map_num = 1;

	// 카메라 회전 각도
	float yaw = -90.0f; // 수평 회전 (기본: -Z축)
	float pitch = 0.0f; // 수직 회전 (기본: 수평)

	PlayMode(){}

	void init() override {

	}

	void updateCameraDirection() {
		// Yaw와 Pitch로 방향 벡터 계산
		glm::vec3 direction;
		direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		direction.y = sin(glm::radians(pitch));
		direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		cameraDirection = glm::normalize(direction); // 정규화
	}

	void moveCamera(unsigned char key, int x, int y) {
		const float rotationSpeed = 2.5f;
		const float cameraSpeed = 0.1f; // 카메라 이동 속도
		glm::vec3 forward = cameraDirection; // 전방 벡터
		glm::vec3 right = glm::normalize(glm::cross(forward, cameraUp)); // 오른쪽 벡터

		switch (key) {
		case 'w': // 전진
			cameraPos += cameraSpeed * forward;
			break;
		case 's': // 후진
			cameraPos -= cameraSpeed * forward;
			break;
		case 'a': // 왼쪽 이동
			cameraPos -= cameraSpeed * right;
			break;
		case 'd': // 오른쪽 이동
			cameraPos += cameraSpeed * right;
			break;
		case 'i':
			pitch += rotationSpeed;
			if (pitch > 89.0f) pitch = 89.0f;
			break;
		case 'j':
			yaw -= rotationSpeed;
			break;
		case 'k':
			pitch -= rotationSpeed;
			if (pitch < -89.0f) pitch = -89.0f;
			break;
		case 'l':
			yaw += rotationSpeed;
			break;
		default:
			break;
		}
		updateCameraDirection();
	}

	void keyboard(unsigned char key, int x, int y) override {
		moveCamera(key, x, y);
	}

	void keySpecial(int key, int x, int y) override{
		
	}

	void draw_model() override {

		glClearColor(1.0, 1.0, 1.0, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(shaderProgramID);


		GLenum error = glGetError();
		if (error != GL_NO_ERROR) {
			std::cout << "Error in glUseProgram: " << error << std::endl;
		}

		this->view = glm::mat4(1.0f);
		this->view = glm::lookAt(
			this->cameraPos,
			this->cameraDirection,
			this->cameraUp
		);
		unsigned int viewLocation = glGetUniformLocation(shaderProgramID, "viewTransform");
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);

		this->projection = glm::mat4(1.0f);
		this->projection = glm::perspective(glm::radians(45.0f), static_cast<float>(width) / static_cast<float>(height), 0.1f, 300.0f);
		unsigned int projectionLocation = glGetUniformLocation(shaderProgramID, "projectionTransform");
		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);

		GLint lightPosLoc = glGetUniformLocation(shaderProgramID, "lightPos");
		GLint lightColorLoc = glGetUniformLocation(shaderProgramID, "lightColor");
		glUniform3fv(lightPosLoc, 1, glm::value_ptr(lightPos));
		glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

		glEnable(GL_DEPTH_TEST);

		for (const auto& kart : karts) { // 실제 모델 draw
			kart->draw(shaderProgramID, isKeyPressed_s);
		}
		for (const auto& road : roads) { // 실제 모델 draw
			road->draw(shaderProgramID, isKeyPressed_s);
		}


		glDisable(GL_DEPTH_TEST);
	}

	void draw_bb() override {
		for (const auto& model : karts) { // 모델 bb draw
			model->draw_rigidBody(shaderProgramID);
		}
		for (const auto& road : roads) { // 모델 bb draw
			road->draw_rigidBody(shaderProgramID);
		}
	}

	void finish() override {

	}

};
