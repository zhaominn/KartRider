#pragma once
#include "include/btBulletCollisionCommon.h"
#include "include/btBulletDynamicsCommon.h"
#include <iostream>

// 충돌 결과를 저장할 Custom Callback
class CustomContactResultCallback : public btCollisionWorld::ContactResultCallback {
public:
    bool hitDetected = false;                // 충돌 여부 플래그
    btVector3 collisionPointA;               // 충돌 지점 A
    btVector3 collisionPointB;               // 충돌 지점 B

    // 충돌이 발생할 때마다 호출되는 함수
    virtual btScalar addSingleResult(btManifoldPoint& cp,
        const btCollisionObjectWrapper* colObj0Wrap,
        int partId0, int index0,
        const btCollisionObjectWrapper* colObj1Wrap,
        int partId1, int index1) override
    {
        // 충돌 감지 시 hitDetected를 true로 설정
        hitDetected = true;

        // 충돌 지점 A와 B를 기록
        collisionPointA = cp.getPositionWorldOnA();
        collisionPointB = cp.getPositionWorldOnB();

        // 콘솔에 충돌 위치를 출력
        /*std::cout << "Collision detected at: "
            << "Point A(" << collisionPointA.getX() << ", " << collisionPointA.getY() << ", " << collisionPointA.getZ() << ") - "
            << "Point B(" << collisionPointB.getX() << ", " << collisionPointB.getY() << ", " << collisionPointB.getZ() << ")"
            << std::endl;*/

        return 0; // 반환값은 특별히 의미가 없으므로 0 반환
    }

    // 충돌 여부를 리셋하는 함수
    void reset() {
        hitDetected = false;
    }
};