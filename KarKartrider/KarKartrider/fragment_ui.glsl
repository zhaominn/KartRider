#version 330 core

in vec3 FragPos;      // 월드 공간에서의 위치 (일반 3D 렌더링에만 사용)
in vec3 Normal;       // 법선 벡터 (일반 3D 렌더링에만 사용)
in vec2 TexCoords;    // 텍스처 좌표

out vec4 FragColor;

uniform sampler2D texture1; // 텍스처 (UI 또는 월드)
uniform bool isUI;          // UI 모드 플래그

void main() {
   FragColor = texture(texture1, TexCoords);
}
