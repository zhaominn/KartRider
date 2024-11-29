#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "Model.h"
#include "LoadTexture.h"

// MTL 파일을 읽어와서 재질 정보를 파싱하는 함수
void read_mtl_file(const std::string& filename, Material& material) {
    std::ifstream file("obj/" + filename);  // MTL 파일 열기
    if (!file.is_open()) {  // 파일 열기 실패 시 예외 처리
        //throw std::runtime_error("Error opening MTL file: " + filename);
        cout << "mtl file open error" << endl;
        return;
    }

    std::string line;
    bool hasTexture = false;

    while (std::getline(file, line)) {
        std::istringstream ss(line);
        std::string prefix;
        ss >> prefix;

        if (prefix == "Ka") {  // 환경광 색상
            ss >> material.Ka.x >> material.Ka.y >> material.Ka.z;
        }
        else if (prefix == "Kd") {  // 난반사광 색상
            ss >> material.Kd.x >> material.Kd.y >> material.Kd.z;
        }
        else if (prefix == "Ks") {  // 반사광 색상
            ss >> material.Ks.x >> material.Ks.y >> material.Ks.z;
        }
        else if (prefix == "Ns") {  // 반짝임 강도
            ss >> material.Ns;
        }
        else if (prefix == "map_Ka") {  // 환경광 텍스처 경로
            ss >> material.map_Ka;
            material.ambientTextureID = load_texture("obj/" + material.map_Ka);
            if (material.ambientTextureID != 0) {
                std::cout << "Ambient texture loaded: " << material.map_Ka << std::endl;
            }
            else {
                std::cerr << "Failed to load ambient texture: " << material.map_Ka << std::endl;
            }
            hasTexture = true;
        }
        else if (prefix == "map_Kd") {  // 난반사광 텍스처 경로
            ss >> material.map_Kd;
            material.diffuseTextureID = load_texture("obj/" + material.map_Kd);
            if (material.diffuseTextureID != 0) {
                std::cout << "Diffuse texture loaded: " << material.map_Kd << std::endl;
            }
            else {
                std::cerr << "Failed to load diffuse texture: " << material.map_Kd << std::endl;
            }
            hasTexture = true;
        }
        else if (prefix == "map_Ks") {  // 반사광 텍스처 경로
            ss >> material.map_Ks;
            material.specularTextureID = load_texture("obj/" + material.map_Ks);
            if (material.specularTextureID != 0) {
                std::cout << "Specular texture loaded: " << material.map_Ks << std::endl;
            }
            else {
                std::cerr << "Failed to load specular texture: " << material.map_Ks << std::endl;
            }
            hasTexture = true;
        }
    }

    material.hasTexture = hasTexture;  // 텍스처가 있는지 여부 플래그 설정
    file.close();  // 파일 닫기
}