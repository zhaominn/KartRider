#version 330 core

layout (location = 0) in vec3 in_Position;  // 정점 위치
layout (location = 1) in vec3 in_Normal;    // 정점 법선
layout (location = 2) in vec2 aTexCoords;  // 텍스처 좌표 (옵션)
layout (location = 4) in vec3 in_Color;     // 정점 색상 (추가)

out vec3 Color;        // 프래그먼트 셰이더로 전달될 색상
out vec2 TexCoords;    // 텍스처 좌표를 프래그먼트 셰이더로 전달

uniform mat4 model;              // 모델 변환 행렬
uniform bool isRed;          // UI 모드 플래그
uniform bool isTimer;          // UI 모드 플래그
uniform bool isTextrue;

void main() {
    // gl_Position을 항상 2D 화면 공간으로 고정
    gl_Position = vec4(in_Position.xy, 0.0, 1.0); // z = 0.0, w = 1.0로 고정

    // isRed 모드에서는 색상 전달
    if (isRed) {
        Color = in_Color;
    }
    // isTimer 모드에서는 타이머 색상 전달
    else if (isTimer) {
        Color = in_Color; 
    }
    // 텍스처 모드일 경우 텍스처 좌표 전달
    else if (isTextrue) {
        TexCoords = aTexCoords; // 텍스처 좌표를 프래그먼트 셰이더로 전달
    }
}
