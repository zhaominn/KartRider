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

#include <gl/glm/glm/gtc/quaternion.hpp> // 쿼터니언 관련
#include <gl/glm/glm/gtx/quaternion.hpp> // SLERP(Spherical Linear Interpolation)

#define MAX_SPEED 0.3
#define ACCELERATION 0.002f
#define DECELERATION 0.001f

class Map1_Mode : public Mode {
public:
	glm::quat cameraRotationQuat = glm::quat(glm::vec3(0.0f, 0.0f, 0.0f)); // 현재 카메라 행렬을 쿼터니언으로 저장
	float reducedRotationInfluence = 0.0f; // 보간할 퍼센트

	GLfloat kart_speed = 0.0f;
	enum Direction { NONE = -1, UP, DOWN, LEFT, RIGHT };
	Direction prev_dir = NONE;

	bool up = false;
	bool down = false;
	bool left = false;
	bool right = false;

	glm::vec3 cameraPos = glm::vec3(0.0, 0.0, 5.0);
	glm::vec3 cameraDirection = glm::vec3(0.0, 0.0, -1.0);
	glm::vec3 cameraUp = glm::vec3(0.0, 1.0, 0.0);
	glm::mat4 projection = glm::mat4(1.0f);
	glm::mat4 view = glm::mat4(1.0f);

	// 카메라 회전 각도
	float yaw = -90.0f; // 수평 회전 (기본: -Z축)
	float pitch = 0.0f; // 수직 회전 (기본: 수평)

	Map1_Mode() {
		Mode::currentInstance = this;  // Map1_Mode 인스턴스를 currentInstance에 할당
	}

	void init() override {
		for (const auto& kart : karts) { // 카트 위치 초기화
			kart->translateMatrix = glm::mat4(1.0f);
			kart->translateMatrix = glm::translate(kart->translateMatrix, glm::vec3(0.0, 2.6, 238.0));

			// 비정적 timer 함수 호출
			glutTimerFunc(0, Map1_Mode::timerHelper, 0);
		}

		kart_speed = 0.0f;
		cameraPos = glm::vec3(0.0, 5.6, 253.0);
		updateCameraDirection();
	}

	void updateCameraDirection() {
		glm::mat3 rotationMatrix = glm::mat3(karts[0]->translateMatrix);

		// 기본 카메라 방향 벡터 계산
		glm::vec3 direction;
		direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		direction.y = sin(glm::radians(pitch));
		direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

		// 회전 행렬을 적용한 방향 벡터 계산
		glm::vec3 rotatedDirection = rotationMatrix * direction;

		//-----------------------
		// 카트를 바라보도록 설정
		glm::vec3 carPosition = glm::vec3(karts[0]->translateMatrix[3]);

		// 자동차를 바라보는 방향 계산 (자동차 위치 - 카메라 위치)
		cameraDirection = glm::normalize(rotatedDirection) + carPosition;
	}

	void setCamera() {
		/*
		glm::vec3 carPosition = glm::vec3(karts[0]->translateMatrix[3]);

		// 오프셋 벡터에 회전 행렬을 적용
		glm::vec3 rotatedOffset = glm::mat3(karts[0]->translateMatrix) * glm::vec3(0.0f, 3.0f, 15.0f);

		// 카메라 위치는 카트 위치에 회전된 오프셋을 더한 값
		cameraPos = carPosition + rotatedOffset;

		updateCameraDirection();*/
		//-------------------------------------
		glm::vec3 carPosition = glm::vec3(karts[0]->translateMatrix[3]);

		// 자동차의 회전 행렬 추출 (3x3 행렬)
		glm::mat3 carRotationMatrix = glm::mat3(karts[0]->translateMatrix);

		// 자동차 회전 행렬을 쿼터니언으로 변환
		glm::quat carRotationQuat = glm::quat_cast(carRotationMatrix);

		// 카메라 회전

		// 자동차 회전과 기본 회전을 보간
		glm::quat interpolatedRotation = glm::slerp(cameraRotationQuat, carRotationQuat, reducedRotationInfluence);

		cameraRotationQuat = interpolatedRotation;

		// 보간된 회전을 행렬로 변환
		glm::mat3 adjustedRotationMatrix = glm::mat3_cast(interpolatedRotation);

		// 카메라 기본 오프셋 정의
		glm::vec3 baseOffset = glm::vec3(0.0f, 3.0f, 15.0f);

		// 조정된 회전을 오프셋에 적용
		glm::vec3 rotatedOffset = adjustedRotationMatrix * baseOffset;

		// 최종 카메라 위치 계산 (자동차 위치 + 회전된 오프셋)
		cameraPos = carPosition + rotatedOffset;

		// 카메라가 자동차를 바라보도록 방향 업데이트
		updateCameraDirection();
	}

	void timer() {
		UpdateRigidBodyTransform(karts[0]);
		if (up || down || left || right) {
			if (up) prev_dir = UP;
			if (down) prev_dir = DOWN;
			if (left) prev_dir = LEFT;
			if (right) prev_dir = RIGHT;

			reducedRotationInfluence = 0.1f;

			if ((up || down) && kart_speed <= MAX_SPEED) { // 속도 제한
				kart_speed += ACCELERATION;
			}
		}
		else {
			if (kart_speed >= 0.0f) // 속도 제한
				kart_speed -= DECELERATION;
			else if (kart_speed == 0.0f)
				prev_dir = NONE;

			if (reducedRotationInfluence <= 1.0f)
				reducedRotationInfluence += 0.01f;
		}
		if (prev_dir == UP) {
			for (const auto& kart : karts) {
				kart->translateMatrix = glm::translate(kart->translateMatrix, glm::vec3(0.0, 0.0, -kart_speed));
			}
		}
		if (prev_dir == DOWN) {
			for (const auto& kart : karts) {
				kart->translateMatrix = glm::translate(kart->translateMatrix, glm::vec3(0.0, 0.0, kart_speed));
			}
		}
		if (prev_dir == LEFT) {
			for (const auto& kart : karts) {
				kart->translateMatrix = glm::translate(kart->translateMatrix, glm::vec3(0.0, 0.0, -1.5));
				kart->translateMatrix = glm::rotate(kart->translateMatrix, glm::radians(kart_speed * 10), glm::vec3(0.0f, 1.0f, 0.0f));
				kart->translateMatrix = glm::translate(kart->translateMatrix, glm::vec3(0.0, 0.0, 1.5));
			}
		}
		if (prev_dir == RIGHT) {
			for (const auto& kart : karts) {
				kart->translateMatrix = glm::translate(kart->translateMatrix, glm::vec3(0.0, 0.0, -1.5));
				kart->translateMatrix = glm::rotate(kart->translateMatrix, glm::radians(-kart_speed * 10), glm::vec3(0.0f, 1.0f, 0.0f));
				kart->translateMatrix = glm::translate(kart->translateMatrix, glm::vec3(0.0, 0.0, 1.5));
			}
		}
		setCamera();
	}

	void moveCamera(unsigned char key, int x, int y) {
		const float cameraSpeed = 0.1f; // 카메라 이동 속도
		float angleInRadians = glm::radians(5.0f); // 5도 회전

		// 카메라 전방 벡터
		glm::vec3 forward = glm::normalize(cameraDirection - cameraPos);
		// 카메라 오른쪽 벡터
		glm::vec3 right = glm::normalize(glm::cross(forward, cameraUp));

		switch (key) {
		case 'w': // 전진
			cameraPos += cameraSpeed * forward;
			cameraDirection += cameraSpeed * forward;
			break;
		case 's': // 후진
			cameraPos -= cameraSpeed * forward;
			cameraDirection -= cameraSpeed * forward;
			break;
		case 'a': // 왼쪽 이동
			cameraPos -= cameraSpeed * right;
			cameraDirection -= cameraSpeed * right;
			break;
		case 'd': // 오른쪽 이동
			cameraPos += cameraSpeed * right;
			cameraDirection += cameraSpeed * right;
			break;
		case 'i': // 위로 회전 (X축 회전)
		{
			pitch += glm::degrees(angleInRadians);
			if (pitch > 89.0f) pitch = 89.0f; // 상단 제한
			updateCameraDirection();
			break;
		}
		case 'k': // 아래로 회전 (X축 반대 방향)
		{
			pitch -= glm::degrees(angleInRadians);
			if (pitch < -89.0f) pitch = -89.0f; // 하단 제한
			updateCameraDirection();
			break;
		}
		case 'j': // 왼쪽 회전 (Y축 회전)
		{
			yaw -= glm::degrees(angleInRadians);
			updateCameraDirection();
			break;
		}
		case 'l': // 오른쪽 회전 (Y축 반대 방향)
		{
			yaw += glm::degrees(angleInRadians);
			updateCameraDirection();
			break;
		}
		default:
			break;
		}
	}

	void keyboard(unsigned char key, int x, int y) override {
		moveCamera(key, x, y);
	}

	void specialKey(int key, int x, int y) override {
		switch (key) {
		case GLUT_KEY_UP:
			up = true;
			break;
		case GLUT_KEY_DOWN:
			down = true;
			break;
		case GLUT_KEY_LEFT:
			left = true;
			break;
		case GLUT_KEY_RIGHT:
			right = true;
			break;
		}
	}

	void specialKeyUp(int key, int x, int y) override {
		switch (key) {
		case GLUT_KEY_UP:
			up = false;
			break;
		case GLUT_KEY_DOWN:
			down = false;
			break;
		case GLUT_KEY_LEFT:
			left = false;
			break;
		case GLUT_KEY_RIGHT:
			right = false;
			break;
		}
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
		for (const auto& road : road1) { // 실제 모델 draw
			road->draw(shaderProgramID, isKeyPressed_s);
		}


		glDisable(GL_DEPTH_TEST);
	}

	void draw_bb() override {
		for (const auto& model : karts) { // 모델 bb draw
			model->draw_rigidBody(shaderProgramID);
		}
		for (const auto& road : road1) { // 모델 bb draw
			road->draw_rigidBody(shaderProgramID);
		}
	}

	void finish() override {

	}
private:

	static void timerHelper(int value) {
		if (Map1_Mode* instance = dynamic_cast<Map1_Mode*>(Mode::currentInstance)) {
			instance->timer(); // 인스턴스의 timer 호출
		}
		glutPostRedisplay();
		glutTimerFunc(16, timerHelper, value); // 타이머 반복 호출
	}

};