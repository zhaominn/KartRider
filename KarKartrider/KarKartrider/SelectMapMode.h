#pragma once
#include <iostream>
#include "Mode.h"
#include "shaderMaker.h"
#include "root.h"
#include "KeyBoard.h"
#include "PlayMode.h"


bool isAnimating = false;  // 카메라가 이동 중인지 확인
float animationSpeed = 0.05f; // 애니메이션 속도 (0.05 = 부드러운 이동)
glm::vec3 targetCameraPos = glm::vec3(0.0, 0.0, 5.0);   // 목표 카메라 위치
glm::vec3 cameraPosMapMode = glm::vec3(0.0, 0.0, 5.0);

void timer(int value) {
	if (isAnimating) {
		// 현재 카메라 위치와 목표 위치(targetCameraPos)를 LERP
		cameraPosMapMode = glm::mix(cameraPosMapMode, targetCameraPos, animationSpeed);

		// 카메라가 목표 위치에 거의 도달했는지 확인 (오차 범위 허용)
		if (glm::distance(cameraPosMapMode, targetCameraPos) < 0.01f) {
			cameraPosMapMode = targetCameraPos; // 목표 위치로 정확히 설정
			isAnimating = false; // 애니메이션 종료
		}
	}

	glutPostRedisplay();            // 화면 업데이트 요청
	glutTimerFunc(16, timer, 0);    // 약 60FPS로 타이머 반복 호출
}


class SelectMapMode : public Mode {
public:
	
	int map_num = 1;

	glm::vec3 map1CamerPos = glm::vec3(0.0, 0.0, 5.0);      // Map1 카메라 위치
	glm::vec3 map2CamerPos = glm::vec3(2.5, 0.0, 5.0);      // Map2 카메라 위치

	glm::vec3 cameraDirectionMapMode = glm::vec3(0.0, 0.0, 0.0);
	glm::vec3 cameraUpMapMode = glm::vec3(0.0, 1.0, 0.0);
	glm::mat4 projectionMapMode = glm::mat4(1.0f);
	glm::mat4 viewMapMode = glm::mat4(1.0f);



	SelectMapMode() {}


	void init() override {
		//// 카메라 위치를 Y축 기준으로 회전
		//float angleInRadians = glm::radians(-15.0f); // 10도 회전
		//glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), angleInRadians, glm::vec3(0.0f, 1.0f, 0.0f));

		//glm::vec4 rotatedCameraPos = rotation * glm::vec4(cameraPosMapMode, 1.0f);
		//cameraPosMapMode = glm::vec3(rotatedCameraPos); // 회전된 위치를 카메라 위치로 적용
		targetCameraPos = cameraPosMapMode; // 시작 위치를 목표 위치로 설정
		glutTimerFunc(0, timer, 0);         // 타이머 함수 시작
	}


	void keyboard(unsigned char key, int x, int y) override {
		switch (key) {
		case '\r': {
			PlayMode* playMode = new PlayMode();
			playMode->map_num = map_num;
			MM.SetMode(playMode);
			break;
		}
		default:
			break;
		}
	}

	void updateTargetCameraPos() {
		if (map_num == 1) {
			targetCameraPos = map1CamerPos;
		}
		else if (map_num == 2) {
			targetCameraPos = map2CamerPos;
		}

		isAnimating = true; // 애니메이션 시작
	}

	void keySpecial(int key, int x, int y) override {
		if (key == GLUT_KEY_LEFT) {
			map_num--;
			if (map_num < 1) {
				map_num = 1;
			}
			this->updateTargetCameraPos(); // 목표 카메라 위치 업데이트
		}
		else if (key == GLUT_KEY_RIGHT) {
			map_num++;
			if (map_num > 2) {
				map_num = 2;
			}
			this->updateTargetCameraPos(); // 목표 카메라 위치 업데이트
		}

		cout << map_num << endl;
	}

	void draw_model() override {

		glClearColor(0.0, 0.0, 0.0, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(shaderProgramID);


		GLenum error = glGetError();
		if (error != GL_NO_ERROR) {
			std::cout << "Error in glUseProgram: " << error << std::endl;
		}
		
		// 정면을 바라보도록 고정 (Z축 -1 방향)
		glm::vec3 fixedLookDirection = glm::vec3(0.0, 0.0, -1.0); // 항상 정면(-Z) 방향
		glm::vec3 lookAtTarget = cameraPosMapMode + fixedLookDirection;

		// View 행렬 설정
		viewMapMode = glm::lookAt(
			cameraPosMapMode,       // 카메라 위치
			lookAtTarget,           // 정면을 바라보도록 설정
			cameraUpMapMode         // 업 벡터
		);
		unsigned int viewLocation = glGetUniformLocation(shaderProgramID, "viewTransform");
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &viewMapMode[0][0]);

		projectionMapMode = glm::mat4(1.0f);
		projectionMapMode = glm::perspective(glm::radians(45.0f), static_cast<float>(width) / static_cast<float>(height), 0.1f, 300.0f);
		unsigned int projectionLocation = glGetUniformLocation(shaderProgramID, "projectionTransform");
		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projectionMapMode[0][0]);

		glEnable(GL_DEPTH_TEST);

		for (const auto& maps : selectMaps) { // 실제 모델 draw
			maps->draw(shaderProgramID, isKeyPressed_s);
		}

		glDisable(GL_DEPTH_TEST);
	}

	void draw_bb() override {
	}

	void finish() override {

	}

};
