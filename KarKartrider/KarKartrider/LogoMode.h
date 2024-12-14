#pragma once
#include <iostream>
#include <thread>

#include "shaderMaker.h"
#include "root.h"
#include "KeyBoard.h"
#include "LoadVideo.h"
#include "LoadSound.h"
#include "SelectMapMode.h"
#include "MapModel.h"
#include "BarricateMap1Model.h"
#include "Ui_v2.h"

class LogoMode : public Mode {
public:
    bool isRunning; // 프로그램 실행 상태를 나타내는 플래그

    /* 기본 생성자 필수~~ */
    LogoMode() : isRunning(true) {}
    ~LogoMode() {
        delete this;
    }

    void init() override {
        // 1. 동영상 재생 스레드 시작
        std::thread videoThread(&LogoMode::runVideo, this);

        // 2. 사운드 재생 스레드 시작
        std::thread soundThread(&LogoMode::runSound, this);

        // 3. 모델 로드 및 Bullet Physics 초기화 (메인 스레드에서 실행)
        loadModelWithProgress<KartModel>("bazzi_face2.obj", "obj/character/", "character_face", "box", glm::scale(glm::mat4(1.0f), glm::vec3(1.0, 1.0, 1.0)), character, false, true);
        loadModelWithProgress<KartModel>("bazzi_body.obj", "obj/character/", "character_body", "box", glm::scale(glm::mat4(1.0f), glm::vec3(1.0, 1.0, 1.0)), character, false, true);
        loadModelWithProgress<KartModel>("booster.obj", "obj/car/booster/", "booster", "box", glm::scale(glm::mat4(1.0f), glm::vec3(1.0, 1.0, 1.0)), character, false, true);
        loadModelWithProgress<KartModel>("kronos.obj", "obj/car/kronos/", "car", "box", glm::scale(glm::mat4(1.0f), glm::vec3(1.0, 1.0, 1.0)), karts, true, true);

        loadModelWithProgress<Ui_v2>("booster_ui.obj", "obj/ui/", "booster_ui1", "box", glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(-0.90, 0.75, 0.0)), glm::vec3(0.1, 0.1, 0.1)), booster_uis, false, true);
        loadModelWithProgress<Ui_v2>("booster_ui.obj", "obj/ui/", "booster_ui2", "box", glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(-0.78, 0.75, 0.0)), glm::vec3(0.1, 0.1, 0.1)), booster_uis, false, true);

        loadModelWithProgress<Ui_v2>("dashboard.obj", "obj/ui/", "dashboard", "box", glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(0.0, -0.92, 0.0)), glm::vec3(0.7, 0.6, 0.5)), dashBoards, false, true);

        loadModelWithProgress<RoadModel>("road_all_01.obj", "obj/road/", "road_all_1", "cube", glm::scale(glm::mat4(1.0f), glm::vec3(15.0, 15.0, 15.0)), road1, false, true);
        loadModelWithProgress<BarricateMap1Model>("baricate1.obj", "obj/road/", "baricate", "box", glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(16.7, 0.0, 0.0)), glm::vec3(15.0, 15.0, 700.0)), road1_barricate, true, false);
        loadModelWithProgress<BarricateMap1Model>("baricate1.obj", "obj/road/", "baricate", "box", glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(-16.7, 0.0, 0.0)), glm::vec3(15.0, 15.0, 700.0)), road1_barricate, true, false);
        loadModelWithProgress<BarricateMap1Model>("baricate1.obj", "obj/road/", "baricate", "box", glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, 304.0)), glm::vec3(40.0, 10.0, 10.0)), road1_barricate, true, true);
        loadModelWithProgress<BarricateMap1Model>("baricate1.obj", "obj/road/", "baricate", "box", glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, -304.0)), glm::vec3(40.0, 10.0, 10.0)), road1_barricate, true, true);
        loadModelWithProgress<BarricateMap1Model>("baricate1.obj", "obj/road/", "finish", "box", glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, -212.0)), glm::vec3(40.0, 10.0, 10.0)), road1_barricate, true, false);

        loadModelWithProgress<RoadModel>("road_all_02.obj", "obj/road/", "road_all_2", "cube", glm::scale(glm::mat4(1.0f), glm::vec3(20.0, 20.0, 20.0)), road2, false, true);
        loadModelWithProgress<BarricateMap1Model>("baricate1.obj", "obj/road/", "baricate", "box", glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(184.7, 0.0, 0.0)), glm::vec3(15.0, 15.0, 400.0)), road2_barricate, true, false);
        loadModelWithProgress<BarricateMap1Model>("baricate1.obj", "obj/road/", "baricate", "box", glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(-184.7, 0.0, 0.0)), glm::vec3(15.0, 15.0, 400.0)), road2_barricate, true, false);
        loadModelWithProgress<BarricateMap1Model>("baricate1.obj", "obj/road/", "baricate", "box", glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, 184.7)), glm::vec3(400.0, 15.0, 15.0)), road2_barricate, true, false);
        loadModelWithProgress<BarricateMap1Model>("baricate1.obj", "obj/road/", "baricate", "box", glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, -184.7)), glm::vec3(400.0, 15.0, 15.0)), road2_barricate, true, false);
        //loadModelWithProgress<BarricateMap1Model>("baricate1.obj", "obj/road/", "baricate", "box", glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, 0.0)), glm::vec3(331.0, 331.0, 331.0)), road2_barricate, true, false); //가운데 장애물
        loadModelWithProgress<BarricateMap1Model>("baricate1.obj", "obj/road/", "baricate", "box", glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(145.3, 0.0, 0.0)), glm::vec3(15.0, 15.0, 315.0)), road2_barricate, true, false);
        loadModelWithProgress<BarricateMap1Model>("baricate1.obj", "obj/road/", "baricate", "box", glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(-145.3, 0.0, 0.0)), glm::vec3(15.0, 15.0, 315.0)), road2_barricate, true, false);
        loadModelWithProgress<BarricateMap1Model>("baricate1.obj", "obj/road/", "baricate", "box", glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, 145.3)), glm::vec3(315.0, 15.0, 15.0)), road2_barricate, true, false);
        loadModelWithProgress<BarricateMap1Model>("baricate1.obj", "obj/road/", "baricate", "box", glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, -145.3)), glm::vec3(315.0, 15.0, 15.0)), road2_barricate, true, false);


        // t -> r-> s
        glm::mat4 m1 = glm::mat4(1.0f);
        m1 = glm::translate(m1, glm::vec3(174.7, 0.0, -175.3));
        m1 = glm::rotate(m1, glm::radians(45.0f), glm::vec3(0.0, 1.0, 0.0));
        m1 = glm::scale(m1, glm::vec3(5.0, 15.0, 15.0));
        loadModelWithProgress<BarricateMap1Model>("baricate1.obj", "obj/road/", "baricate", "box", m1, road2_barricate, true, true);

        m1 = glm::mat4(1.0f);
        m1 = glm::translate(m1, glm::vec3(174.7, 0.0, 175.3));
        m1 = glm::rotate(m1, glm::radians(-45.0f), glm::vec3(0.0, 1.0, 0.0));
        m1 = glm::scale(m1, glm::vec3(5.0, 15.0, 15.0));
        loadModelWithProgress<BarricateMap1Model>("baricate1.obj", "obj/road/", "baricate", "box", m1, road2_barricate, true, true);

        m1 = glm::mat4(1.0f);
        m1 = glm::translate(m1, glm::vec3(-174.7, 0.0, 175.3));
        m1 = glm::rotate(m1, glm::radians(45.0f), glm::vec3(0.0, 1.0, 0.0));
        m1 = glm::scale(m1, glm::vec3(5.0, 15.0, 15.0));
        loadModelWithProgress<BarricateMap1Model>("baricate1.obj", "obj/road/", "baricate", "box", m1, road2_barricate, true, true);

        m1 = glm::mat4(1.0f);
        m1 = glm::translate(m1, glm::vec3(-174.7, 0.0, -175.3));
        m1 = glm::rotate(m1, glm::radians(-45.0f), glm::vec3(0.0, 1.0, 0.0));
        m1 = glm::scale(m1, glm::vec3(5.0, 15.0, 15.0));
        loadModelWithProgress<BarricateMap1Model>("baricate1.obj", "obj/road/", "baricate", "box", m1, road2_barricate, true, true);


        loadModelWithProgress<KartModel>("3.obj", "obj/ui/", "countDown", "box", glm::scale(glm::mat4(1.0f), glm::vec3(1.0, 1.0, 1.0)), countDown, true, true);
        loadModelWithProgress<KartModel>("2.obj", "obj/ui/", "countDown", "box", glm::scale(glm::mat4(1.0f), glm::vec3(1.0, 1.0, 1.0)), countDown, true, true);
        loadModelWithProgress<KartModel>("1.obj", "obj/ui/", "countDown", "box", glm::scale(glm::mat4(1.0f), glm::vec3(1.0, 1.0, 1.0)), countDown, true, true);
        loadModelWithProgress<KartModel>("start.obj", "obj/ui/", "countDown", "box", glm::scale(glm::mat4(1.0f), glm::vec3(1.0, 1.0, 1.0)), countDown, true, true);

        loadModelWithProgress<KartModel>("pause.obj", "obj/ui/", "pause", "box", glm::scale(glm::mat4(1.0f), glm::vec3(1.0, 1.0, 1.0)), pause, true, true);
         
        //빌리지 로드 맵
        loadModelWithProgress<MapModel>("village_road.obj", "asset/select_mode/", "village_road", "box", glm::scale(glm::mat4(1.0f), glm::vec3(1.0, 1.0, 1.0)), selectMaps, false, true);

        //// 빌리지 운하 맵
        glm::mat4 map2_matrix = glm::mat4(1.0f);
        map2_matrix = glm::translate(map2_matrix, glm::vec3(2.5, 0.0, 0.0));
        map2_matrix = glm::scale(map2_matrix, glm::vec3(1.0, 1.0, 1.0));
        loadModelWithProgress<MapModel>("villiage_unha.obj", "asset/select_mode/", "villiage_unha", "box", map2_matrix, selectMaps, false, true);

        // 맵선택 키보드 방향키
        glm::mat4 arrow_matrix = glm::mat4(1.0f);
        arrow_matrix = glm::translate(arrow_matrix, glm::vec3(-0.2, -1.0, 1.0));
        arrow_matrix = glm::scale(arrow_matrix, glm::vec3(0.5, 0.5, 0.5));
        arrow_matrix = glm::rotate(arrow_matrix, glm::radians(-80.0f), glm::vec3(1.0, 0.0, 0.0));
        loadModelWithProgress<MapModel>("arrow_key.obj", "asset/select_mode/", "arrow_key", "box", arrow_matrix, selectMaps, false, true);

        glm::mat4 enter_matrix = glm::mat4(1.0f);
        enter_matrix = glm::translate(enter_matrix, glm::vec3(0.6, -1.0, 1.0));
        enter_matrix = glm::scale(enter_matrix, glm::vec3(0.4, 0.3, 0.4));
        enter_matrix = glm::rotate(enter_matrix, glm::radians(-80.0f), glm::vec3(1.0, 0.0, 0.0));
        loadModelWithProgress<MapModel>("enter_key.obj", "asset/select_mode/", "enter_key", "box", enter_matrix, selectMaps, false, true);

        // 5. 동영상 및 사운드 스레드 종료 대기
        videoThread.join();
        soundThread.join();

        // 4. 프로그램 종료 플래그 설정
        isRunning = false;
        SelectMapMode* selectMapMode = new SelectMapMode();
        MM.SetMode(selectMapMode);
    }

    void mouseClick(int button, int state, int x, int y) override {}

    void keyboard(unsigned char key, int x, int y) override {}

    void specialKey(int key, int x, int y) override {}

    void specialKeyUp(int key, int x, int y) override {}

    void draw_model() override {}

    void draw_bb() override {}

    void finish() override {

    }

private:
    // 동영상 실행 함수
    void runVideo() {
        loadVideo("kartrider_intro.mp4", "./asset/select_mode/", &isRunning);
    }

    // 사운드 실행 함수
    void runSound() {
        play_sound2D("kartrider_intro.WAV", "./asset/select_mode/", false, &isRunning);
    }
};