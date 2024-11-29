#pragma once

#include <iostream>
#include "Model.h"

// 디버깅: Model 클래스 내부 데이터 출력 (포인터 버전)
void debug_model(const Model* model) {
    if (!model) {
        std::cerr << "Model pointer is null!" << std::endl;
        return;
    }

    std::cout << "Model Debug Info:" << std::endl;
    std::cout << "Name: " << model->name << std::endl;
    std::cout << "Type: " << model->type << std::endl;

    std::cout << "\nVertices (" << model->vertices.size() << "):" << std::endl;
    for (size_t i = 0; i < model->vertices.size(); ++i) {
        std::cout << "  [" << i << "] (" << model->vertices[i].x << ", "
            << model->vertices[i].y << ", " << model->vertices[i].z << ")" << std::endl;
    }

    std::cout << "\nTexture Coordinates (" << model->texCoords.size() << "):" << std::endl;
    for (size_t i = 0; i < model->texCoords.size(); ++i) {
        std::cout << "  [" << i << "] (" << model->texCoords[i].u << ", "
            << model->texCoords[i].v << ", " << model->texCoords[i].w << ")" << std::endl;
    }

    std::cout << "\nNormals (" << model->normals.size() << "):" << std::endl;
    for (size_t i = 0; i < model->normals.size(); ++i) {
        std::cout << "  [" << i << "] (" << model->normals[i].nx << ", "
            << model->normals[i].ny << ", " << model->normals[i].nz << ")" << std::endl;
    }

    std::cout << "\nFaces (" << model->faces.size() << "):" << std::endl;
    for (size_t i = 0; i < model->faces.size(); ++i) {
        const auto& face = model->faces[i];
        std::cout << "  Face[" << i << "] v=(" << face.v1 << ", " << face.v2 << ", " << face.v3
            << ") t=(" << face.t1 << ", " << face.t2 << ", " << face.t3
            << ") n=(" << face.n1 << ", " << face.n2 << ", " << face.n3 << ")" << std::endl;
    }
}

// 디버깅: std::unordered_map<std::string, Material> 전체 데이터 출력
void debug_materials(const std::unordered_map<std::string, Material>& materials) {
    std::cout << "\nMaterials Debug Info:" << std::endl;

    if (materials.empty()) {
        std::cout << "No materials found!" << std::endl;
        return;
    }

    // 각 Material 정보 출력
    for (const auto& [materialName, material] : materials) {
        std::cout << "\nMaterial Name: " << materialName << std::endl;

        std::cout << "  Ambient Color (Ka): (" << material.Ka.x << ", " << material.Ka.y << ", " << material.Ka.z << ")" << std::endl;
        std::cout << "  Diffuse Color (Kd): (" << material.Kd.x << ", " << material.Kd.y << ", " << material.Kd.z << ")" << std::endl;
        std::cout << "  Specular Color (Ks): (" << material.Ks.x << ", " << material.Ks.y << ", " << material.Ks.z << ")" << std::endl;
        std::cout << "  Shininess (Ns): " << material.Ns << std::endl;

        // 텍스처 정보 출력
        std::cout << "  Textures:" << std::endl;

        if (!material.map_Ka.empty()) {
            std::cout << "    Ambient Texture: " << material.map_Ka
                << " (ID: " << material.ambientTextureID << ")" << std::endl;
        }
        if (!material.map_Kd.empty()) {
            std::cout << "    Diffuse Texture: " << material.map_Kd
                << " (ID: " << material.diffuseTextureID << ")" << std::endl;
        }
        if (!material.map_Ks.empty()) {
            std::cout << "    Specular Texture: " << material.map_Ks
                << " (ID: " << material.specularTextureID << ")" << std::endl;
        }

        std::cout << "  Has Texture: " << (material.hasTexture_s ? "Yes" : "No") << std::endl;

        // 추가 디버깅: textures 벡터 내 상세 정보 출력
        if (!material.textures.empty()) {
            std::cout << "  Texture List:" << std::endl;
            for (size_t i = 0; i < material.textures.size(); ++i) {
                const auto& texture = material.textures[i];
                std::cout << "    [" << i << "] Type: " << texture.type
                    << ", Path: " << texture.path
                    << ", ID: " << texture.id << std::endl;
            }
        }
        else {
            std::cout << "    No textures associated with this material." << std::endl;
        }
    }

    std::cout << "\nTotal Materials: " << materials.size() << std::endl;
}
