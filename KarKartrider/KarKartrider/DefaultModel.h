#pragma once

#include <vector>
#include <string>
#include <gl/glew.h>
#include <gl/glm/glm/gtc/type_ptr.hpp>
#include <gl/glm/glm/gtc/matrix_transform.hpp>
#include <unordered_map>

#include "Model.h"
#include "LoadObj.h"
#include "BulletPhysics.h"

using namespace std;

// std::tuple의 커스텀 해시 함수 정의
struct TupleHash {
    template <typename T>
    static void hashCombine(std::size_t& seed, const T& value) {
        std::hash<T> hasher;
        seed ^= hasher(value) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }

    template <typename... Types>
    std::size_t operator()(const std::tuple<Types...>& tuple) const {
        std::size_t seed = 0;
        std::apply([&seed](const auto&... args) { ((hashCombine(seed, args)), ...); }, tuple);
        return seed;
    }
};

class DefaultModel : public Model {
public:
    DefaultModel(){}

    DefaultModel(string name, string obj_name, string obj_type, glm::mat4 start_matrix = glm::mat4(1.0f)) {
        read_obj_file(name, this, obj_name, obj_type);
        this->matrix = start_matrix * this->matrix;
    }

    ~DefaultModel(){}

    void load_obj(string name, string obj_name, string obj_type, glm::mat4 start_matrix = glm::mat4(1.0f)) override {
        read_obj_file(name, this, obj_name, obj_type);
        this->matrix = start_matrix * this->matrix;
    }

    const void draw(GLint shaderProgramID, bool (*isKeyPressed_s)(const char&)) override {
        GLint modelStatus = glGetUniformLocation(shaderProgramID, "modelStatus");
        GLint modelLoc = glGetUniformLocation(shaderProgramID, "model");
        GLint normalLoc = glGetUniformLocation(shaderProgramID, "normalMatrix");

        if (this->model_status) {
            // VAO 바인딩
            glBindVertexArray(this->vao);
            glLineWidth(1.0f);

            // 텍스처가 있는 경우
            if (this->material.hasTexture) {
                // Environment Texture (map_Ka)
                if (material.ambientTextureID != 0) {
                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, material.ambientTextureID);
                    glUniform1i(glGetUniformLocation(shaderProgramID, "map_Ka"), 0);
                    glUniform1i(glGetUniformLocation(shaderProgramID, "hasKaTexture"), 1);
                }
                else {
                    glUniform1i(glGetUniformLocation(shaderProgramID, "hasKaTexture"), 0);
                }

                // Diffuse Texture (map_Kd)
                if (material.diffuseTextureID != 0) {
                    glActiveTexture(GL_TEXTURE1);
                    glBindTexture(GL_TEXTURE_2D, material.diffuseTextureID);
                    glUniform1i(glGetUniformLocation(shaderProgramID, "map_Kd"), 1);
                    glUniform1i(glGetUniformLocation(shaderProgramID, "hasKdTexture"), 1);
                }
                else {
                    glUniform1i(glGetUniformLocation(shaderProgramID, "hasKdTexture"), 0);
                }

                // Specular Texture (map_Ks)
                if (material.specularTextureID != 0) {
                    glActiveTexture(GL_TEXTURE2);
                    glBindTexture(GL_TEXTURE_2D, material.specularTextureID);
                    glUniform1i(glGetUniformLocation(shaderProgramID, "map_Ks"), 2);
                    glUniform1i(glGetUniformLocation(shaderProgramID, "hasKsTexture"), 1);
                }
                else {
                    glUniform1i(glGetUniformLocation(shaderProgramID, "hasKsTexture"), 0);
                }
            }
            // 텍스처가 없는 경우 기본 재질 속성만 사용
            else {
                glUniform1i(glGetUniformLocation(shaderProgramID, "hasTexture"), 0);

                // 기본 재질 속성 전달
                GLint KaLoc = glGetUniformLocation(shaderProgramID, "Ka");
                GLint KdLoc = glGetUniformLocation(shaderProgramID, "Kd");
                GLint KsLoc = glGetUniformLocation(shaderProgramID, "Ks");
                GLint NsLoc = glGetUniformLocation(shaderProgramID, "Ns");

                glUniform3fv(KaLoc, 1, glm::value_ptr(this->material.Ka));
                glUniform3fv(KdLoc, 1, glm::value_ptr(this->material.Kd));
                glUniform3fv(KsLoc, 1, glm::value_ptr(this->material.Ks));
                glUniform1f(NsLoc, this->material.Ns);
            }

            // 모델 매트릭스와 법선 매트릭스 전달
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(this->matrix));
            glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(this->matrix)));
            glUniformMatrix3fv(normalLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

            glUniform1i(modelStatus, 0);

            // 렌더링 모드 (1번 키로 와이어프레임 전환)
            if (isKeyPressed_s('1')) {
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            }
            else {
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }

            // 삼각형 렌더링
            glDrawElements(GL_TRIANGLES, this->faces.size() * 3, GL_UNSIGNED_INT, 0);

            // OpenGL 상태 초기화
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // 렌더링 모드 초기화

            // 텍스처 상태 초기화
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, 0);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, 0);
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, 0);

            // VAO 바인딩 해제
            
            
            
            (0);
        }
    }


    const void draw_rigidBody(GLuint shaderProgramID) override {
        if (this->rigidBody) {
            RenderCollisionBox(this, shaderProgramID); // 충돌 박스 그리기
        }
    }

    void initBuffer() override {
        // Step 1: VAO 생성 및 바인딩
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        // Step 2: 복합 인덱스를 단일화된 정점 데이터로 변환
        std::unordered_map<std::tuple<unsigned int, unsigned int, unsigned int>, unsigned int, TupleHash> uniqueVertices;
        std::vector<glm::vec3> vertexBuffer;   // 위치 데이터
        std::vector<glm::vec3> normalBuffer;   // 법선 데이터
        std::vector<glm::vec2> texCoordBuffer; // 텍스처 좌표 데이터
        std::vector<unsigned int> indices;    // 단일화된 인덱스 배열

        for (const Face& face : this->faces) {
            for (int i = 0; i < 3; i++) { // 삼각형의 세 정점 처리
                // 복합 인덱스 생성
                unsigned int vertexIndex = (i == 0) ? face.v1 : (i == 1) ? face.v2 : face.v3;
                unsigned int texCoordIndex = (i == 0) ? face.t1 : (i == 1) ? face.t2 : face.t3;
                unsigned int normalIndex = (i == 0) ? face.n1 : (i == 1) ? face.n2 : face.n3;
                std::tuple<unsigned int, unsigned int, unsigned int> key = std::make_tuple(vertexIndex, texCoordIndex, normalIndex);

                // 고유한 정점인지 확인
                if (uniqueVertices.count(key) == 0) {
                    // 새로운 정점 추가
                    uniqueVertices[key] = static_cast<unsigned int>(vertexBuffer.size());

                    // 위치 추가
                    vertexBuffer.push_back(glm::vec3(
                        this->vertices[vertexIndex].x,
                        this->vertices[vertexIndex].y,
                        this->vertices[vertexIndex].z
                    ));

                    // 텍스처 좌표 추가 (존재하면)
                    if (texCoordIndex != static_cast<unsigned int>(-1)) {
                        texCoordBuffer.push_back(glm::vec2(
                            this->texCoords[texCoordIndex].u,
                            this->texCoords[texCoordIndex].v
                        ));
                    }
                    else {
                        texCoordBuffer.push_back(glm::vec2(0.0f, 0.0f));
                    }

                    // 법선 추가
                    normalBuffer.push_back(glm::vec3(
                        this->normals[normalIndex].nx,
                        this->normals[normalIndex].ny,
                        this->normals[normalIndex].nz
                    ));
                }

                // 인덱스 추가
                indices.push_back(uniqueVertices[key]);
            }
        }

        // Step 3: VBO 설정 - 위치 데이터
        glGenBuffers(1, &vbos[0]);
        glBindBuffer(GL_ARRAY_BUFFER, vbos[0]);
        glBufferData(GL_ARRAY_BUFFER, vertexBuffer.size() * sizeof(glm::vec3), vertexBuffer.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
        glEnableVertexAttribArray(0);

        // Step 4: VBO 설정 - 법선 데이터
        glGenBuffers(1, &vbos[1]);
        glBindBuffer(GL_ARRAY_BUFFER, vbos[1]);
        glBufferData(GL_ARRAY_BUFFER, normalBuffer.size() * sizeof(glm::vec3), normalBuffer.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
        glEnableVertexAttribArray(1);

        // Step 5: VBO 설정 - 텍스처 좌표 데이터
        glGenBuffers(1, &vbos[2]);
        glBindBuffer(GL_ARRAY_BUFFER, vbos[2]);
        glBufferData(GL_ARRAY_BUFFER, texCoordBuffer.size() * sizeof(glm::vec2), texCoordBuffer.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
        glEnableVertexAttribArray(2);

        // Step 6: EBO 설정 - 인덱스 데이터
        glGenBuffers(1, &vbos[3]);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbos[3]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        // Step 7: VAO 바인딩 해제
        glBindVertexArray(0);
    }
};