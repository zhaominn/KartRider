#pragma once
#include <iostream>
#include "Mode.h"
#include "RoadModel.h"
#include "KartModel.h"
#include "shaderMaker.h"
#include "LoadProgress.h"
#include "root.h"
#include "KeyBoard.h"


class LogoMode : public Mode {
public:
	void init() override {

		// play mode에 필요한 모델 로드
		loadModelWithProgress <KartModel>("kronos.obj", "obj/car/kronos/", "car", "sphere", glm::scale(glm::mat4(1.0f), glm::vec3(10.0, 10.0, 10.0)), karts);

		initializeModelsWithPhysics(karts); // 모든 모델 Bullet world에 추가
	}

	void keyboard() override {

	}

	void draw_model() override {
		for (const auto& kart : karts) { // 실제 모델 draw
			kart->draw(shaderProgramID, isKeyPressed_s);
		}
	}

	void draw_bb() override {
		for (const auto& model : karts) { // 모델 bb draw
			model->draw_rigidBody(shaderProgramID);
		}
	}

	void finish() override {

	}

};
