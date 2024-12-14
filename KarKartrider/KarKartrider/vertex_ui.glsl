#version 330 core

layout (location = 0) in vec3 in_Position;  // 정점 위치
layout (location = 1) in vec3 in_Normal;    // 정점 법선
layout (location = 2) in vec2 aTexCoords;  // 텍스처 좌표 (옵션)
layout (location = 4) in vec3 in_Color;     // 정점 색상 (추가)

out vec3 Color;        // 프래그먼트 셰이더로 전달될 색상
out vec2 TexCoords;    // 텍스처 좌표를 프래그먼트 셰이더로 전달

uniform bool isRed;          // UI 모드 플래그
uniform bool isTimer;        // UI 모드 플래그
uniform bool isTexture;      // 텍스처 활성화 플래그

void main() {
    // 화면 공간 위치 고정
    gl_Position = vec4(in_Position.xy, 0.0, 1.0);

    // 플래그에 따라 색상 또는 텍스처 좌표 설정
    if (isRed) {
        Color = in_Color;
        TexCoords = vec2(0.0, 0.0); // 기본 좌표 (필요 없는 경우 0으로 설정)
    } else if (isTimer) {
        Color = in_Color;
        TexCoords = vec2(0.0, 0.0); // 기본 좌표
    } else if (isTexture) {
        TexCoords = aTexCoords; // 텍스처 좌표를 올바르게 전달
        Color = vec3(1.0, 1.0, 1.0); // 텍스처 모드에서는 기본 흰색
    } else {
        // 기본값 설정
        TexCoords = vec2(0.0, 0.0);
        Color = vec3(0.0, 0.0, 0.0); // 검정색
    }
}
