#pragma once
#include <iostream>
#include <irrKlang.h>
#include <thread>
#include <chrono>

using namespace irrklang;

void play_sound2D(const std::string& name, const std::string& path, bool repeat, bool* isRunning) {
    ISoundEngine* engine = createIrrKlangDevice();
    if (!engine) {
        std::cerr << "Error: Could not initialize irrKlang engine!" << std::endl;
        return;
    }

    // 경로 디버깅 출력
    std::string fullPath = path + name;
    std::cout << "Attempting to play sound: " << fullPath << std::endl;

    // 사운드 재생
    ISound* sound = engine->play2D(fullPath.c_str(), repeat, false, true);
    if (!sound) {
        std::cerr << "Error: Could not play sound file!" << std::endl;
        engine->drop();
        return;
    }

    // 볼륨 설정 (기본 1.0, 필요하면 외부에서 수정)
    sound->setVolume(1.0f);

    // 플래그가 true인 동안 사운드 재생 유지
    while (*isRunning) {
        if (!repeat && sound->isFinished()) break; // 반복하지 않으면 사운드 종료 확인
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // 100ms마다 상태 체크
    }

    // 종료: 사운드 정지 및 자원 해제
    if (!*isRunning) {
        sound->stop(); // 강제 정지
    }
    sound->drop(); // 사운드 객체 해제
    engine->drop(); // 엔진 종료
}
