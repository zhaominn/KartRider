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

#include <thread>
#include <gl/glm/glm/gtc/quaternion.hpp> // 쿼터니언 관련
#include <gl/glm/glm/gtx/quaternion.hpp> // SLERP(Spherical Linear Interpolation)
#include <unordered_map> // keystate

#define ACCELERATION 0.002f
#define DECELERATION 0.001f

class Map1_Mode : public Mode {
public:
	glm::quat cameraRotationQuat = glm::quat(glm::vec3(0.0f, 0.0f, 0.0f)); // 현재 카메라 행렬을 쿼터니언으로 저장
	float reducedRotationInfluence = 0.0f; // 보간할 퍼센트

	GLfloat kart_speed = 0.0f;

	enum Move { NONE_M, UP, DOWN, LEFT, RIGHT };
	float MAX_SPEED = 0.5;

	int start_count;

	bool Pause;

	//키
	std::unordered_map<Move, bool> kart_keyState;

	bool up = false;
	bool down = false;
	bool left = false;
	bool right = false;

	glm::vec3 cameraTargetPos = glm::vec3(0.0, 0.0, 5.0); // 카메라의 목표 위치
	glm::vec3 cameraPos = glm::vec3(0.0, 0.0, 5.0);       // 카메라의 현재 위치
	glm::vec3 cameraDirection = glm::vec3(0.0, 0.0, -1.0);
	glm::vec3 cameraUp = glm::vec3(0.0, 1.0, 0.0);
	glm::mat4 projection = glm::mat4(1.0f);
	glm::mat4 view = glm::mat4(1.0f);

	// 카메라 회전 각도
	float yaw = -90.0f; // 수평 회전 (기본: -Z축)
	float pitch = 0.0f; // 수직 회전 (기본: 수평)
	float TURN_ANGLE = 1.0f; // 회전 각도 (기본 1도) (카트 회전 각도)

	//sound
	bool isBackgroundSound = false;
	std::thread backgroundSoundThread;
	bool isCountNSound = false;
	std::thread countNSoundThread;
	bool isCountGoSound = false;
	std::thread countGoSoundThread;

	Map1_Mode() {
		Mode::currentInstance = this;  // Map1_Mode 인스턴스를 currentInstance에 할당
		isCountNSound = true;
		isCountGoSound = true;
	}

	void init() override {

		UpdateRigidBodyTransforms(road1_barricate);
		UpdateRigidBodyTransforms(karts);

		// Move 상태 초기화
		kart_keyState[UP] = false;
		kart_keyState[DOWN] = false;
		kart_keyState[LEFT] = false;
		kart_keyState[RIGHT] = false;

		start_count = -1;

		Pause = false;

		for (const auto& kart : karts) { // 카트 위치 초기화
			kart->translateMatrix = glm::mat4(1.0f);
			kart->translateMatrix = glm::translate(kart->translateMatrix, glm::vec3(0.0, 2.6, 238.0));
		}
		for (const auto& c : character) { //카트와 같은 행렬 적용
			c->translateMatrix = karts[0]->translateMatrix;
		}
		for (const auto& c : countDown) { //카트와 같은 행렬 적용
			c->translateMatrix = karts[0]->translateMatrix;
			c->translateMatrix = glm::translate(c->translateMatrix, glm::vec3(0.0, 4.0, 0.0));
		}

		kart_speed = 0.0f;
		draw_model();
		glutTimerFunc(0, Map1_Mode::timerHelper, 0);

		cameraPos = glm::vec3(0.0, 6.0, 253.0);
		updateCameraDirection();
	}

	void playCountdown(int count) {
		if (count < 3) { // count_n 사운드 3번 실행
			if (countNSoundThread.joinable()) {
				countNSoundThread.join();
			}

			countNSoundThread = std::thread([this]() {
				play_sound2D("count_n.wav", "./asset/map_1/", false, &isCountNSound);
				});

			countNSoundThread.join();

		}
		else { // count_go 사운드 실행
			if (countGoSoundThread.joinable()) {
				countGoSoundThread.join();
			}
			glutTimerFunc(0, timerHelper, 0);

			countGoSoundThread = std::thread([this]() {
				play_sound2D("count_go.wav", "./asset/map_1/", false, &isCountGoSound);
				});

			countGoSoundThread.join(); // `count_go` 재생이 끝날 때까지 기다림

			isCountGoSound = false;
			isCountNSound = false;
			isBackgroundSound = true;
			backgroundSoundThread = std::thread(&Map1_Mode::backgroundSound, this);
		}
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
		glm::vec3 carPosition = glm::vec3(karts[0]->translateMatrix[3]);

		// 자동차의 회전 행렬 추출 (3x3 행렬)
		glm::mat3 carRotationMatrix = glm::mat3(karts[0]->translateMatrix);

		// 자동차 회전 행렬을 쿼터니언으로 변환
		glm::quat carRotationQuat = glm::quat_cast(carRotationMatrix);

		// 자동차 회전과 기본 회전을 보간
		glm::quat interpolatedRotation = glm::slerp(cameraRotationQuat, carRotationQuat, reducedRotationInfluence);
		cameraRotationQuat = interpolatedRotation;

		// 보간된 회전을 행렬로 변환
		glm::mat3 adjustedRotationMatrix = glm::mat3_cast(interpolatedRotation);

		// 카메라 목표 위치 정의 (속도에 따라 동적으로 조정)
		glm::vec3 baseOffset = glm::vec3(0.0f, 6.0f + (kart_speed * 2.0f), 14.0f + (kart_speed * 10.0f));
		glm::vec3 rotatedOffset = adjustedRotationMatrix * baseOffset;

		// 카메라 목표 위치 계산 (자동차 위치 + 회전된 오프셋)
		cameraTargetPos = carPosition + rotatedOffset;

		// 카메라가 자동차를 바라보도록 방향 업데이트
		cameraDirection = carPosition; // 자동차를 항상 바라봄
	}

	void checkCollisionKart() {
		for (auto& kart : karts) {
			if (kart->name != "car") continue;
			for (const auto& barri : road1_barricate) {
				if (barri->name != "baricate") continue;
				CustomContactResultCallback resultCallback;
				dynamicsWorld->contactPairTest(kart->rigidBody, barri->rigidBody, resultCallback);
				if (resultCallback.hitDetected) {
					cout << "충돌!!!!" << endl;
				}
			}
		}
	}

	void timer() {
		if (start_count < 4) {
			if (start_count >= 0)
				playCountdown(start_count);
			++start_count;
		}
		else {
			UpdateRigidBodyTransforms(karts);

			// 가속/감속 처리
			if (kart_keyState[UP]) {
				if (kart_speed < MAX_SPEED) {
					kart_speed += ACCELERATION;
				}
			}
			else if (kart_keyState[DOWN]) {
				if (kart_speed > -MAX_SPEED / 2.0f) { // 후진 속도는 전진의 절반까지만 허용
					kart_speed -= ACCELERATION;
				}
			}
			else {
				if (kart_speed > 0.0f) {
					kart_speed -= DECELERATION; // 전진 감속
					if (kart_speed < 0.0f) kart_speed = 0.0f;
				}
				else if (kart_speed < 0.0f) {
					kart_speed += DECELERATION; // 후진 감속
					if (kart_speed > 0.0f) kart_speed = 0.0f;
				}
			}

			// 카트 이동 처리
			if (kart_speed > 0.0f) { // 전진
				for (const auto& kart : karts) {
					kart->translateMatrix = glm::translate(kart->translateMatrix, glm::vec3(0.0, 0.0, -kart_speed));
				}
			}
			else if (kart_speed < 0.0f) { // 후진
				for (const auto& kart : karts) {
					kart->translateMatrix = glm::translate(kart->translateMatrix, glm::vec3(0.0, 0.0, -kart_speed));
				}
			}

			// 방향 전환 처리
			if (kart_keyState[LEFT]) {
				if (kart_speed != 0.0f) {
					for (const auto& kart : karts) {
						kart->translateMatrix = glm::translate(kart->translateMatrix, glm::vec3(0.0, 0.0, -1.5));
						kart->translateMatrix = glm::rotate(kart->translateMatrix, glm::radians(TURN_ANGLE), glm::vec3(0.0f, 1.0f, 0.0f));
						kart->translateMatrix = glm::translate(kart->translateMatrix, glm::vec3(0.0, 0.0, 1.5));
					}
				}
			}

			if (kart_keyState[RIGHT]) {
				if (kart_speed != 0.0f) {
					for (const auto& kart : karts) {
						kart->translateMatrix = glm::translate(kart->translateMatrix, glm::vec3(0.0, 0.0, -1.5));
						kart->translateMatrix = glm::rotate(kart->translateMatrix, glm::radians(-TURN_ANGLE), glm::vec3(0.0f, 1.0f, 0.0f));
						kart->translateMatrix = glm::translate(kart->translateMatrix, glm::vec3(0.0, 0.0, 1.5));
					}
				}
			}

			//캐릭터 
			for (const auto& c : character) { //카트와 같은 행렬 적용
				c->translateMatrix = karts[0]->translateMatrix;
			}

			// 카메라 회전 보간율 업데이트 (속도에 따라 카메라 회전이 빨라짐)
			if (kart_speed != 0.0f) {
				reducedRotationInfluence = 0.1f + (std::abs(kart_speed) / MAX_SPEED) * 0.4f; // 속도 비례 보간율
			}
			else {
				reducedRotationInfluence += 0.01f; // 키가 안 눌릴 때 천천히 회복
				if (reducedRotationInfluence > 1.0f) reducedRotationInfluence = 1.0f;
			}

			// 카메라 업데이트
			setCamera();
			// 현재 카메라 위치를 목표 위치로 점진적으로 이동
			float cameraFollowSpeed = 0.1f; // 카메라가 따라가는 속도 (0.0 ~ 1.0 사이의 값)
			cameraPos = glm::mix(cameraPos, cameraTargetPos, cameraFollowSpeed);

			checkCollisionKart();
		}
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

	void mouseClick(int button, int state, int x, int y) override {}

	void keyboard(unsigned char key, int x, int y) override {
		moveCamera(key, x, y);
		if (key == 27) { //esc
			if (Pause) {
				glutTimerFunc(16, timerHelper, 0); // 타이머 호출
			}
			else {
				glm::vec3 zAxis = glm::normalize(cameraPos - glm::vec3(karts[0]->translateMatrix[3]));

				// 오른쪽 벡터 (X축) 계산
				glm::vec3 xAxis = glm::normalize(glm::cross(cameraUp, zAxis));

				// 상단 벡터 (Y축) 계산
				glm::vec3 yAxis = glm::cross(zAxis, xAxis);

				// 3x3 회전 행렬 생성
				glm::mat3 rotationMatrix = glm::mat3(
					xAxis, // X축
					yAxis, // Y축
					zAxis  // Z축
				);

				// 4x4 행렬로 확장
				glm::mat4 modelMatrix = glm::mat4(1.0f); // 단위 행렬로 초기화
				modelMatrix[0] = glm::vec4(rotationMatrix[0], 0.0f); // X축
				modelMatrix[1] = glm::vec4(rotationMatrix[1], 0.0f); // Y축
				modelMatrix[2] = glm::vec4(rotationMatrix[2], 0.0f); // Z축
				modelMatrix[3] = glm::vec4(cameraPos, 1.0f);          // 위치 추가

				pause[0]->translateMatrix= modelMatrix;
				pause[0]->translateMatrix = glm::translate(pause[0]->translateMatrix, glm::vec3(0.0,0.0,-10.0));
			}
			Pause = !Pause;
		}
	}

	void specialKey(int key, int x, int y) override {
		switch (key) {
		case GLUT_KEY_UP:
			kart_keyState[UP] = true;
			break;
		case GLUT_KEY_DOWN:
			kart_keyState[DOWN] = true;
			break;
		case GLUT_KEY_LEFT:
			kart_keyState[LEFT] = true;
			break;
		case GLUT_KEY_RIGHT:
			kart_keyState[RIGHT] = true;
			break;
		}
	}

	void specialKeyUp(int key, int x, int y) override {
		switch (key) {
		case GLUT_KEY_UP:
			kart_keyState[UP] = false;
			break;
		case GLUT_KEY_DOWN:
			kart_keyState[DOWN] = false;
			break;
		case GLUT_KEY_LEFT:
			kart_keyState[LEFT] = false;
			break;
		case GLUT_KEY_RIGHT:
			kart_keyState[RIGHT] = false;
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
		this->projection = glm::perspective(glm::radians(45.0f), static_cast<float>(width) / static_cast<float>(height), 0.1f, 10000.0f);
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
		for (const auto& c : character) { // 실제 모델 draw
			c->draw(shaderProgramID, isKeyPressed_s);
		}
		for (const auto& barricate : road1_barricate) { // 실제 모델 draw
			barricate->draw(shaderProgramID, isKeyPressed_s);
		}
		if (start_count >= 0 && start_count < 4) {
			countDown[start_count]->draw(shaderProgramID, isKeyPressed_s);
		}
		if (Pause)
			pause[0]->draw(shaderProgramID, isKeyPressed_s);

		glDisable(GL_DEPTH_TEST);
	}

	void draw_bb() override {

		for (const auto& model : karts) { // 모델 bb draw
			model->draw_rigidBody(shaderProgramID);
		}
		for (const auto& barricate : road1_barricate) { // 모델 bb draw
			barricate->draw_rigidBody(shaderProgramID);
		}
	}

	void finish() override {

	}
private:

	static void timerHelper(int value) {
		if (Map1_Mode* instance = dynamic_cast<Map1_Mode*>(Mode::currentInstance)) {
			instance->timer(); // 인스턴스의 timer 호출
			if (!instance->Pause) {
				glutTimerFunc(16, timerHelper, value); // 타이머 반복 호출
			}
		}
		glutPostRedisplay();

	}


	// bgm 실행 함수
	void backgroundSound() {
		play_sound2D("village_04.ogg", "./asset/map_1/", true, &isBackgroundSound);
	}

};