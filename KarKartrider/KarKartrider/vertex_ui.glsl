#version 330 core

layout (location = 0) in vec3 in_Position;  // 정점 위치
layout (location = 2) in vec2 aTexCoords;  // 텍스처 좌표

out vec2 TexCoords;    // 텍스처 좌표를 프래그먼트 셰이더로 전달

uniform mat4 model;              // 모델 변환 행렬

void main() {
    // UI 모드: 정점 위치를 직접 클립 공간으로 설정
    gl_Position = vec4(in_Position.xy, 0.0, 1.0); // z = 0.0, w = 1.0로 2D 위치 고정
    // 텍스처 좌표는 UI와 월드 렌더링 모두 동일
    TexCoords = aTexCoords;
}
