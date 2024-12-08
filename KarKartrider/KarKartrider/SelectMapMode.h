#pragma once
#include <iostream>
#include <thread>
#include <random>

#include "Mode.h"
#include "shaderMaker.h"
#include "root.h"
#include "KeyBoard.h"
//#include "PlayMode.h"
#include "Map1_Mode.h"
#include "Map2_Mode.h"


bool isAnimating = false;  // 카메라가 이동 중인지 확인
float animationSpeed = 0.05f; // 애니메이션 속도 (0.05 = 부드러운 이동)
glm::vec3 targetCameraPos = glm::vec3(0.0, 0.0, 5.0);   // 목표 카메라 위치
glm::vec3 cameraPosMapMode = glm::vec3(0.0, 0.0, 5.0);

void SelectMap_timer(int value) {
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
	glutTimerFunc(16, SelectMap_timer, 0);    // 약 60FPS로 타이머 반복 호출
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


	bool isSoundRunning;
	std::thread soundThread;

	bool isclickRunning;

	SelectMapMode() :isSoundRunning(true), isclickRunning(true) {}

	void init() override {

		soundThread = std::thread(&SelectMapMode::runSound, this);

		//// 카메라 위치를 Y축 기준으로 회전
		//float angleInRadians = glm::radians(-15.0f); // 10도 회전
		//glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), angleInRadians, glm::vec3(0.0f, 1.0f, 0.0f));

		//glm::vec4 rotatedCameraPos = rotation * glm::vec4(cameraPosMapMode, 1.0f);
		//cameraPosMapMode = glm::vec3(rotatedCameraPos); // 회전된 위치를 카메라 위치로 적용
		targetCameraPos = cameraPosMapMode; // 시작 위치를 목표 위치로 설정
		glutTimerFunc(0, SelectMap_timer, 0);         // 타이머 함수 시작

	}


	void keyboard(unsigned char key, int x, int y) override {
		std::thread clickSoundThread(&SelectMapMode::clickSound, this);
		switch (key) {
		case '\r': {
			isSoundRunning = false;
			/*
			PlayMode* playMode = new PlayMode();
			playMode->map_num = map_num;
			MM.SetMode(playMode);
			*/
			if (map_num == 1) {
				Map1_Mode* map1Mode = new Map1_Mode();
				MM.SetMode(map1Mode);
			}
			else if (map_num == 2) {
				Map2_Mode* map2Mode = new Map2_Mode();
				MM.SetMode(map2Mode);
			}
			break;
		}
		default:
			break;
		}
		clickSoundThread.detach();
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

	void specialKey(int key, int x, int y) override {
		std::thread clickSoundThread(&SelectMapMode::clickSound, this);
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
		clickSoundThread.detach(); // 스레드 독립 실행 (메인 스레드 대기 없음)
	}

	void specialKeyUp(int key, int x, int y) override {}

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
		// 스레드 안전 종료 처리
		if (soundThread.joinable()) {
			isSoundRunning = false; // 사운드 실행 플래그 중지
			soundThread.join();     // 스레드가 종료될 때까지 대기
		}
	}
private:
	// 사운드 실행 함수
	void runSound() {

		random_device rd;
		mt19937 gen(rd());
		uniform_int_distribution<> file_random_dis(0, 2);

		std::string file1 = "title_xmas_01.ogg";
		std::string file2 = "title_original.ogg";
		std::string file3 = "title_world.ogg";
		std::string file = "";

		switch (file_random_dis(gen))
		{
		case 0:
			file = file1;
			break;
		case 1:
			file = file2;
			break;
		case 2:
			file = file3;
			break;
		default:
			file = file2;
			break;
		}

		play_sound2D(file, "./asset/select_mode/", true, &isSoundRunning);
	}

	void clickSound() {
		isclickRunning = true;
		play_sound2D("click.wav", "./asset/select_mode/", false, &isclickRunning);
		isclickRunning = false;
	}
};