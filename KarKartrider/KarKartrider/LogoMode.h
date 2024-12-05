#pragma once
#include <iostream>
#include "Mode.h"
#include "RoadModel.h"
#include "KartModel.h"
#include "shaderMaker.h"
#include "LoadProgress.h"
#include "root.h"
#include "KeyBoard.h"
#include "PlayMode.h"

class LogoMode : public Mode {
public:

	/* 기본 생성자 필수~~ */
	LogoMode() {}

	void init() override {

	}

	void keyboard(unsigned char key, int x, int y) override {
		switch (key)
		{
		case 'k':
		{
			PlayMode* playMode = new PlayMode();
			MM.SetMode(playMode);
			break;
		}
		default:
			break;
		}
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
