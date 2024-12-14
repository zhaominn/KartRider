#version 330 core

layout (location = 0) in vec3 in_Position;  // 정점 위치
layout (location = 1) in vec3 in_Normal;    // 정점 법선
layout (location = 2) in vec2 aTexCoords;  // 텍스처 좌표 (옵션)
layout (location = 4) in vec3 in_Color;     // 정점 색상 (추가)

out vec3 Color;        // 프래그먼트 셰이더로 전달될 색상
out vec3 FragPos;      // 프래그먼트 위치 (world space)
out vec3 Normal;       // 프래그먼트의 법선 벡터 (world space)
out vec2 TexCoords;    // 텍스처 좌표를 프래그먼트 셰이더로 전달

uniform mat4 model;              // 모델 변환 행렬
uniform mat4 viewTransform;      // 뷰 변환 행렬
uniform mat4 projectionTransform; // 투영 변환 행렬
uniform mat3 normalMatrix;       // 법선 변환 행렬 (C++에서 계산하여 전달)
uniform bool isUI;

void main() {
    if (isUI) {
        // 정점의 클립 공간 위치 계산 (UI 용도라면 직접 설정)
        gl_Position = vec4(in_Position.xy, 0.0, 1.0); // z = 0.0, w = 1.0로 2D 위치 고정

        // 정점 색상을 프래그먼트 셰이더로 전달
        Color = in_Color;
    }else{
          // 클립 좌표 계산
        gl_Position = projectionTransform * viewTransform * model * vec4(in_Position, 1.0);

        // 월드 공간에서의 정점 위치 계산
        FragPos = vec3(model * vec4(in_Position, 1.0));

        // 법선 벡터를 법선 행렬로 변환
        Normal = normalize(normalMatrix * in_Normal);

        // 텍스처 좌표 전달
        TexCoords = aTexCoords;
    }
}
