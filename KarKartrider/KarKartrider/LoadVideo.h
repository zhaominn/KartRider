#pragma once
#include <iostream>
#include <opencv2/opencv.hpp>

void loadVideo(const std::string& fileName, const std::string& path, bool* isRunning) {
    // 1. OpenCV 버전 출력
    std::cout << "OpenCV Version: " << CV_VERSION << std::endl;

    // 2. 동영상 파일 열기
    cv::VideoCapture cap(path + fileName);

    if (!cap.isOpened()) {
        std::cerr << "Error: Could not open video file." << std::endl;
        return;
    }

    // 4. 윈도우 생성 (창 이름 지정)
    cv::namedWindow("Kartrider Intro", cv::WINDOW_NORMAL);

    // 5. 창 위치 및 크기 설정
    cv::moveWindow("Kartrider Intro", location_x, location_y);
    cv::resizeWindow("Kartrider Intro", window_width, window_height);

    // 6. 동영상 재생 루프
    cv::Mat frame;
    while (isRunning) {
        cap >> frame; // 프레임 읽기
        if (frame.empty()) {
            break; // 동영상 끝
        }

        // 창 상태 확인: 창이 닫히면 루프 종료
        if (cv::getWindowProperty("Kartrider Intro", cv::WND_PROP_VISIBLE) < 1) {
            break;
        }

        // 프레임 화면에 출력
        cv::imshow("Kartrider Intro", frame);

        // ESC 키를 누르면 종료
        if (cv::waitKey(30) == 27) {
            break;
        }
    }

    // 7. 자원 해제
    cap.release();
    cv::destroyAllWindows();
}