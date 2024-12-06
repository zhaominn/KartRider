#pragma once
#include <iostream>
#include <opencv2/opencv.hpp>
#include "root.h"

void loadImage(const std::string& fileName, const std::string& path) {
    // 1. OpenCV 버전 출력
    std::cout << "OpenCV Version: " << CV_VERSION << std::endl;

    // 2. 이미지 파일 열기
    cv::Mat image = cv::imread(path + fileName);

    if (image.empty()) {
        std::cerr << "Error: Could not open image file." << std::endl;
        return;
    }

    // 3. 윈도우 생성 (창 이름 지정)
    cv::namedWindow("Kartrider Image", cv::WINDOW_NORMAL);

    // 4. 창 위치 및 크기 설정
    cv::moveWindow("Kartrider Image", location_x, location_y);
    cv::resizeWindow("Kartrider Image", window_width, window_height);

    // 5. 이미지 출력 루프
    while (true) {
        // 창 상태 확인: 창이 닫히면 루프 종료
        if (cv::getWindowProperty("Kartrider Image", cv::WND_PROP_VISIBLE) < 1) {
            break;
        }

        // 이미지 화면에 출력
        cv::imshow("Kartrider Image", image);

        // ESC 키를 누르면 종료
        if (cv::waitKey(30) == 27) {
            break;
        }
    }

    // 6. 윈도우 닫기
    cv::destroyAllWindows();
}