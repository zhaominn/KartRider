#pragma once

#define STB_IMAGE_IMPLEMENTATION

#include <iostream>
#include <string>
#include <gl/glm/glm/glm.hpp>
#include <gl/glew.h>
#include <gl/stb_image.h>


/// 텍스처 로드 함수: OpenGL 텍스처를 생성하고 파일에서 데이터를 로드
/// @param path 텍스처 파일 경로
/// @return OpenGL 텍스처 ID
GLuint load_texture(const std::string& path) {
    int width, height, nrChannels; // 텍스처 이미지의 너비, 높이, 채널 수 저장 변수

    // 텍스처 로드 시 Y축을 뒤집기 (OpenGL 텍스처 좌표계와 이미지 좌표계의 차이를 맞추기 위함)
    stbi_set_flip_vertically_on_load(true);

    // 이미지 파일을 로드하여 텍스처 데이터를 읽음
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);

    // 텍스처 로드 실패 시 처리
    if (!data) {
        std::cerr << "[ERROR] Failed to load texture: " << path << std::endl; // 에러 메시지 출력
        throw std::runtime_error("Failed to load texture");                  // 예외 발생
    }

    // OpenGL 텍스처 객체 생성
    GLuint textureID;
    glGenTextures(1, &textureID);                  // 텍스처 ID 생성
    glBindTexture(GL_TEXTURE_2D, textureID);      // 생성한 텍스처를 2D 텍스처로 바인딩

    // 텍스처 래핑(Wrapping) 옵션 설정 (S축과 T축 모두 반복)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // U축 반복
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // V축 반복

    // 텍스처 필터링 옵션 설정
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // 축소 시 Mipmap + 선형 보간
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);              // 확대 시 선형 보간

    // 텍스처의 포맷 결정: 3채널이면 RGB, 4채널이면 RGBA
    GLenum format = (nrChannels == 3) ? GL_RGB : GL_RGBA;

    // 텍스처 데이터를 OpenGL로 업로드
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D); // Mipmap 생성 (다중 해상도 지원)

    // CPU 메모리에서 텍스처 데이터 해제 (GPU에 업로드가 완료되었으므로 불필요)
    stbi_image_free(data);

    // 텍스처 ID 반환
    return textureID;
}