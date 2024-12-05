#pragma once
#include <iostream>
#include <opencv2/opencv.hpp>

void loadVideo(std::string fileName, std::string path) {
    // 1. OpenCV 버전 출력
    std::cout << "OpenCV Version: " << CV_VERSION << std::endl;

    // 2. 동영상 파일 열기
    cv::VideoCapture cap(path + fileName);

    if (!cap.isOpened()) {
        std::cerr << "Error: Could not open video file." << std::endl;
        return;
    }

    // 3. 동영상 재생 루프
    cv::Mat frame;
    while (true) {
        cap >> frame; // 프레임 읽기
        if (frame.empty()) {
            break; // 동영상 끝
        }

        // 4. 프레임 화면에 출력
        cv::imshow("Video Playback", frame);

        // 5. ESC 키를 누르면 종료
        if (cv::waitKey(30) == 27) {
            break;
        }
    }

    cap.release(); // 자원 해제
    cv::destroyAllWindows(); // 모든 창 닫기
    return;
}