#pragma once
#include <iostream>
#include "Mode.h"
#include "shaderMaker.h"
#include "root.h"
#include "KeyBoard.h"
#include "LoadImg.h"


class SelectMapMode : public Mode {
public:

	SelectMapMode() {}

	void init() override {
		loadImage("kart1.jpg", "./asset/");
	}

	void keyboard(unsigned char key, int x, int y) override {

	}

	void draw_model() override {
	}

	void draw_bb() override {
	}

	void finish() override {

	}

};
