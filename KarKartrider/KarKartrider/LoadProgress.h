#pragma once
#include<iostream>
#include <gl/glm/glm/glm.hpp>

#include "DefaultModel.h"
#include "Model.h"

void printProgressBar(int progress, int total) {
    const int barWidth = 50; // 로딩바 너비
    float percentage = static_cast<float>(progress) / total;

    std::cout << "\r[";
    int pos = static_cast<int>(barWidth * percentage);
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) std::cout << "=";
        else if (i == pos) std::cout << ">";
        else std::cout << " ";
    }
    std::cout << "] " << static_cast<int>(percentage * 100.0) << "%";
    std::cout.flush();
}

template <typename T, typename = std::enable_if<std::is_base_of<Model, T>::value>>
void loadModelWithProgress(const std::string& modelPath, const std::string path, const std::string& modelName, const std::string& modelType, glm::mat4 scale, std::vector<Model*>& models) {
    std::cout << "Loading model: " << modelName << std::endl;

    const int totalSteps = 4; // 총 작업 단계 수
    int currentStep = 0;

    // Step 1: OBJ 파일 로드
    std::cout << "Step 1/4: Reading OBJ file..." << std::endl;
    T* model = new T(modelPath, path, modelName, modelType, scale);  // 템플릿으로 모델 생성
    currentStep++;
    printProgressBar(currentStep, totalSteps);

    // Step 2: MTL 파일 처리
    std::cout << "\nStep 2/4: Processing Materials (MTL file)..." << std::endl;
    for (const auto& [materialName, material] : model->materials) {
        std::cout << "Processing material: " << materialName << std::endl;
        if (material.hasTexture()) {
            std::cout << "  Material has textures: " << material.textures.size() << std::endl;
        }
        else {
            std::cout << "  No textures found for material: " << materialName << std::endl;
        }
    }
    currentStep++;
    printProgressBar(currentStep, totalSteps);

    // Step 3: 텍스처 로드
    std::cout << "\nStep 3/4: Loading textures..." << std::endl;
    for (const auto& [materialName, material] : model->materials) {
        for (const auto& texture : material.textures) {
            std::cout << "Loading texture: " << texture.path << std::endl;
            if (texture.id != 0) {
                std::cout << "  Texture loaded successfully (ID: " << texture.id << ")" << std::endl;
            }
            else {
                std::cerr << "  Failed to load texture: " << texture.path << std::endl;
            }
        }
    }
    currentStep++;
    printProgressBar(currentStep, totalSteps);

    // Step 4: 버퍼 초기화
    std::cout << "\nStep 4/4: Initializing buffers..." << std::endl;
    model->initBuffer();
    currentStep++;
    printProgressBar(currentStep, totalSteps);

    // 모델 로드 완료
    std::cout << "\nModel loaded successfully!" << std::endl;
    models.push_back(model);
}
