#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map> // 추가: 재질별 매핑 관리

#include "Model.h"
#include "LoadMtl.h"

// OBJ 파일을 읽어와서 모델 데이터를 파싱하는 함수
void read_obj_file(const std::string& filename, const string path, Model* model, const std::string& name, const std::string& type) {
    std::ifstream file(path + filename);  // 파일 읽기 모드로 열기
    if (!file.is_open()) {  // 파일을 열지 못한 경우
        std::cerr << "Error opening file: " << filename << std::endl;
        return;
    }

    std::string line;
    std::string mtlFilename;  // MTL 파일 이름
    std::string currentMaterial;  // 현재 사용 중인 재질 이름
    std::unordered_map<std::string, Material> materials;  // 재질별 데이터를 저장할 매핑 구조

    // 파일을 한 줄씩 읽어가며 정점, 텍스처 좌표, 법선 벡터, 면 데이터를 처리하는 루프
    while (std::getline(file, line)) {
        std::istringstream ss(line);
        std::string prefix;
        ss >> prefix;

        if (prefix == "v") {  // 정점 데이터를 읽을 때
            Vertex vertex;
            ss >> vertex.x >> vertex.y >> vertex.z;
            model->vertices.push_back(vertex);
        }
        else if (prefix == "vt") {  // 텍스처 좌표 데이터를 읽을 때
            TextureCoord texCoord;

            ss >> texCoord.u >> texCoord.v;

            // 만약 w 값이 있으면 추가로 읽음
            if (!(ss >> texCoord.w)) {
                texCoord.w = 0.0f; // 기본값을 0.0으로 설정
            }

            model->texCoords.push_back(texCoord);
        }
        else if (prefix == "vn") {  // 법선 벡터 데이터를 읽을 때
            Normal normal;
            ss >> normal.nx >> normal.ny >> normal.nz;
            model->normals.push_back(normal);
        }
        else if (prefix == "usemtl") {  // 재질 지정
            ss >> currentMaterial;  // 현재 재질 이름 업데이트
        }
        else if (prefix == "f") {  // Face 데이터를 읽을 때
            std::vector<unsigned int> vertexIndices, texCoordIndices, normalIndices;
            unsigned int v, t = 0, n;
            char slash;

            // Face 데이터를 처리
            while (ss >> v) {
                vertexIndices.push_back(v - 1); // OBJ 인덱스는 1부터 시작하므로 -1
                if (ss.peek() == '/') {
                    ss >> slash;
                    if (ss.peek() != '/') {
                        ss >> t;
                        texCoordIndices.push_back(t - 1); // 텍스처 인덱스
                    }
                    else {
                        texCoordIndices.push_back(-1);  // 텍스처가 없을 경우 -1
                    }
                    if (ss.peek() == '/') {
                        ss >> slash >> n;
                        normalIndices.push_back(n - 1); // 법선 인덱스
                    }
                }
            }

            // 삼각형 Face 처리
            if (vertexIndices.size() == 3) {
                Face face;

                // 정점 처리
                face.v1 = vertexIndices[0];
                face.v2 = vertexIndices[1];
                face.v3 = vertexIndices[2];

                // 텍스처 처리
                face.t1 = (texCoordIndices.size() > 0 && texCoordIndices[0] != -1) ? texCoordIndices[0] : 0;
                face.t2 = (texCoordIndices.size() > 1 && texCoordIndices[1] != -1) ? texCoordIndices[1] : 0;
                face.t3 = (texCoordIndices.size() > 2 && texCoordIndices[2] != -1) ? texCoordIndices[2] : 0;

                // 법선 처리
                face.n1 = normalIndices.size() > 0 ? normalIndices[0] : 0;
                face.n2 = normalIndices.size() > 1 ? normalIndices[1] : 0;
                face.n3 = normalIndices.size() > 2 ? normalIndices[2] : 0;

                face.materialName = currentMaterial;  // 현재 재질 이름 저장
                // Face와 normalFaces 추가
                model->faces.push_back(face);
            }
            // 사각형 Face 처리 (두 개의 삼각형으로 분할)
            else if (vertexIndices.size() == 4) {
                // 첫 번째 삼각형 (v1, v2, v3)
                Face face1;

                face1.v1 = vertexIndices[0];
                face1.v2 = vertexIndices[1];
                face1.v3 = vertexIndices[2];

                face1.t1 = (texCoordIndices.size() > 0 && texCoordIndices[0] != -1) ? texCoordIndices[0] : 0;
                face1.t2 = (texCoordIndices.size() > 1 && texCoordIndices[1] != -1) ? texCoordIndices[1] : 0;
                face1.t3 = (texCoordIndices.size() > 2 && texCoordIndices[2] != -1) ? texCoordIndices[2] : 0;

                face1.n1 = normalIndices.size() > 0 ? normalIndices[0] : 0;
                face1.n2 = normalIndices.size() > 1 ? normalIndices[1] : 0;
                face1.n3 = normalIndices.size() > 2 ? normalIndices[2] : 0;

                face1.materialName = currentMaterial;  // 현재 재질 이름 저장
                model->faces.push_back(face1);

                // 두 번째 삼각형 (v1, v3, v4)
                Face face2;

                face2.v1 = vertexIndices[0];
                face2.v2 = vertexIndices[2];
                face2.v3 = vertexIndices[3];

                face2.t1 = (texCoordIndices.size() > 0 && texCoordIndices[0] != -1) ? texCoordIndices[0] : 0;
                face2.t2 = (texCoordIndices.size() > 2 && texCoordIndices[2] != -1) ? texCoordIndices[2] : 0;
                face2.t3 = (texCoordIndices.size() > 3 && texCoordIndices[3] != -1) ? texCoordIndices[3] : 0;

                face2.n1 = normalIndices.size() > 0 ? normalIndices[0] : 0;
                face2.n2 = normalIndices.size() > 2 ? normalIndices[2] : 0;
                face2.n3 = normalIndices.size() > 3 ? normalIndices[3] : 0;

                face2.materialName = currentMaterial;  // 현재 재질 이름 저장
                model->faces.push_back(face2);
            }
        }
        else if (prefix == "mtllib") {  // MTL 파일 참조
            ss >> mtlFilename;
        }
    }

    // 모델 이름 및 타입 설정
    model->name = name;
    model->type = type;

    // MTL 파일 처리
    if (!mtlFilename.empty()) {
        read_mtl_file(mtlFilename, path, materials);  // MTL 파일 로드
        model->materials = materials;          // 로드한 재질 정보를 모델에 저장
    }

    file.close();  // 파일 닫기
}
