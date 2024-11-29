#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map> // 추가: 재질별 매핑 관리

#include "Model.h"
#include "LoadTexture.h"

// MTL 파일을 읽어와서 재질 정보를 파싱하는 함수
void read_mtl_file(const std::string& filename, std::unordered_map<std::string, Material>& materials) {
    std::ifstream file("obj/" + filename);  // MTL 파일 열기
    if (!file.is_open()) {  // 파일 열기 실패 시 예외 처리
        std::cerr << "[ERROR] Failed to open MTL file: " << filename << std::endl;
        return;
    }

    std::string line, currentMaterial;
    while (std::getline(file, line)) {
        std::istringstream ss(line);
        std::string prefix;
        ss >> prefix;

        if (prefix == "newmtl") {
            // 새로운 재질 시작
            ss >> currentMaterial;
            materials[currentMaterial] = Material(); // 새 재질 추가
            materials[currentMaterial].name = currentMaterial;
        }
        else if (prefix == "Ka") {  // 환경광 색상
            ss >> materials[currentMaterial].Ka.x >> materials[currentMaterial].Ka.y >> materials[currentMaterial].Ka.z;
        }
        else if (prefix == "Kd") {  // 난반사광 색상
            ss >> materials[currentMaterial].Kd.x >> materials[currentMaterial].Kd.y >> materials[currentMaterial].Kd.z;
        }
        else if (prefix == "Ks") {  // 반사광 색상
            ss >> materials[currentMaterial].Ks.x >> materials[currentMaterial].Ks.y >> materials[currentMaterial].Ks.z;
        }
        else if (prefix == "Ns") {  // 반짝임 강도
            ss >> materials[currentMaterial].Ns;
        }
        else if (prefix == "map_Ka") {  // 환경광 텍스처 경로 (Ambient Texture)
            std::string texturePath;
            ss >> texturePath;
            Texture ambientTexture = load_texture("obj/" + texturePath, "ambient");
            if (ambientTexture.id != 0) {
                materials[currentMaterial].textures.push_back(ambientTexture);
                materials[currentMaterial].map_Ka = texturePath;
                /*std::cout << "[SUCCESS] Ambient texture loaded: " << texturePath
                    << " (ID: " << ambientTexture.id << ")" << std::endl;*/
            }
            else {
                std::cerr << "[ERROR] Failed to load Ambient texture: " << texturePath << std::endl;
            }
        }
        else if (prefix == "map_Kd") {  // 난반사광 텍스처 경로 (Diffuse Texture)
            std::string texturePath;
            ss >> texturePath;
            Texture diffuseTexture = load_texture("obj/" + texturePath, "diffuse");
            if (diffuseTexture.id != 0) {
                materials[currentMaterial].textures.push_back(diffuseTexture);
                materials[currentMaterial].map_Kd = texturePath;
               /* std::cout << "[SUCCESS] Diffuse texture loaded: " << texturePath
                    << " (ID: " << diffuseTexture.id << ")" << std::endl;*/
            }
            else {
                std::cerr << "[ERROR] Failed to load Diffuse texture: " << texturePath << std::endl;
            }
        }
        else if (prefix == "map_Ks") {  // 반사광 텍스처 경로 (Specular Texture)
            std::string texturePath;
            ss >> texturePath;

            Texture specularTexture = load_texture("obj/" + texturePath, "specular");
            if (specularTexture.id != 0) {
                materials[currentMaterial].textures.push_back(specularTexture);
                materials[currentMaterial].map_Ks = texturePath;
                /*std::cout << "[SUCCESS] Specular texture loaded: " << texturePath
                    << " (ID: " << specularTexture.id << ")" << std::endl;*/
            }
            else {
                std::cerr << "[ERROR] Failed to load Specular texture: " << texturePath << std::endl;
            }
        }
    }

    // 모든 재질에 대해 텍스처 상태 확인
    for (const auto& [name, material] : materials) {
        materials.at(name).hasTexture_s = !material.textures.empty();
    }

    file.close();  // 파일 닫기
}