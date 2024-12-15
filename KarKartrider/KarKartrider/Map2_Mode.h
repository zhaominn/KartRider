#pragma once
#include <iostream>
#include "RoadModel.h"
#include "KartModel.h"
#include "shaderMaker.h"
#include "LoadProgress.h"
#include "root.h"
#include "KeyBoard.h"
#include "Light.h"

#include <gl/glm/glm/gtc/quaternion.hpp> // 쿼터니언 관련
#include <gl/glm/glm/gtx/quaternion.hpp> // SLERP(Spherical Linear Interpolation)
#include <functional>  // std::function을 사용하기 위해 필요

class Map2_Mode : public Mode {
public:

	std::function<void()> goSelectMode; // 셀렉트 모드로 돌아가는 함수

	glm::quat cameraRotationQuat = glm::quat(glm::vec3(0.0f, 0.0f, 0.0f)); // 현재 카메라 행렬을 쿼터니언으로 저장
	float reducedRotationInfluence = 0.0f; // 보간할 퍼센트

	GLfloat kart_speed = 0.0f;

	enum Move { NONE_M, UP, DOWN, LEFT, RIGHT, CTRL };
	float ACCELERATION = 0.004f;
	float DECELERATION = 0.003f;
	float LIMIT_SPEED = 1.0;
	float BOOSTER_SPEED = 2.0;
	float MAX_SPEED = 1.0;

	int finish_cnt = 0;

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
	bool isMotorSound = false;
	std::thread motorSoundThread;
	bool isCrashSound = false;
	std::thread crashSoundThread;
	bool isBoosterSound = false;
	std::thread boosterSoundThread;
	bool isWinSound = false;
	std::thread winSoundThread;
	bool isLoseSound = false;
	std::thread loseSoundThread;

	// ----- game ------
	int booster_cnt = 2;
	const int MAX_BOOSTER_CNT = 2;        // 최대 부스터 개수
	bool isBoosterRegenActive = true;     // 부스터 재생성 활성화 여부
	std::thread boosterRegenThread;       // 부스터 재생성 스레드
	bool isBoosterActive = false; // 부스트 활성화 상태
	bool isGameOver = false; // 게임 종료 상태 플래그
	int game_timer = 30;

	//캐릭터 얼굴 회전 각도
	float character_face_rotation = 0.0f; // 캐릭터 얼굴의 현재 Y축 회전 각도
	const float MAX_FACE_ROTATION = 25.0f; // 고개가 좌우로 최대 회전할 각도 (도 단위)
	const float ROTATION_SPEED = 5.0f;     // 고개 회전 속도 (프레임당 회전 각도)
	const float RETURN_SPEED = 2.0f;       // 고개가 정면으로 돌아가는 속도 (프레임당 회전 각도)

	float booster_head_tilt = 0.0f; // 캐릭터 머리의 X축 회전 각도
	const float MAX_HEAD_TILT = 20.0f; // 부스터 사용 시 최대 X축 회전 각도
	const float TILT_SPEED = 2.0f;     // 부스터 시 머리의 회전 속도

	Map2_Mode() {
		Mode::currentInstance = this;  // Map1_Mode 인스턴스를 currentInstance에 할당
		isCountNSound = true;
		isCountGoSound = true;
		// 부스터 재생성 스레드 시작
		boosterRegenThread = std::thread(&Map2_Mode::startBoosterRegen, this);
	}
	~Map2_Mode(){}

	// 부스터 재생성 로직
	void startBoosterRegen() {
		while (isBoosterRegenActive) {
			// 부스터가 최대 개수에 도달하지 않았을 때만 대기 후 증가
			if (booster_cnt < MAX_BOOSTER_CNT) {
				std::this_thread::sleep_for(std::chrono::seconds(6)); // 3초 대기

				// 부스터 증가 (다시 확인해 조건 충족 시 증가)
				if (booster_cnt < MAX_BOOSTER_CNT) {
					++booster_cnt; // 부스터 개수 증가
					std::cout << "Booster regenerated! Current boosters: " << booster_cnt << std::endl;
				}
			}
			else {
				// 만약 부스터가 이미 최대치라면 일정 시간 대기 후 다시 확인
				std::this_thread::sleep_for(std::chrono::milliseconds(100)); // 0.1초 대기
			}
		}
	}

	void draw_dashBoard() {
		glUseProgram(shaderProgramID_UI);

		// 텍스처 활성화 플래그
		GLint isTextureLocation = glGetUniformLocation(shaderProgramID_UI, "isTexture");
		glUniform1i(isTextureLocation, true);

		// 텍스처 모델 렌더링
		for (const auto& dashBoard : dashBoards) {
			dashBoard->draw(shaderProgramID_UI, isKeyPressed_s);
		}
		glUniform1i(isTextureLocation, false);

		glUseProgram(0); // 원래 셰이더로 복원
	}

	void draw_speed() {
		glUseProgram(shaderProgramID_UI); // UI 렌더링용 셰이더 활성화

		// isUI 플래그 활성화
		GLint isUILocation = glGetUniformLocation(shaderProgramID_UI, "isTimer");
		glUniform1i(isUILocation, true); // UI 모드 활성화

		glPushMatrix();
		glPixelZoom(5.0f, 5.0f);  // x축, y축 확대 비율 (2배 확대)

		// 자동차 속도 문자열 생성
		std::string speedText = std::to_string(static_cast<int>(kart_speed * 100));

		// 자동차 속도를 화면 우측 상단에 표시
		glRasterPos2f(0.0f, -0.97f); // 화면 우측 상단
		for (char c : speedText) {
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
		}

		glPixelZoom(1.0f, 1.0f);  // 원래 크기로 복구
		glPopMatrix();
		glUniform1i(isUILocation, false); // UI 모드 활성화

		glUseProgram(0); // 원래 셰이더로 복원
	}

	void draw_ui() {
		glUseProgram(shaderProgramID_UI);

		// 활성화 플래그
		GLint isUILocation = glGetUniformLocation(shaderProgramID_UI, "isTimer");
		glUniform1i(isUILocation, true);

		std::string uiText = "map : village road";
		glRasterPos2f(-0.95f, 0.85f); // 좌상단 위치
		for (char c : uiText) {
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
		}
		glUniform1i(isUILocation, false);

		// 텍스처 활성화 플래그
		GLint isTextureLocation = glGetUniformLocation(shaderProgramID_UI, "isTexture");
		glUniform1i(isTextureLocation, true);

		// 텍스처 모델 렌더링
		for (int i = 0; i < booster_cnt; ++i) {
			booster_uis[i]->draw(shaderProgramID_UI, isKeyPressed_s);
		}
		glUniform1i(isTextureLocation, false);

		glUseProgram(0); // 원래 셰이더로 복원
	}

	void draw_timer() {
		glUseProgram(shaderProgramID_UI);

		// 활성화 플래그
		GLint isTimerLocation = glGetUniformLocation(shaderProgramID_UI, "isTimer");
		glUniform1i(isTimerLocation, true);

		// 타이머 텍스트
		std::string timerText = "Time: " + std::to_string(game_timer);
		glRasterPos2f(-0.95f, 0.9f); // 좌상단 위치
		for (char c : timerText) {
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
		}
		glUniform1i(isTimerLocation, false);

		glUseProgram(0); // 원래 셰이더로 복원
	}

	void init() override {

		UpdateRigidBodyTransforms(road2_barricate);
		UpdateRigidBodyTransforms(karts);

		// Move 상태 초기화
		kart_keyState[UP] = false;
		kart_keyState[DOWN] = false;
		kart_keyState[LEFT] = false;
		kart_keyState[RIGHT] = false;

		start_count = -1;

		for (const auto& kart : karts) { // 카트 위치 초기화
			kart->translateMatrix = glm::mat4(1.0f);
			kart->translateMatrix = glm::translate(kart->translateMatrix, glm::vec3(165.0, 1.0, 30.0));
		}
		for (const auto& c : character) { //카트와 같은 행렬 적용
			c->translateMatrix = karts[0]->translateMatrix;
		}
		for (const auto& c : countDown) { //카트와 같은 행렬 적용
			c->translateMatrix = karts[0]->translateMatrix;
			c->translateMatrix = glm::translate(c->translateMatrix, glm::vec3(0.0, 4.0, 0.0));
		}

		isBackgroundSound = true;
		backgroundSoundThread = std::thread(&Map2_Mode::backgroundSound, this);

		

		kart_speed = 0.0f;
		draw_model();

		if (!isGameRunning2)
		{
			isGameRunning2 = true;
			glutTimerFunc(0, Map2_Mode::timerHelper, 0);
		}
		cameraPos = glm::vec3(165.0, 4.4, 45.0);
		updateCameraDirection();
	}

	void playCountdown(int count) {
		if (count < 3) { // count_n 사운드 3번 실행
			if (countNSoundThread.joinable()) {
				countNSoundThread.join();
			}

			countNSoundThread = std::thread(&Map2_Mode::count_n, this);

			countNSoundThread.join(); // 이 부분은 count_n.wav가 끝날 때까지 기다립니다.
		}
		else if (count == 3) { // count_go 사운드 실행
			if (countGoSoundThread.joinable()) {
				countGoSoundThread.join();
			}

			countGoSoundThread = std::thread(&Map2_Mode::count_go, this);

			// 플래그 초기화
			//isCountGoSound = false;
			isCountNSound = false;
			isBackgroundSound = true;
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
		// "kart_speed"를 사용하여 카메라 목표를 동적으로 설정
		glm::vec3 baseOffset = glm::vec3(0.0f, 6.0f + (kart_speed * 2.0f), 14.0f + (kart_speed * 10.0f));
		glm::vec3 rotatedOffset = adjustedRotationMatrix * baseOffset;

		// 목표 위치 계산
		cameraTargetPos = carPosition + rotatedOffset;

		// 현재 카메라 위치를 목표 위치로 점진적으로 이동 (보간 속도 조정 가능)
		float cameraFollowSpeed = 0.1f; // 카메라가 목표로 따라가는 속도 (0.0 ~ 1.0)
		cameraPos = glm::mix(cameraPos, cameraTargetPos, cameraFollowSpeed);

		// 카메라가 자동차를 바라보도록 방향 업데이트
		cameraDirection = carPosition; // 카메라가 항상 자동차를 바라봄
	}

	void goSelectMode_() {
		Pause = true;
		if (goSelectMode) { // goSelectMode가 설정되어 있다면 실행
			isBackgroundSound = false;
			isMotorSound = false;
			if (motorSoundThread.joinable()) {
				motorSoundThread.join();
			}
			goSelectMode();
		}
		isBoosterRegenActive = false; // 부스터 재생성 종료
		if (boosterRegenThread.joinable()) {
			boosterRegenThread.join(); // 스레드 종료
		}
	}

	void finish_game() {
		if (finish_cnt != 0) {
			isBackgroundSound = false;
			if (isWinSound) return; // 이미 실행 중이면 종료
			isWinSound = true;
			isGameOver = true; // 게임 종료 상태 설정

			// 새로운 스레드 생성 및 분리
			winSoundThread = std::thread([this]() {
				win_sound();  // 사운드 재생
				isWinSound = false; // 사운드 재생 완료 후 플래그 해제
				});

			winSoundThread.detach();

			// 5초 후 goSelectMode_() 실행을 위한 스레드 생성
			std::thread([this]() {
				std::this_thread::sleep_for(std::chrono::seconds(9)); // 5초 대기
				goSelectMode_(); // 5초 후 실행
				}).detach();
		}
		else {
			return;
		}
	}

	void draw_finish_time() {
		glUseProgram(shaderProgramID_UI);

		// 활성화 플래그
		GLint isTimerLocation = glGetUniformLocation(shaderProgramID_UI, "isRed");
		glUniform1i(isTimerLocation, true);

		// 타이머 텍스트
		std::string Text = "Time: " + std::to_string(30 - game_timer);

		glRasterPos2f(0.0f, 0.0f);
		for (char c : Text) {
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
		}
		glUniform1i(isTimerLocation, false);

		glUseProgram(0); // 원래 셰이더로 복원
	}

	void lose_game() {
		if (isGameOver) return; // 이미 종료 상태라면 실행하지 않음

		isGameOver = true; // 게임 종료 상태 설정
		isBackgroundSound = false; // 배경음악 종료

		std::cout << "Game Over! Time is up!" << std::endl;

		// 패배 사운드 재생
		if (!isLoseSound) {
			isLoseSound = true;
			loseSoundThread = std::thread([this]() {
				lose_sound();
				isLoseSound = false; // 사운드 재생 완료 후 플래그 해제
				});
			loseSoundThread.detach();

			// 5초 후 goSelectMode_() 실행을 위한 스레드 생성
			std::thread([this]() {
				std::this_thread::sleep_for(std::chrono::seconds(9)); // 5초 대기
				goSelectMode_(); // 5초 후 실행
				}).detach();
		}
	}

	void checkCollisionKart() {
		for (auto& kart : karts) {
			if (kart->name != "car") continue; // 카트가 "car" 이름이 아니면 스킵

			// 중력을 제거 (한 번만 설정)
			kart->rigidBody->setGravity(btVector3(0.0f, 0.0f, 0.0f));

			for (const auto& barri : road2_barricate) {
				// 충돌 콜백 객체 생성
				CustomContactResultCallback resultCallback;

				// 두 객체의 충돌 감지
				dynamicsWorld->contactPairTest(kart->rigidBody, barri->rigidBody, resultCallback);

				if (resultCallback.hitDetected) { // 충돌이 감지되었을 때

					if (barri->name == "finish_ch") {
						finish_cnt++;
						continue;
					}

					if (barri->name == "finish") { //종료~~~~
						finish_game();
						continue;
					}

					// 충돌 사운드 재생 (isCrashSound로 중복 재생 방지)
					if (!isCrashSound) {
						isCrashSound = true;
						crashSoundThread = std::thread(&Map2_Mode::crash_sound, this);
						crashSoundThread.detach(); // 스레드를 분리하여 비동기 재생
					}

					// 1. 충돌 방향 및 침투 깊이 가져오기
					btVector3 collisionNormal = resultCallback.collisionNormal; // 충돌 방향
					collisionNormal.setY(0.0f); // y축 방향 제거 (xz 평면에서만 처리)

					// 침투 깊이 제한
					float penetrationDepth = std::abs(resultCallback.penetrationDepth);
					const float MAX_PENETRATION_DEPTH = 2.0f; // 최대 침투 깊이
					if (penetrationDepth > MAX_PENETRATION_DEPTH) {
						penetrationDepth = MAX_PENETRATION_DEPTH;
					}

					// 2. 보정 값 계산 (침투 깊이에 기반한 이동)
					float correctionScale = 0.5f; // 보정 값 감쇠 계수
					btVector3 correction = correctionScale * collisionNormal * penetrationDepth;

					// 3. 속도 기반 추가 이동 적용
					glm::vec3 kartVelocity = glm::vec3(-kart->translateMatrix[2]) * kart_speed;
					float speedFactor = glm::length(kartVelocity); // 카트의 속도 크기
					correction += collisionNormal * speedFactor * 0.2f; // 속도에 비례한 추가 이동 (0.2는 조정 가능)

					// 4. 카트 위치 업데이트
					btTransform kartTransform;
					kart->rigidBody->getMotionState()->getWorldTransform(kartTransform);
					btVector3 kartPos = kartTransform.getOrigin();

					btVector3 newKartPos = kartPos + correction; // 최종 위치 계산
					newKartPos.setY(1.0f); // y축 고정

					kartTransform.setOrigin(newKartPos);

					// 물리 엔진에 새로운 Transform 적용
					kart->rigidBody->getMotionState()->setWorldTransform(kartTransform);
					kart->rigidBody->setWorldTransform(kartTransform);

					// OpenGL 변환 행렬에 반영
					btScalar transformMatrix[16];
					kartTransform.getOpenGLMatrix(transformMatrix);
					kart->translateMatrix = glm::make_mat4(transformMatrix);

					// 5. 속도 감소 처리
					float decelerationFactor = 0.2f; // 감속 비율 (충돌 후 속도 감소)
					kart_speed *= 1.0f - decelerationFactor;
					if (kart_speed < 0.01f) { // 너무 느려지면 정지
						kart_speed = 0.0f;
					}
				}
			}
		}
	}


	void checkEngineSound() {
		if (kart_speed != 0.0f) {
			if (!isMotorSound) { // 엔진 사운드가 재생 중이지 않을 때만 실행
				isMotorSound = true;
				motorSoundThread = std::thread(&Map2_Mode::engine_sound, this); // 엔진 사운드 시작
			}
		}
		else { // 속도가 0일 경우
			if (isMotorSound) {
				isMotorSound = false;
				if (motorSoundThread.joinable()) {
					motorSoundThread.detach(); // 스레드 종료 (필요한 경우 detach)
				}
			}
		}
	}

	void timer() {
		if (!Pause) {
			if (start_count < 4) {
				if (start_count >= 0)
					playCountdown(start_count);
				++start_count;
			}
			else {

				// 가속/감속 처리
				if (kart_keyState[UP]) {
					if (kart_speed < MAX_SPEED) {
						kart_speed += ACCELERATION; // 가속도에 따라 증가
						if (kart_speed > MAX_SPEED) kart_speed = MAX_SPEED; // 최대 속도 제한
					}
				}
				else if (kart_keyState[DOWN]) {
					if (kart_speed > -MAX_SPEED / 2.0f) { // 후진 속도는 전진의 절반까지만 허용
						kart_speed -= ACCELERATION; // 후진 시에도 가속도 반영
						if (kart_speed < -MAX_SPEED / 2.0f) kart_speed = -MAX_SPEED / 2.0f; // 후진 최대 속도 제한
					}
				}
				else {
					// 속도가 감소할 때 감속
					if (kart_speed > 0.0f) {
						kart_speed -= DECELERATION; // 전진 감속
						if (kart_speed < 0.0f) kart_speed = 0.0f; // 0으로 안정화
					}
					else if (kart_speed < 0.0f) {
						kart_speed += DECELERATION; // 후진 감속
						if (kart_speed > 0.0f) kart_speed = 0.0f; // 0으로 안정화
					}
				}

				// 속도를 제한 (MAX_SPEED를 초과하지 않도록)
				if (kart_speed > MAX_SPEED) kart_speed = MAX_SPEED;

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

				// 고개가 천천히 정면으로 돌아가도록 보간
				if (!kart_keyState[LEFT] && !kart_keyState[RIGHT]) {
					if (character_face_rotation > 0.0f) {
						character_face_rotation -= RETURN_SPEED;
						if (character_face_rotation < 0.0f) {
							character_face_rotation = 0.0f; // 정면으로 고정
						}
					}
					else if (character_face_rotation < 0.0f) {
						character_face_rotation += RETURN_SPEED;
						if (character_face_rotation > 0.0f) {
							character_face_rotation = 0.0f; // 정면으로 고정
						}
					}
				}

				if (isBoosterActive) {
					// 머리를 X축으로 뒤로 기울이기 (서서히 MAX_HEAD_TILT까지)
					if (booster_head_tilt < MAX_HEAD_TILT) {
						booster_head_tilt += TILT_SPEED;
						if (booster_head_tilt > MAX_HEAD_TILT) {
							booster_head_tilt = MAX_HEAD_TILT;
						}
					}
				}
				else {
					// 머리를 원래 상태로 복구 (서서히 0도로 복귀)
					if (booster_head_tilt > 0.0f) {
						booster_head_tilt -= TILT_SPEED;
						if (booster_head_tilt < 0.0f) {
							booster_head_tilt = 0.0f;
						}
					}
				}

				// 캐릭터 모델 업데이트 (머리의 X축 회전 포함)
				for (const auto& c : character) {
					if (c->name == "character_face") {
						// 기존 Y축 회전 (character_face_rotation) 추가
						glm::mat4 headRotation = glm::rotate(
							glm::mat4(1.0f),
							glm::radians(-character_face_rotation),
							glm::vec3(0.0f, 0.0f, 1.0f)
						);

						// X축 회전 추가 (부스터 상태에 따른 머리 기울임)
						headRotation = glm::rotate(
							headRotation,
							glm::radians(booster_head_tilt), // X축 회전
							glm::vec3(1.0f, 0.0f, 0.0f)
						);

						c->translateMatrix = karts[0]->translateMatrix * headRotation;
					}
					else {
						c->translateMatrix = karts[0]->translateMatrix;
					}
				}

				// 카메라 업데이트
				setCamera();
				// 현재 카메라 위치를 목표 위치로 점진적으로 이동
				float cameraFollowSpeed = 0.1f; // 카메라가 따라가는 속도 (0.0 ~ 1.0 사이의 값)
				cameraPos = glm::mix(cameraPos, cameraTargetPos, cameraFollowSpeed);

				checkCollisionKart();
				checkEngineSound();
			}
		}
	}

	void mouseClick(int button, int state, int x, int y) override {
		if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
			if (x <= 470 && x >= 400 && y <= 410 && y >= 360) { //다시시도
				Pause = true;
				isBackgroundSound = false;
				isMotorSound = false;
				if (motorSoundThread.joinable()) {
					motorSoundThread.join();
				}
				Map2_Mode* map2Mode = new Map2_Mode();
				map2Mode->goSelectMode = [this]() { goSelectMode(); }; // 람다로 전달
				MM.SetMode(map2Mode);
			}
			else if (x <= 580 && x >= 510 && y <= 410 && y >= 360) { //메뉴
				goSelectMode_();
			}
		}
	}

	void keyboard(unsigned char key, int x, int y) override {
		if (key == 27) { //esc
			if (Pause) {
				//glutTimerFunc(16, timerHelper, 0); // 타이머 호출
				//isMotorSound = true;
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
				pause[0]->translateMatrix = modelMatrix;
				pause[0]->translateMatrix = glm::translate(pause[0]->translateMatrix, glm::vec3(0.0, 0.0, -2.0));


				isMotorSound = false;
				if (motorSoundThread.joinable()) {
					motorSoundThread.detach(); // 스레드 종료 (필요한 경우 detach)
				}
			}
			Pause = !Pause;
		}
		if (key == 'p') {
			goSelectMode_();
		}
	}

	// 부스터 실행 함수
	void activateBooster() {

		// 부스트가 이미 활성화 중이면 실행하지 않음
		if (isBoosterActive) {
			std::cout << "Booster is already active!" << std::endl;
			return;
		}

		// 부스트 활성화 상태로 설정
		isBoosterActive = true;

		std::cout << "Booster activated! Remaining boosters: " << booster_cnt << std::endl;

		// 기존 MAX_SPEED와 ACCELERATION 값을 저장
		float originalMaxSpeed = MAX_SPEED;
		float originalAcceleration = ACCELERATION;

		// 부스터 속도 및 가속도 설정
		MAX_SPEED = BOOSTER_SPEED;           // 부스터 속도
		ACCELERATION *= 1.05f;                // 가속도를 1.5배로 증가 (조정 가능)

		// 부스터 사운드 재생
		if (!isBoosterSound) {
			isBoosterSound = true;
			boosterSoundThread = std::thread(&Map2_Mode::booster_sound, this);
			boosterSoundThread.detach(); // 비동기 실행
		}

		// 부스터 종료 후 속도 및 가속도 복구
		std::thread([this, originalMaxSpeed, originalAcceleration]() {
			std::this_thread::sleep_for(std::chrono::duration<double>(4.4)); // 부스터 지속 시간 3초
			MAX_SPEED = originalMaxSpeed;        // 원래 속도 복구
			ACCELERATION = originalAcceleration; // 원래 가속도 복구
			isBoosterActive = false;             // 부스트 비활성화 상태로 설정
			std::cout << "Booster ended. MAX_SPEED and ACCELERATION restored." << std::endl;
			}).detach();
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
			// 왼쪽 방향으로 고개 회전
			if (character_face_rotation > -MAX_FACE_ROTATION) {
				character_face_rotation -= ROTATION_SPEED;
			}
			break;
		case GLUT_KEY_RIGHT:
			kart_keyState[RIGHT] = true;
			// 오른쪽 방향으로 고개 회전
			if (character_face_rotation < MAX_FACE_ROTATION) {
				character_face_rotation += ROTATION_SPEED;
			}
			break;
		}

		int modifiers = glutGetModifiers();

		// Ctrl 단독 감지
		if (modifiers & GLUT_ACTIVE_CTRL) {
			// 부스트가 이미 활성화 중이거나 부스트 개수가 0이면 실행하지 않음
			if (isBoosterActive) {
				std::cout << "Booster is already active!" << std::endl;
				return;
			}

			if (booster_cnt > 0) { // 부스터가 남아 있는 경우
				booster_cnt--; // 부스터 개수 감소
				activateBooster(); // 부스터 활성화
			}
			else {
				std::cout << "No boosters left!" << std::endl;
			}
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
		for (const auto& road : road2) { // 실제 모델 draw
			road->draw(shaderProgramID, isKeyPressed_s);
		}
		for (const auto& c : character) { // 실제 모델 draw
			if (c->name == "booster" && !isBoosterActive)
				continue;
			c->draw(shaderProgramID, isKeyPressed_s);
		}
		for (const auto& barricate : road2_barricate) { // 실제 모델 draw
			barricate->draw(shaderProgramID, isKeyPressed_s);
		}
		if (start_count >= 0 && start_count < 4) {
			countDown[start_count]->draw(shaderProgramID, isKeyPressed_s);
		}

		if (Pause)
			pause[0]->draw(shaderProgramID, isKeyPressed_s);

		// Draw Timer
		glDisable(GL_DEPTH_TEST);
		draw_timer();
		draw_ui();
		draw_dashBoard();
		draw_speed();
		if (isGameOver)
			draw_finish_time();
		glEnable(GL_DEPTH_TEST);

		glDisable(GL_DEPTH_TEST);
	}

	void draw_bb() override {
		if (!bb_status)
			return;
		for (const auto& model : karts) { // 모델 bb draw
			model->draw_rigidBody(shaderProgramID);
		}
		for (const auto& barricate : road2_barricate) { // 모델 bb draw
			barricate->draw_rigidBody(shaderProgramID);
		}
	}

	void finish() override {

	}
private:

	void updatePhysics(float deltaTime) {
		// 물리 엔진 업데이트 (deltaTime에 따라 정확도 조절)
		dynamicsWorld->stepSimulation(deltaTime);

		// 물리 엔진에서 객체의 Transform 업데이트
		UpdateRigidBodyTransforms(karts);
		UpdateRigidBodyTransforms(road2_barricate);

		// 충돌 처리 (물리 엔진 업데이트 후 실행)
		checkCollisionKart();
	}

	static void timerHelper(int value) {
		if (Map2_Mode* instance = dynamic_cast<Map2_Mode*>(Mode::currentInstance)) {
			const float deltaTime = 1.0f / 60.0f; // 60FPS 기준, 한 프레임의 시간

			// 물리 엔진 및 게임 로직 업데이트 (게임 종료 상태에서도 계속 실행)
			instance->updatePhysics(deltaTime); // 물리 엔진 업데이트
			instance->timer(); // 렌더링 및 게임 로직 업데이트

			// 게임 타이머 갱신 (게임 종료 상태에서는 타이머를 감소시키지 않음)
			if (!instance->isGameOver) {
				static float elapsedTime = 0.0f;
				elapsedTime += deltaTime;
				if (elapsedTime >= 1.0f) { // 1초가 지났다면
					elapsedTime = 0.0f;
					instance->game_timer--; // 타이머 1초 감소
					if (instance->game_timer <= 0) { // 타이머가 0이 되면
						instance->game_timer = 0;
						instance->lose_game(); // 패배 처리
					}
				}
			}
		}

		// 렌더링 업데이트를 60FPS로 유지
		glutPostRedisplay();
		glutTimerFunc(16, timerHelper, value); // 16ms 간격으로 호출 (약 60FPS)
	}


	// bgm 실행 함수
	void backgroundSound() {
		play_sound2D("village_04.ogg", "./asset/map_1/", true, &isBackgroundSound);
	}
	void count_go() {
		play_sound2D("count_go.wav", "./asset/map_1/", false, &isCountGoSound);
	}
	void count_n() {
		play_sound2D("count_n.wav", "./asset/map_1/", false, &isCountNSound);
	}
	void engine_sound() {
		play_sound2D("motor_x.ogg", "./asset/map_1/", true, &isMotorSound);
	}
	void crash_sound() {
		play_sound2D("crash.ogg", "./asset/map_1/", false, &isCrashSound);
		isCrashSound = false;
	}
	void booster_sound() {
		play_sound2D("booster.ogg", "./asset/map_1/", false, &isBoosterSound);
		isBoosterSound = false;
	}
	void win_sound() {
		play_sound2D("game_win.ogg", "./asset/map_1/", false, &isWinSound);
		isWinSound = false;
	}
	void lose_sound() {
		play_sound2D("game_lose.ogg", "./asset/map_1/", false, &isLoseSound);
		isLoseSound = false;
	}
};