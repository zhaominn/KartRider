#pragma once
#include <iostream>
#include "Mode.h"
#include "RoadModel.h"
#include "KartModel.h"
#include "shaderMaker.h"
#include "LoadProgress.h"
#include "root.h"
#include "KeyBoard.h"


class PlayMode : public Mode {
public:

	PlayMode(){}

	void init() override {

	}

	void keyboard(unsigned char key, int x, int y) override {

	}

	void draw_model() override {
		for (const auto& kart : karts) { // ½ÇÁ¦ ¸ðµ¨ draw
			kart->draw(shaderProgramID, isKeyPressed_s);
		}
		for (const auto& road : roads) { // ½ÇÁ¦ ¸ðµ¨ draw
			road->draw(shaderProgramID, isKeyPressed_s);
		}
	}

	void draw_bb() override {
		for (const auto& model : karts) { // ¸ðµ¨ bb draw
			model->draw_rigidBody(shaderProgramID);
		}
		for (const auto& road : roads) { // ¸ðµ¨ bb draw
			road->draw_rigidBody(shaderProgramID);
		}
	}

	void finish() override {

	}

};
