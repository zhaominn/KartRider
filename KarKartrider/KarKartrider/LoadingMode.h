#pragma once
#include <iostream>
#include <thread>

#include "Mode.h"
#include "shaderMaker.h"
#include "root.h"
#include "KeyBoard.h"
#include "LoadVideo.h"
#include "LoadSound.h"
#include "SelectMapMode.h"
#include "LoadProgress.h"

class LoadingMode : public Mode {
public:
    /* 기본 생성자 필수~~ */
    LoadingMode() : {}

    void init() override {

    }

    void keyboard(unsigned char key, int x, int y) override {
        switch (key) {
        case '\r': {
            SelectMapMode* selectMapMode = new SelectMapMode();
            MM.SetMode(selectMapMode);
            break;
        }
        default:
            break;
        }
    }

    void draw_model() override {}

    void draw_bb() override {}

    void finish() override {}

};