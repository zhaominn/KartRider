class CustomContactResultCallback : public btCollisionWorld::ContactResultCallback {
public:
    bool hitDetected = false;                // 충돌 여부 플래그
    btVector3 collisionPointA;               // 충돌 지점 A
    btVector3 collisionPointB;               // 충돌 지점 B
    btVector3 collisionNormal;               // 충돌 법선 벡터
    float penetrationDepth = 0.0f;           // 침투 깊이

    // 충돌이 발생할 때마다 호출되는 함수
    virtual btScalar addSingleResult(btManifoldPoint& cp,
        const btCollisionObjectWrapper* colObj0Wrap,
        int partId0, int index0,
        const btCollisionObjectWrapper* colObj1Wrap,
        int partId1, int index1) override
    {
        hitDetected = true;

        // 충돌 지점 A와 B를 기록
        collisionPointA = cp.getPositionWorldOnA();
        collisionPointB = cp.getPositionWorldOnB();

        // 충돌 법선 벡터와 침투 깊이 저장
        collisionNormal = cp.m_normalWorldOnB;
        penetrationDepth = cp.getDistance(); // m_distance1은 getDistance()로 접근 가능

        return 0;
    }

    // 충돌 여부를 리셋하는 함수
    void reset() {
        hitDetected = false;
        penetrationDepth = 0.0f;
    }
};