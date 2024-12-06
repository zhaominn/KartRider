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

    // 3. 윈도우 생성 (창 이름 지정)
    cv::namedWindow("KarKartrider intro", cv::WINDOW_NORMAL); // `cv::WINDOW_NORMAL`을 사용하면 크기 변경 가능

    cv::moveWindow("KarKartrider intro", location_x, location_y);
    cv::resizeWindow("KarKartrider intro", window_width, window_height);

    // 5. 동영상 재생 루프
    cv::Mat frame;
    while (true) {
        cap >> frame; // 프레임 읽기
        if (frame.empty()) {
            break; // 동영상 끝
        }

        // 6. 창 상태 확인
        if (cv::getWindowProperty("KarKartrider intro", cv::WND_PROP_VISIBLE) < 1) {
            break; // 창이 닫혔으면 루프 종료
        }

        // 7. 프레임 화면에 출력
        cv::imshow("KarKartrider intro", frame);

        // 8. ESC 키를 누르면 종료
        if (cv::waitKey(30) == 27) {
            break;
        }
    }

    cap.release(); // 자원 해제
    cv::destroyAllWindows(); // 모든 창 닫기
    return;
}