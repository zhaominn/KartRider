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
Texture load_texture(const std::string& path, const std::string& type) {
    Texture texture;
    texture.type = type;
    texture.path = path;

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);

    if (!data) {
        std::cerr << "[ERROR] Failed to load texture: " << path << std::endl;
        throw std::runtime_error("Failed to load texture");
    }

    // OpenGL 텍스처 ID 생성
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // 텍스처 데이터 업로드 및 설정
    GLenum format = (nrChannels == 3) ? GL_RGB : GL_RGBA;
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    // 텍스처 필터 및 래핑 옵션 설정
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // CPU 메모리에서 텍스처 데이터 해제
    stbi_image_free(data);

    // 텍스처 ID를 Texture 구조체에 저장
    texture.id = textureID;

    //std::cout << "[INFO] Texture loaded: " << path << " (ID: " << textureID << ")" << std::endl;

    return texture;  // Texture 구조체 반환
}