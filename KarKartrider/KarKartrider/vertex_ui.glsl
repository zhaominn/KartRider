#version 330 core

layout (location = 0) in vec3 in_Position;  // 정점 위치
layout (location = 1) in vec3 in_Color;     // 정점 색상 (추가)
layout (location = 2) in vec2 aTexCoords;  // 텍스처 좌표 (옵션)

out vec3 Color;        // 프래그먼트 셰이더로 전달될 색상
out vec2 TexCoords;    // 텍스처 좌표 (선택 사항)

uniform mat4 model; // 모델 변환 행렬 (필요 없는 경우 삭제 가능)

void main() {
    // 정점의 클립 공간 위치 계산 (UI 용도라면 직접 설정)
    gl_Position = vec4(in_Position.xy, 0.0, 1.0); // z = 0.0, w = 1.0로 2D 위치 고정

    // 정점 색상을 프래그먼트 셰이더로 전달
    Color = in_Color;
}
