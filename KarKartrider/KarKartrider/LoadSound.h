#pragma once
#include <iostream>
#include <irrKlang.h>
using namespace irrklang;

void play_sound2D(std::string path, std::string name, bool repeat) {
    // irrKlang 엔진 초기화
    ISoundEngine* engine = createIrrKlangDevice();
    if (!engine) {
        std::cerr << "Error: Could not initialize irrKlang engine!" << std::endl;
        return;
    }

    // std::string을 const char*로 변환
    std::string fullPath = path + name;
    engine->play2D(fullPath.c_str(), repeat); // c_str() 사용

    // 종료 대기
    std::cout << "Press Enter to quit..." << std::endl;
    std::cin.get();

    // 엔진 종료
    engine->drop();
}