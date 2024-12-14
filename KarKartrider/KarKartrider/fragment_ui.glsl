#version 330 core

in vec3 Color;         // 버텍스 셰이더에서 전달받은 색상
in vec2 TexCoords;     // 텍스처 좌표

out vec4 FragColor;    // 최종 출력 색상

uniform sampler2D texture1; // 텍스처 샘플러
uniform bool isRed;         // UI 모드 플래그
uniform bool isTimer;       // 타이머 플래그
uniform bool isTexture;     // 텍스처 플래그

void main() {
    if (isRed) {
        FragColor = vec4(1.0, 0.0, 0.0, 1.0); // 빨간색
    } else if (isTimer) {
        FragColor = vec4(1.0, 1.0, 1.0, 1.0); // 흰색
    } else if (isTexture) {
        FragColor = texture(texture1, TexCoords); // 텍스처 샘플링
    } else {
        FragColor = vec4(0.0, 0.0, 0.0, 1.0); // 기본값 (검정색)
    }
}
