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

class LogoMode : public Mode {
public:
    bool isRunning; // 프로그램 실행 상태를 나타내는 플래그

    /* 기본 생성자 필수~~ */
    LogoMode() : isRunning(true) {}

    void init() override {
        // 1. 동영상 재생 스레드 시작
        std::thread videoThread(&LogoMode::runVideo, this);

        // 2. 사운드 재생 스레드 시작
        std::thread soundThread(&LogoMode::runSound, this);

        // 3. 모델 로드 및 Bullet Physics 초기화 (메인 스레드에서 실행)
        loadModelWithProgress<KartModel>("kronos.obj", "obj/car/kronos/", "car", "sphere", glm::scale(glm::mat4(1.0f), glm::vec3(1.0, 1.0, 1.0)), karts);
        loadModelWithProgress<RoadModel>("road_all_1.obj", "obj/road/", "road_all", "cube", glm::scale(glm::mat4(1.0f), glm::vec3(100.0, 100.0, 100.0)), roads);
        initializeModelsWithPhysics(karts);
        initializeModelsWithPhysics(roads);

        // 5. 동영상 및 사운드 스레드 종료 대기
        videoThread.join();
        soundThread.join();

        // 4. 프로그램 종료 플래그 설정
        isRunning = false;
        SelectMapMode* selectMapMode = new SelectMapMode();
        MM.SetMode(selectMapMode);
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

private:
    // 동영상 실행 함수
    void runVideo() {
        loadVideo("kartrider_intro.mp4", "./asset/", &isRunning);
    }

    // 사운드 실행 함수
    void runSound() {
        play_sound2D("kartrider_intro.WAV", "./asset/", false, &isRunning);
    }
};