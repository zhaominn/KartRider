#pragma once

#include <limits>  // std::numeric_limits 사용을 위한 헤더 추가
#include <gl/glm/glm/glm.hpp>
#include <gl/glm/glm/gtx/matrix_decompose.hpp>

#include "CustomContactResultCallback.h"

#include"include/btBulletCollisionCommon.h"
#include"include/btBulletDynamicsCommon.h"

#include "Model.h"

void removeRigidBodyFromModel(Model& model);

// Bullet Physics 관련 변수들
btBroadphaseInterface* broadphase;
btDefaultCollisionConfiguration* collisionConfiguration;
btCollisionDispatcher* dispatcher;
btSequentialImpulseConstraintSolver* solver;
btDiscreteDynamicsWorld* dynamicsWorld;

// Bullet Physics 초기화 함수
void initPhysics() {
    broadphase = new btDbvtBroadphase();
    collisionConfiguration = new btDefaultCollisionConfiguration();
    dispatcher = new btCollisionDispatcher(collisionConfiguration);
    solver = new btSequentialImpulseConstraintSolver();
    dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
    dynamicsWorld->setGravity(btVector3(0, -9.81f, 0));
}

// Model의 AABB를 계산하여 크기를 반환하는 함수
glm::vec3 calculateModelSize(const Model* model) {
    // 초기 최소, 최대값을 설정
    glm::vec3 min(std::numeric_limits<float>::max());
    glm::vec3 max(std::numeric_limits<float>::lowest());

    // 모든 정점 순회
    for (const Vertex& vertex : model->vertices) {
        // 정점 위치를 월드 공간으로 변환
        glm::vec4 transformedVertex = model->matrix * glm::vec4(vertex.x, vertex.y, vertex.z, 1.0f);

        // 최소값과 최대값 갱신
        min.x = std::min(min.x, transformedVertex.x);
        min.y = std::min(min.y, transformedVertex.y);
        min.z = std::min(min.z, transformedVertex.z);

        max.x = std::max(max.x, transformedVertex.x);
        max.y = std::max(max.y, transformedVertex.y);
        max.z = std::max(max.z, transformedVertex.z);
    }

    // 폭, 높이, 깊이를 계산
    return max - min;
}

// 모델에 대한 충돌 객체와 강체 생성 및 물리 세계에 추가
void addModelToPhysicsWorld(Model*& model) {
    // 모델의 크기 계산 (AABB)
    glm::vec3 size = calculateModelSize(model);
    btCollisionShape* shape = nullptr;
    if (model->type == "box") {
        // 각 모델에 독립적인 충돌 박스 생성
        shape = new btBoxShape(btVector3(size.x * 0.5f, size.y * 0.5f, size.z * 0.5f));
    }
    else if (model->type == "sphere") {
        float radius = std::max(std::max(size.x, size.y), size.z) * 0.5f;
        shape = new btSphereShape(radius);
    }
    else if (model->type == "cylinder") {
        shape = new btCylinderShape(btVector3(size.x * 0.5f, size.y * 0.5f, size.z * 0.5f));
    }
    else if (model->type == "capsule") {
        float radius = std::min(size.x, size.z) * 0.5f;
        float height = size.y - 2 * radius;
        shape = new btCapsuleShape(radius, height);
    }
    else if (model->type == "cone") {
        float radius = size.x * 0.5f;
        float height = size.y;
        shape = new btConeShape(radius, height);
    }
    else if (model->type == "plane") {
        float radius = size.x * 0.5f;
        float height = size.y;
        shape = new btConeShape(radius, height);
    }
    else {
        shape = new btBoxShape(btVector3(size.x * 0.5f, 0.01f, size.z * 0.5f));
    }

    if (!shape) {
        std::cerr << "Failed to create collision shape for model: " << model->name << std::endl;
        return;
    }

    // OpenGL의 modelMatrix에서 위치, 회전, 스케일 추출
    glm::vec3 translation, scale, skew;
    glm::vec4 perspective;
    glm::quat rotation;
    glm::decompose(model->matrix, scale, rotation, translation, skew, perspective);

    // Bullet Physics에서 사용하는 btTransform으로 변환
    btTransform startTransform;
    startTransform.setIdentity();
    startTransform.setOrigin(btVector3(translation.x, translation.y, translation.z));

    // 질량 및 관성 설정
    btScalar mass = 1.0f;
    btVector3 localInertia(0, 0, 0);
    if (mass != 0.0f) {
        shape->calculateLocalInertia(mass, localInertia);
    }

    // RigidBody 생성 및 물리 세계에 추가
    btDefaultMotionState* motionState = new btDefaultMotionState(startTransform);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, shape, localInertia);
    btRigidBody* body = new btRigidBody(rbInfo);

    dynamicsWorld->addRigidBody(body);
    model->rigidBody = body;
}


// 모든 모델에 대한 물리 세계 충돌 객체 초기화
void initializeModelsWithPhysics(std::vector<Model*>& models) {
    for (auto& model : models) {
        addModelToPhysicsWorld(model);
    }
}

void cleanupPhysics() {
    for (int i = dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--) {
        btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[i];
        btRigidBody* body = btRigidBody::upcast(obj);
        if (body && body->getMotionState()) {
            delete body->getMotionState();
        }
        dynamicsWorld->removeCollisionObject(obj);
        delete obj;
    }

    delete dynamicsWorld;
    delete solver;
    delete dispatcher;
    delete collisionConfiguration;
    delete broadphase;
}

// 특정 모델을 삭제하는 함수
void removeModelFromWorld(std::vector<Model*>& models, Model*& modelToDelete) {
    // 물리 세계에서 모델의 RigidBody 제거
    if (modelToDelete->rigidBody) {
        dynamicsWorld->removeRigidBody(modelToDelete->rigidBody);

        // RigidBody에 할당된 리소스 해제
        delete modelToDelete->rigidBody->getMotionState();
        delete modelToDelete->rigidBody;
        modelToDelete->rigidBody = nullptr;
    }

    // models 벡터에서 해당 모델 제거
    auto it = std::find_if(models.begin(), models.end(), [&](const Model* model) {
        return *&model == *&modelToDelete;
        });

    if (it != models.end()) {
        models.erase(it);
    }
}

// 특정 모델의 RigidBody만 제거하는 함수
void removeRigidBodyFromModel(Model*& model) {
    if (model->rigidBody) {
        // 물리 세계에서 RigidBody 제거
        dynamicsWorld->removeRigidBody(model->rigidBody);

        // RigidBody에 할당된 리소스 해제
        delete model->rigidBody->getMotionState();
        delete model->rigidBody;
        model->rigidBody = nullptr;
    }
}

void UpdateRigidBodyTransforms(std::vector<Model*>models) {

    for (auto& model : models) {

        if (!model->rigidBody) return;

        glm::mat4 modelMatrix = model->matrix;
        btTransform transform;
        transform.setFromOpenGLMatrix(glm::value_ptr(modelMatrix));

        // 디버깅 로그
        std::cout << "Updating RigidBody for Model: " << model->name
            << " | OpenGL Position: (" << modelMatrix[3][0] << ", "
            << modelMatrix[3][1] << ", " << modelMatrix[3][2] << ")"
            << std::endl;

        model->rigidBody->setWorldTransform(transform);
    }

}

void UpdateRigidBodyTransform(Model*& model) {
    if (!model->rigidBody) return;

    glm::mat4 modelMatrix = model->matrix;
    btTransform transform;
    transform.setFromOpenGLMatrix(glm::value_ptr(modelMatrix));

    // 디버깅 로그
    std::cout << "Updating RigidBody for Model: " << model->name
        << " | OpenGL Position: (" << modelMatrix[3][0] << ", "
        << modelMatrix[3][1] << ", " << modelMatrix[3][2] << ")"
        << std::endl;

    model->rigidBody->setWorldTransform(transform);
}

void RenderCollisionBox(const Model* model, GLuint shaderProgram) {
    if (!model->rigidBody) return; // 물리 객체가 없으면 건너뜀

    // AABB 계산
    btVector3 aabbMin, aabbMax;
    model->rigidBody->getCollisionShape()->getAabb(model->rigidBody->getWorldTransform(), aabbMin, aabbMax);

    glm::vec3 min = glm::vec3(aabbMin.getX(), aabbMin.getY(), aabbMin.getZ());
    glm::vec3 max = glm::vec3(aabbMax.getX(), aabbMax.getY(), aabbMax.getZ());

    glm::vec3 corners[8] = {
        glm::vec3(min.x, min.y, min.z),
        glm::vec3(max.x, min.y, min.z),
        glm::vec3(max.x, max.y, min.z),
        glm::vec3(min.x, max.y, min.z),
        glm::vec3(min.x, min.y, max.z),
        glm::vec3(max.x, min.y, max.z),
        glm::vec3(max.x, max.y, max.z),
        glm::vec3(min.x, max.y, max.z)
    };

    GLuint indices[24] = {
        0, 1, 1, 2, 2, 3, 3, 0,  // 앞면
        4, 5, 5, 6, 6, 7, 7, 4,  // 뒷면
        0, 4, 1, 5, 2, 6, 3, 7   // 앞면-뒷면 연결
    };

    // VAO/VBO/EBO 설정
    GLuint vao, vbo, ebo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(corners), corners, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // 셰이더 사용 및 유니폼 설정
    glUseProgram(shaderProgram);

    GLint isRigidBodyLoc = glGetUniformLocation(shaderProgram, "isRigidBody");
    glUniform1i(isRigidBodyLoc, 1); // Rigid body 렌더링 활성화

    // AABB 박스 렌더링
    glBindVertexArray(vao);
    glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, 0);

    // 상태 복원 및 리소스 정리
    glUniform1i(isRigidBodyLoc, 0);
    glBindVertexArray(0);
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
}
