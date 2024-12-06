#pragma once
#include <iostream>
#include <irrKlang.h>
using namespace irrklang;

void play_sound2D(std::string path, std::string name, bool repeat, bool* isRunning) {
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

    // 플래그가 true인 동안 사운드 재생 유지
    while (*isRunning) {
        if (!repeat && sound->isFinished()) break; // 반복하지 않으면 종료
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    sound->drop(); // 사운드 객체 해제
    engine->drop(); // 엔진 종료
}